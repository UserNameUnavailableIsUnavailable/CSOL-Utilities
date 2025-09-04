#include "Utilities.hpp"
#include "Exception.hpp"
#include "OCR.hpp"

namespace CSOL_Utilities
{
	CRNN::CRNN(std::unique_ptr<CRNN_Setting> crnn_setting) :
		m_Setting(std::move(crnn_setting))
	{
		if (!std::filesystem::is_regular_file(m_Setting->m_ModelPath))
		{
			throw Exception(Translate("CRNN::ERROR_FileNotFound@1", ConvertUtf16ToUtf8(m_Setting->m_ModelPath)));
		}
		if (!std::filesystem::is_regular_file(m_Setting->m_DictionaryPath))
		{
			throw Exception(Translate("CRNN::ERROR_FileNotFound@1", ConvertUtf16ToUtf8(m_Setting->m_DictionaryPath)));
		}
		
		/* 并行推理 */
		sessionOptions.SetExecutionMode(ExecutionMode::ORT_PARALLEL);
		
		/* 禁用空转，降低开销 */
		sessionOptions.AddConfigEntry(kOrtSessionOptionsConfigAllowIntraOpSpinning, "0");
		sessionOptions.AddConfigEntry(kOrtSessionOptionsConfigAllowInterOpSpinning, "0");
		
		/* 图优化级别拉满 */
		sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

		session = std::make_unique<Ort::Session>(env, m_Setting->m_ModelPath.wstring().c_str(), sessionOptions);

		inputNamesPtr = GetInputNames(*session);
		outputNamesPtr = GetOutputNames(*session);
		std::ifstream in(m_Setting->m_DictionaryPath);
		if (in)
		{
			std::string line;
			keys.emplace_back("#");
			while (getline(in, line))
			{
				keys.emplace_back(line);
			}
			keys.emplace_back(" ");
		}
	}

	template <class ForwardIterator>
	inline static size_t argmax(ForwardIterator first, ForwardIterator last)
	{
		return std::distance(first, std::max_element(first, last));
	}

	CRNN::~CRNN()
	{
		inputNamesPtr.clear();
		outputNamesPtr.clear();
	}

	OCR_RecognitionResult CRNN::scoreToTextLine(const std::vector<float>& outputData, size_t h, size_t w)
	{
		auto keySize = keys.size();
		auto dataSize = outputData.size();
		std::string text;
		std::vector<float> Confidences;
		size_t lastIndex = 0;
		size_t maxIndex;
		float maxValue;

		for (size_t i = 0; i < h; i++)
		{
			size_t start = i * w;
			size_t stop = (i + 1) * w;
			if (stop > dataSize - 1)
			{
				stop = (i + 1) * w - 1;
			}
			maxIndex = int(argmax(&outputData[start], &outputData[stop]));
			maxValue = float(*std::max_element(&outputData[start], &outputData[stop]));

			if (maxIndex > 0 && maxIndex < keySize && (!(i > 0 && maxIndex == lastIndex)))
			{
				Confidences.emplace_back(maxValue);
				text.append(keys[maxIndex]);
			}
			lastIndex = maxIndex;
		}
		return {text, Confidences};
	}

	OCR_RecognitionResult CRNN::Run(const cv::Mat& src)
	{
		float scale = (float)dstHeight / (float)src.rows;
		int dstWidth = int((float)src.cols * scale);
		cv::Mat srcResize;
		resize(src, srcResize, cv::Size(dstWidth, dstHeight));
		std::vector<float> inputTensorValues = SubstractMeanNormalize(srcResize, meanValues, normValues);
		std::array<int64_t, 4> inputShape{1, srcResize.channels(), srcResize.rows, srcResize.cols};
		auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
		Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
			memoryInfo, inputTensorValues.data(), inputTensorValues.size(), inputShape.data(), inputShape.size());
		assert(inputTensor.IsTensor());
		std::vector<const char*> inputNames = {inputNamesPtr.data()->get()};
		std::vector<const char*> outputNames = {outputNamesPtr.data()->get()};
		auto outputTensor = session->Run(Ort::RunOptions{nullptr}, inputNames.data(), &inputTensor, inputNames.size(),
										 outputNames.data(), outputNames.size());
		assert(outputTensor.size() == 1 && outputTensor.front().IsTensor());
		std::vector<int64_t> outputShape = outputTensor[0].GetTensorTypeAndShapeInfo().GetShape();
		int64_t outputCount = std::accumulate(outputShape.begin(), outputShape.end(), 1, std::multiplies<int64_t>());
		float* floatArray = outputTensor.front().GetTensorMutableData<float>();
		std::vector<float> outputData(floatArray, floatArray + outputCount);
		return scoreToTextLine(outputData, outputShape[1], outputShape[2]);
	}

	// std::vector<TextLine> CRNN::GetTextLines(std::vector<cv::Mat>& partImg)
	// {
	// 	int size = partImg.size();
	// 	std::vector<TextLine> textLines(size);
	// 	for (int i = 0; i < size; ++i)
	// 	{

	// 		// getTextLine
	// 		TextLine textLine = GetTextLine(partImg[i]);
	// 		textLines[i] = textLine;
	// 	}
	// 	return textLines;
	// }
} // namespace CSOL_Utilities
