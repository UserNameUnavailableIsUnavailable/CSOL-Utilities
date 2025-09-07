#include "Exception.hpp"
#include "OCR.hpp"
#include "Utilities.hpp"
#include "ModelUtilities.hpp"

using namespace CSOL_Utilities;

OCR_Detector::OCR_Detector(std::filesystem::path json_path)
{
	if (!std::filesystem::is_regular_file(json_path))
	{
		throw Exception(Translate("ERROR_FileNotFound@1", ConvertUtf16ToUtf8(json_path)));
	}
	std::ifstream in(json_path);
	if (!in)
	{
		auto iostate = static_cast<std::size_t>(in.rdstate());
		throw Exception(
			Translate("ERROR_FileStream@2", ConvertUtf16ToUtf8(json_path),
					  std::format("std::ifstream failed with IO state {:#x} ({}:{})", iostate, __FILE__, __LINE__)));
	}
	nlohmann::json metadata_json;
	in >> metadata_json;
	// JSON 样例
	// {
	//     "model_path": "dbnet.onnx",
	//     "dilation_offset_ratio": 1.5,
	//     "binarization_probability_threshold": 0.2,
	//     "confidence_threshold": 0.6,
	//	   "mean": [123.675, 116.28, 103.53],
	//	   "norm": [0.0171247538316637, 0.0158127767235927, 0.0174291938997821]
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
	if (metadata_json.contains("dilation_offset_ratio")) // optional
	{
		dilation_offset_ratio_ = metadata_json["dilation_offset_ratio"].get<float>();
	}
	if (metadata_json.contains("binarization_probability_threshold")) // optional
	{
		binarization_probability_threshold_ = metadata_json["binarization_probability_threshold"].get<float>();
	}
	if (metadata_json.contains("confidence_threshold")) // optional
	{
		confidence_threshold_ = metadata_json["confidence_threshold"].get<float>();
	}
	if (metadata_json.contains("mean")) // optional
	{
		auto mean_array = metadata_json["mean"].get<std::vector<float>>();
		for (size_t i = 0; i < 3 && i < mean_array.size(); ++i)
		{
			mean_[i] = mean_array[i];
		}
	}
	if (metadata_json.contains("norm")) // optional
	{
		auto norm_array = metadata_json["norm"].get<std::vector<float>>();
		for (size_t i = 0; i < 3 && i < norm_array.size(); ++i)
		{
			norm_[i] = norm_array[i];
		}
	}
	/* 并行推理 */
	sessionOptions.SetExecutionMode(ExecutionMode::ORT_PARALLEL);

	/* 禁用空转，降低开销 */
	sessionOptions.AddConfigEntry(kOrtSessionOptionsConfigAllowIntraOpSpinning, "0");
	sessionOptions.AddConfigEntry(kOrtSessionOptionsConfigAllowInterOpSpinning, "0");

	/* 图优化级别拉满 */
	sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
	env_ = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_ERROR, model_path_.stem().string().c_str());
	session = std::make_unique<Ort::Session>(*env_, model_path_.wstring().c_str(), sessionOptions);
	input_names_ = GetInputNames(*session);
	output_names_ = GetOutputNames(*session);
}

std::vector<std::array<cv::Point, 4>> OCR_Detector::MakeDetectionResult(
	const cv::Mat& possibility_map,
	const cv::Mat& dilated_image
) {
	constexpr const int MIN_SIDE_LENGTH = 5;
	constexpr const auto MAX_CANDIDATES_COUNT = 1000;

	using Polygon = std::vector<cv::Point>;

	std::vector<Polygon> contours; /* 轮廓点列 */

	cv::findContours(dilated_image, contours, /* hierarchy,  */ cv::RETR_LIST,
					 cv::CHAIN_APPROX_SIMPLE); /* 提取文字轮廓 */

	size_t contours_count = contours.size() >= MAX_CANDIDATES_COUNT ? MAX_CANDIDATES_COUNT : contours.size();

	std::vector<std::array<cv::Point, 4>> detection_result;

	for (size_t i = 0; i < contours_count; i++)
	{
		if (contours[i].size() < 3) /* 轮廓点集合太小，不足以构成平面图形 */
		{
			continue;
		}

		auto minAreaRect = cv::minAreaRect(contours[i]); /* 包围由该轮廓所构成的图形的最小矩形 */
		if (std::max(minAreaRect.size.width, minAreaRect.size.height) < MIN_SIDE_LENGTH)
		{
			continue;
		}

		std::vector<std::vector<cv::Point>> roi{
			std::ref(contours[i]) /* 使用 std::ref 避免多余拷贝 */
		};
		auto confidence = static_cast<float>(ComputeMeanInROI(possibility_map, roi)[0]);

		std::array<cv::Point2f, 4> vertices2f;
		minAreaRect.points(vertices2f.data());

		auto delta = CalculatePolygonDilationOffset(vertices2f);

		cv::RotatedRect bounding_box = ComputePolygonBoundingBox(vertices2f, delta);

		if (std::max(bounding_box.size.width, bounding_box.size.height) <= MIN_SIDE_LENGTH)
		{
			continue;
		}

		std::array<cv::Point, 4> roi_candidate;
		bounding_box.points(vertices2f.data());

		for (int i = 0; i < 4; i++)
		{
			const auto& vertex2f = vertices2f[i];
			int x = static_cast<int>(vertex2f.x);
			int y = static_cast<int>(vertex2f.y);
			roi_candidate[i] = cv::Point(x, y);
		}
		// 根据置信度阈值筛选，提取有效的检测结果
		if (confidence > confidence_threshold_)
		{
			detection_result.emplace_back(std::move(roi_candidate));
		}
	}
	std::reverse(detection_result.begin(), detection_result.end());
	return detection_result;
}

std::vector<std::array<cv::Point, 4>> OCR_Detector::Run(const cv::Mat& image)
{
	std::vector<float> inputTensorValues = SubstractMeanNormalize(image, mean_, norm_);

	std::array<int64_t, 4> tensor_shape{1, image.channels(), image.rows, image.cols};

	auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

	Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
		memoryInfo, inputTensorValues.data(), inputTensorValues.size(), tensor_shape.data(), tensor_shape.size());

	assert(inputTensor.IsTensor());

	std::vector<const char*> inputNames = {input_names_.data()->get()};
	std::vector<const char*> outputNames = {output_names_.data()->get()};
	auto results = session->Run(Ort::RunOptions{nullptr}, inputNames.data(), &inputTensor, inputNames.size(),
								outputNames.data(), outputNames.size());
	assert(results.size() == 1 && results.front().IsTensor());

	auto tensor_info = results[0].GetTensorTypeAndShapeInfo();

	std::vector<int64_t> shape = tensor_info.GetShape();

	int64_t elements_count =
		std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<int64_t>()); /* 输出的张量中的元素个数 */
	auto raw_output_tensor =
		results.front()
			.GetTensorData<float>(); /* 模型输出概率图，对应于输入图像的每个像素点，指示其属于文字符号的概率 */

	auto height = shape[2];
	auto width = shape[3];
	auto roi_size = height * width;

	std::vector<float> probability_map_content; /* 概率图数据 */
	std::vector<unsigned char> threshold_map_content; /* 阈值图的数据 */

	for (int i = 0; i < roi_size; i++)
	{
		probability_map_content.emplace_back(raw_output_tensor[i]);
		threshold_map_content.emplace_back(raw_output_tensor[i] * 255);
	}
	cv::Mat probability_map(height, width, CV_32F, probability_map_content.data()); /* 概率图 */
	cv::Mat threshold_map(height, width, CV_8UC1, threshold_map_content.data()); /* 阈值图 */

	/* 对阈值图进行二值化，得到掩码图 */
	const double binarization_threshold = binarization_probability_threshold_ * 255;
	cv::Mat mask;
	cv::threshold(threshold_map, mask, binarization_threshold, 255.0f, cv::THRESH_BINARY); /* 二值化 */

	/* 对掩码图进行膨胀卷积 */
	cv::Mat dilated_mask;
	cv::Mat dilation_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2)); /* 膨胀卷积核 */
	cv::dilate(mask, dilated_mask, dilation_kernel);

	auto detection_results = MakeDetectionResult(probability_map, dilated_mask);
	return detection_results;
}
