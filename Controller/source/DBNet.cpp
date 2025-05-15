#include "pch.hpp"

#include "Exception.hpp"
#include "OCR.hpp"
#include "Utilities.hpp"

namespace CSOL_Utilities
{
	DBNet::DBNet(std::unique_ptr<DBNet_Setting> setting) :
		m_Setting(std::move(setting))
	{
		if (!std::filesystem::is_regular_file(ConvertUtf16ToUtf8(m_Setting->m_ModelPath.wstring())))
		{
			throw Exception(Translate("DBNet::ERROR_FileNotFound@1", ConvertUtf16ToUtf8(m_Setting->m_ModelPath.wstring())));
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
	}

	DBNet::~DBNet()
	{
	}

	std::vector<OCR_DetectionResult> DBNet::MakeDetectionResult(const cv::Mat& possibility_map, const cv::Mat& dilated_image)
	{
		constexpr const int MIN_SIDE_LENGTH = 5;
		constexpr const auto MAX_CANDIDATES_COUNT = 1000;

		using Polygon = std::vector<cv::Point>;

		std::vector<Polygon> contours; /* 轮廓点列 */

		cv::findContours(dilated_image, contours, /* hierarchy,  */cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE); /* 提取文字轮廓 */

		size_t contours_count = contours.size() >= MAX_CANDIDATES_COUNT ? MAX_CANDIDATES_COUNT : contours.size();

		std::vector<OCR_DetectionResult> detection_result;

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
			detection_result.emplace_back(OCR_DetectionResult{ roi_candidate, confidence });
		}
		std::reverse(detection_result.begin(), detection_result.end());
		return detection_result;
	}

	std::vector<OCR_DetectionResult> DBNet::Run(const cv::Mat& image)
	{
		std::vector<float> inputTensorValues = SubstractMeanNormalize(image, MEAN, NORM);

		std::array<int64_t, 4> tensor_shape{1, image.channels(), image.rows, image.cols};

		auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

		Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
			memoryInfo, inputTensorValues.data(), inputTensorValues.size(), tensor_shape.data(), tensor_shape.size());

		assert(inputTensor.IsTensor());
		
		std::vector<const char*> inputNames = {inputNamesPtr.data()->get()};
		std::vector<const char*> outputNames = {outputNamesPtr.data()->get()};
		auto results = session->Run(Ort::RunOptions{nullptr}, inputNames.data(), &inputTensor, inputNames.size(),
										 outputNames.data(), outputNames.size());
		assert(results.size() == 1 && results.front().IsTensor());

		auto tensor_info = results[0].GetTensorTypeAndShapeInfo();

		std::vector<int64_t> shape = tensor_info.GetShape();

		int64_t elements_count = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<int64_t>()); /* 输出的张量中的元素个数 */
		auto raw_output_tensor = results.front().GetTensorData<float>(); /* 模型输出概率图，对应于输入图像的每个像素点，指示其属于文字符号的概率 */

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
		const double binarization_threshold = m_Setting->m_BinarizationProbabilityThreshold * 255;
		cv::Mat mask;
		cv::threshold(threshold_map, mask, binarization_threshold, 255.0f, cv::THRESH_BINARY); /* 二值化 */
		
		/* 对掩码图进行膨胀卷积 */
		cv::Mat dilated_mask;
		cv::Mat dilation_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2)); /* 膨胀卷积核 */
		cv::dilate(mask, dilated_mask, dilation_kernel);

		auto detection_results = MakeDetectionResult(probability_map, dilated_mask);
		return detection_results;
	}
	
} // namespace CSOL_Utilities
