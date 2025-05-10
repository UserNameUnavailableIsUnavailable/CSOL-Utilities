#include <filesystem>
#include <fstream>
#include <numeric>
#include <opencv2/imgproc.hpp>
#include "Utilities.hpp"
#include "Exception.hpp"
#include "OCR.hpp"

namespace CSOL_Utilities
{
	CRNN::CRNN(std::filesystem::path crnn_model_path, std::filesystem::path dict_file_path, int nThreads) :
		m_nThreads(nThreads)
	{
		if (!std::filesystem::is_regular_file(crnn_model_path))
		{
			throw Exception(Translate("CRNN::ERROR_FileNotFound@1", crnn_model_path.string()));
		}
		if (!std::filesystem::is_regular_file(dict_file_path))
		{
			throw Exception(Translate("CRNN::ERROR_FileNotFound@1", dict_file_path.string()));
		}
		sessionOptions.SetInterOpNumThreads(m_nThreads);
		sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
		session = std::make_unique<Ort::Session>(env, crnn_model_path.wstring().c_str(), sessionOptions);
		inputNamesPtr = GetInputNames(*session);
		outputNamesPtr = GetOutputNames(*session);
		std::ifstream in(dict_file_path);
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

	TextLine CRNN::scoreToTextLine(const std::vector<float>& outputData, size_t h, size_t w)
	{
		auto keySize = keys.size();
		auto dataSize = outputData.size();
		std::string strRes;
		std::vector<float> scores;
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
				scores.emplace_back(maxValue);
				strRes.append(keys[maxIndex]);
			}
			lastIndex = maxIndex;
		}
		return {strRes, scores};
	}

	// TextLine CRNN::scoreToTextLine(const std::vector<float> &outputData, size_t h, size_t w) {
	//     auto keySize = keys.size();
	//     std::string strRes;
	//     std::vector<float> scores;
	//     size_t lastIndex = 0;
	//     size_t maxIndex;
	//     float maxValue;

	//    for (size_t i = 0; i < h; i++) {
	//        maxIndex = 0;
	//        maxValue = -1000.f;
	//        //do softmax
	//        std::vector<float> exps(w);
	//        for (size_t j = 0; j < w; j++) {
	//            float expSingle = exp(outputData[i * w + j]);
	//            exps.at(j) = expSingle;
	//        }
	//        float partition = std::accumulate(exps.begin(), exps.end(), 0.0);//row sum
	//        maxIndex = int(argmax(exps.begin(), exps.end()));
	//        maxValue = float(*std::max_element(exps.begin(), exps.end())) / partition;
	//        if (maxIndex > 0 && maxIndex < keySize && (!(i > 0 && maxIndex == lastIndex))) {
	//            scores.emplace_back(maxValue);
	//            strRes.append(keys[maxIndex - 1]);
	//        }
	//        lastIndex = maxIndex;
	//    }
	//    return {strRes, scores};
	//}

	// TextLine CRNN::GetTextLine(const cv::Mat &src) {
	//     float scale = (float) dstHeight / (float) src.rows;
	//     int dstWidth = int((float) src.cols * scale);

	//    cv::Mat srcResize;
	//    cv::resize(src, srcResize, cv::Size(dstWidth, dstHeight));

	//    std::vector<float> inputTensorValues = SubstractMeanNormalize(srcResize, meanValues, normValues);

	//    std::array<int64_t, 4> inputShape{1, srcResize.channels(), srcResize.rows, srcResize.cols};

	//    auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

	//    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, inputTensorValues.data(),
	//    inputTensorValues.size(), inputShape.data(), inputShape.size()); assert(inputTensor.IsTensor());
	//    std::vector<const char *> inputNames = {inputNamesPtr.data()->get()};
	//    std::vector<const char *> outputNames = {outputNamesPtr.data()->get()};
	//    auto outputTensor = session->Run(Ort::RunOptions{nullptr}, inputNames.data(), &inputTensor,
	//                                    inputNames.size(), outputNames.data(), outputNames.size());
	//    assert(outputTensor.size() == 1 && outputTensor.front().IsTensor());

	//    std::vector<int64_t> outputShape = outputTensor[0].GetTensorTypeAndShapeInfo().GetShape();

	//    int64_t outputCount = std::accumulate(outputShape.begin(), outputShape.end(), 1,
	//                                        std::multiplies<int64_t>());

	//    float *floatArray = outputTensor.front().GetTensorMutableData<float>();
	//    std::vector<float> outputData(floatArray, floatArray + outputCount);
	//    return scoreToTextLine(outputData, outputShape[0], outputShape[2]);
	//}

	// std::vector<TextLine> CRNN::GetTextLines(std::vector<cv::Mat> &partImg) {
	//     int size = partImg.size();
	//     std::vector<TextLine> textLines(size);
	//     for (int i = 0; i < size; ++i)
	//     {
	//         TextLine textLine = GetTextLine(partImg[i]);
	//         textLines[i] = textLine;
	//     }
	//     return textLines;
	// }

	TextLine CRNN::GetTextLine(const cv::Mat& src)
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

	std::vector<TextLine> CRNN::GetTextLines(std::vector<cv::Mat>& partImg)
	{
		int size = partImg.size();
		std::vector<TextLine> textLines(size);
		for (int i = 0; i < size; ++i)
		{

			// getTextLine
			TextLine textLine = GetTextLine(partImg[i]);
			textLines[i] = textLine;
		}
		return textLines;
	}
} // namespace CSOL_Utilities
