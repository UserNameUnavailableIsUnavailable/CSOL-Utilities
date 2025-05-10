#include <numeric>
#include <opencv2/imgproc.hpp>
#include "OCR.hpp"

namespace CSOL_Utilities
{
	AngleNet::AngleNet(std::filesystem::path angle_model_path, int nThreads) : m_number_of_threads(nThreads)
	{
		sessionOptions.SetInterOpNumThreads(nThreads);
		sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
		session = std::make_unique<Ort::Session>(Ort::Session(env, angle_model_path.wstring().c_str(), sessionOptions));
		inputNamesPtr = GetInputNames(*session);
		outputNamesPtr = GetOutputNames(*session);
	}

	AngleNet::~AngleNet()
	{
		inputNamesPtr.clear();
		outputNamesPtr.clear();
	}

	Angle scoreToAngle(const std::vector<float>& outputData)
	{
		int maxIndex = 0;
		float maxScore = -1000.0f;
		for (int i = 0; i < outputData.size(); i++)
		{
			if (i == 0)
				maxScore = outputData[i];
			else if (outputData[i] > maxScore)
			{
				maxScore = outputData[i];
				maxIndex = i;
			}
		}
		return {maxIndex, maxScore};
	}

	Angle AngleNet::getAngle(cv::Mat& src)
	{

		std::vector<float> inputTensorValues = SubstractMeanNormalize(src, meanValues, normValues);
		std::array<int64_t, 4> inputShape{1, src.channels(), src.rows, src.cols};
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
		return scoreToAngle(outputData);
	}

	std::vector<Angle> AngleNet::GetAngles(std::vector<cv::Mat>& imgs, bool doAngle, bool mostAngle)
	{
		size_t size = imgs.size();
		std::vector<Angle> angles(size);
		if (doAngle)
		{
			for (size_t i = 0; i < size; ++i)
			{
				cv::Mat angleImg;
				cv::resize(imgs[i], angleImg, cv::Size(dstWidth, dstHeight));
				Angle angle = getAngle(angleImg);

				angles[i] = angle;
			}
		}
		else
		{
			for (size_t i = 0; i < size; ++i)
			{
				angles[i] = Angle{-1, 0.f};
			}
		}
		// Most Possible AngleIndex
		if (doAngle && mostAngle)
		{
			auto angleIndexes = GetAngleIndexes(angles);
			double sum = std::accumulate(angleIndexes.begin(), angleIndexes.end(), 0.0);
			double halfPercent = angles.size() / 2.0f;
			int mostAngleIndex;
			if (sum < halfPercent)
			{ // all angle set to 0
				mostAngleIndex = 0;
			}
			else
			{ // all angle set to 1
				mostAngleIndex = 1;
			}
			for (size_t i = 0; i < angles.size(); ++i)
			{
				Angle angle = angles[i];
				angle.index = mostAngleIndex;
				angles.at(i) = angle;
			}
		}

		return angles;
	}
} // namespace CSOL_Utilities
