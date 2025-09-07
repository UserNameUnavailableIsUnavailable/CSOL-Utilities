#include "Exception.hpp"
#include "OCR.hpp"
#include "Utilities.hpp"
#include "ModelUtilities.hpp"

using namespace CSOL_Utilities;

OCR_Recognizer::OCR_Recognizer(std::filesystem::path json_path)
{
	if (!std::filesystem::is_regular_file(json_path))
	{
		throw Exception(Translate("ERROR_FileNotFound@1", ConvertUtf16ToUtf8(json_path)));
	}
	std::ifstream json_fstream(json_path);
	if (!json_fstream)
	{
		auto iostate = static_cast<std::size_t>(json_fstream.rdstate());
		throw Exception(Translate(
			"ERROR_FileStream@2", ConvertUtf16ToUtf8(json_path),
			std::format("std::ifstream failed on {} with IO state {:#x}", ConvertUtf16ToUtf8(json_path), iostate)));
	}
	nlohmann::json metadata_json;
	json_fstream >> metadata_json;
	// JSON 样例
	// {
	//     "model_path": "crnn.onnx",
	//     "dictionary_path": "dictionary.txt",
	//     "confidence_threshold": 0.6,
	//     "mean": [127.5, 127.5, 127.5],
	//     "norm": [0.007843137, 0.007843137, 0.007843137]
	// }
	if (!metadata_json.contains("model_path")) // 模型路径为必填项
	{
		throw Exception(Translate("ERROR_MandatoryFieldMissing@1", "model_path"));
	}
	model_path_ = metadata_json["model_path"].get<std::string>();
	if (model_path_.is_relative()) // 若是相对路径，则相对于 JSON 文件所在目录
	{
		model_path_ = json_path.parent_path() / model_path_;
	}
	if (!metadata_json.contains("dictionary_path"))
	{
		throw Exception(Translate("ERROR_MandatoryFieldMissing@1", "dictionary_path"));
	}
	dictionary_path_ = metadata_json["dictionary_path"].get<std::string>();
	if (dictionary_path_.is_relative()) // 若是相对路径，则相对于 JSON 文件所在目录
	{
		dictionary_path_ = json_path.parent_path() / dictionary_path_;
	}
	if (metadata_json.contains("mean"))
	{
		auto mean_array = metadata_json["mean"].get<std::vector<float>>();
		for (size_t i = 0; i < 3 && i < mean_array.size(); ++i)
		{
			mean_[i] = mean_array[i];
		}
	}
	if (metadata_json.contains("norm"))
	{
		auto norm_array = metadata_json["norm"].get<std::vector<float>>();
		for (size_t i = 0; i < 3 && i < norm_array.size(); ++i)
		{
			norm_[i] = norm_array[i];
		}
	}
	if (metadata_json.contains("confidence_threshold"))
	{
		confidence_threshold_ = metadata_json["confidence_threshold"].get<float>();
	}
	// 加载字典
	if (!std::filesystem::is_regular_file(dictionary_path_))
	{
		throw Exception(Translate("ERROR_FileNotFound@1", ConvertUtf16ToUtf8(dictionary_path_)));
	}
	std::ifstream dict_fstream(dictionary_path_);
	if (!dict_fstream)
	{
		throw Exception(Translate("ERROR_FileStream@2", ConvertUtf16ToUtf8(dictionary_path_),
								  std::format("std::ifstream failed on {} with IO state {:#x}",
											  ConvertUtf16ToUtf8(dictionary_path_),
											  static_cast<std::size_t>(dict_fstream.rdstate()))));
	}
	std::string line;
	dictionary_.emplace_back("#");
	while (getline(dict_fstream, line))
	{
		dictionary_.emplace_back(line);
	}
	dictionary_.emplace_back(" ");
	// 加载模型
	if (!std::filesystem::is_regular_file(model_path_))
	{
		throw Exception(Translate("ERROR_FileNotFound@1", ConvertUtf16ToUtf8(model_path_)));
	}
	/* 并行推理 */
	session_opts_.SetExecutionMode(ExecutionMode::ORT_PARALLEL);

	/* 禁用空转，降低开销 */
	session_opts_.AddConfigEntry(kOrtSessionOptionsConfigAllowIntraOpSpinning, "0");
	session_opts_.AddConfigEntry(kOrtSessionOptionsConfigAllowInterOpSpinning, "0");

	/* 图优化级别拉满 */
	session_opts_.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

	env_ = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_ERROR, model_path_.stem().string().c_str());
	session = std::make_unique<Ort::Session>(*env_, model_path_.wstring().c_str(), session_opts_);

	input_names_ = GetInputNames(*session);
	output_names_ = GetOutputNames(*session);
}

template <class ForwardIterator>
inline static size_t argmax(ForwardIterator first, ForwardIterator last)
{
	return std::distance(first, std::max_element(first, last));
}

OCR_Recognizer::~OCR_Recognizer()
{
	input_names_.clear();
	output_names_.clear();
}

std::string OCR_Recognizer::ScoreToText(const std::vector<float>& score, size_t h, size_t w)
{
	auto key_size = dictionary_.size();
	auto score_size = score.size();
	std::string text;
	std::vector<float> Confidences;
	size_t lastIndex = 0;
	size_t maxIndex;
	float maxValue;

	for (size_t i = 0; i < h; i++)
	{
		size_t start = i * w;
		size_t stop = (i + 1) * w;
		if (stop > score_size - 1)
		{
			stop = (i + 1) * w - 1;
		}
		maxIndex = int(argmax(&score[start], &score[stop]));
		maxValue = float(*std::max_element(&score[start], &score[stop]));

		if (maxIndex > 0 && maxIndex < key_size && (!(i > 0 && maxIndex == lastIndex)))
		{
			Confidences.emplace_back(maxValue);
			text.append(dictionary_[maxIndex]);
		}
		lastIndex = maxIndex;
	}
	if (Confidences.empty())
	{
		return "";
	}
	auto max_confidence = *std::ranges::max_element(Confidences);
	if (max_confidence < confidence_threshold_)
	{
		return "";
	}
	return text;
}

std::string OCR_Recognizer::Run(const cv::Mat& src)
{
	float scale = (float)dstHeight / (float)src.rows;
	int dstWidth = int((float)src.cols * scale);
	cv::Mat srcResize;
	cv::resize(src, srcResize, cv::Size(dstWidth, dstHeight));
	std::vector<float> inputTensorValues = SubstractMeanNormalize(srcResize, mean_, norm_);
	std::array<int64_t, 4> inputShape{1, srcResize.channels(), srcResize.rows, srcResize.cols};
	auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
	Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
		memoryInfo, inputTensorValues.data(), inputTensorValues.size(), inputShape.data(), inputShape.size());
	assert(inputTensor.IsTensor());
	std::vector<const char*> inputNames = {input_names_.data()->get()};
	std::vector<const char*> outputNames = {output_names_.data()->get()};
	auto outputTensor = session->Run(Ort::RunOptions{nullptr}, inputNames.data(), &inputTensor, inputNames.size(),
									 outputNames.data(), outputNames.size());
	assert(outputTensor.size() == 1 && outputTensor.front().IsTensor());
	std::vector<int64_t> outputShape = outputTensor[0].GetTensorTypeAndShapeInfo().GetShape();
	int64_t outputCount = std::accumulate(outputShape.begin(), outputShape.end(), 1, std::multiplies<int64_t>());
	float* floatArray = outputTensor.front().GetTensorMutableData<float>();
	std::vector<float> outputData(floatArray, floatArray + outputCount);
	return ScoreToText(outputData, outputShape[1], outputShape[2]);
}
