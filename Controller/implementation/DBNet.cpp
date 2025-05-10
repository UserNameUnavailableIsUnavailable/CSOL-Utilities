#include <filesystem>
#include <numeric>
#include <opencv2/imgproc.hpp>
#include "Exception.hpp"
#include "OCR.hpp"
#include "Utilities.hpp"

namespace CSOL_Utilities
{
	DBNet::DBNet(std::filesystem::path dbnet_model_path, int nThreads) : numThread(nThreads)
	{
		if (!std::filesystem::is_regular_file(dbnet_model_path))
		{
			throw Exception(Translate("DBNet::ERROR_FileNotFound@1", dbnet_model_path.string()));
		}
		sessionOptions.SetInterOpNumThreads(numThread);
		sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
		session = std::make_unique<Ort::Session>(env, dbnet_model_path.wstring().c_str(), sessionOptions);
		inputNamesPtr = GetInputNames(*session);
		outputNamesPtr = GetOutputNames(*session);
	}

	DBNet::~DBNet()
	{
		inputNamesPtr.clear();
		outputNamesPtr.clear();
	}

	std::vector<TextBox> findRsBoxes(const cv::Mat& predMat, const cv::Mat& dilateMat, ScaleParam& s,
									 const float boxScoreThresh, const float unClipRatio)
	{
		const int longSideThresh = 3;
		const int maxCandidates = 1000;

		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;

		cv::findContours(dilateMat, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

		size_t numContours = contours.size() >= maxCandidates ? maxCandidates : contours.size();

		std::vector<TextBox> rsBoxes;

		for (size_t i = 0; i < numContours; i++)
		{
			if (contours[i].size() <= 2)
			{
				continue;
			}
			cv::RotatedRect minAreaRect = cv::minAreaRect(contours[i]);

			float longSide;
			std::vector<cv::Point2f> minBoxes = GetMinBoxes(minAreaRect, longSide);

			if (longSide < longSideThresh)
			{
				continue;
			}

			float boxScore = BoxScoreFast(minBoxes, predMat);
			if (boxScore < boxScoreThresh)
				continue;

			//-----unClip-----
			cv::RotatedRect clipRect = Unclip(minBoxes, unClipRatio);
			if (clipRect.size.height < 1.001 && clipRect.size.width < 1.001)
			{
				continue;
			}
			//-----unClip-----

			std::vector<cv::Point2f> clipMinBoxes = GetMinBoxes(clipRect, longSide);
			if (longSide < longSideThresh + 2)
				continue;

			std::vector<cv::Point> intClipMinBoxes;

			for (auto& clipMinBox : clipMinBoxes)
			{
				float x = clipMinBox.x / s.ratioWidth;
				float y = clipMinBox.y / s.ratioHeight;
				int ptX = (std::min)((std::max)(int(x), 0), s.srcWidth - 1);
				int ptY = (std::min)((std::max)(int(y), 0), s.srcHeight - 1);
				cv::Point point{ptX, ptY};
				intClipMinBoxes.push_back(point);
			}
			rsBoxes.push_back(TextBox{intClipMinBoxes, boxScore});
		}
		reverse(rsBoxes.begin(), rsBoxes.end());
		return rsBoxes;
	}


	std::vector<TextBox> DBNet::getTextBoxes(cv::Mat& src, ScaleParam& s, float boxScoreThresh, float boxThresh,
											 float unClipRatio)
	{
		cv::Mat srcResize;
		resize(src, srcResize, cv::Size(s.dstWidth, s.dstHeight));
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

		//-----Data preparation-----
		int outHeight = (int)outputShape[2];
		int outWidth = (int)outputShape[3];
		size_t area = outHeight * outWidth;

		std::vector<float> predData(area, 0.0);
		std::vector<unsigned char> cbufData(area, ' ');

		for (int i = 0; i < area; i++)
		{
			predData[i] = float(outputData[i]);
			cbufData[i] = (unsigned char)((outputData[i]) * 255);
		}

		cv::Mat predMat(outHeight, outWidth, CV_32F, (float*)predData.data());
		cv::Mat cBufMat(outHeight, outWidth, CV_8UC1, (unsigned char*)cbufData.data());

		//-----boxThresh-----
		const double maxValue = 255;
		const double threshold = boxThresh * 255;
		cv::Mat thresholdMat;
		cv::threshold(cBufMat, thresholdMat, threshold, maxValue, cv::THRESH_BINARY);

		//-----dilate-----
		cv::Mat dilateMat;
		cv::Mat dilateElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
		cv::dilate(thresholdMat, dilateMat, dilateElement);

		return findRsBoxes(predMat, dilateMat, s, boxScoreThresh, unClipRatio);
	}
} // namespace CSOL_Utilities
