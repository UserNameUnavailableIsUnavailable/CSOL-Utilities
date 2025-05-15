#include "pch.hpp"

#include "OCR.hpp"
#include "Console.hpp"

namespace CSOL_Utilities
{
	OCR::OCR(
		std::unique_ptr<OCR_Setting> ocr_setting,
		std::unique_ptr<DBNet_Setting> dbnet_setting, std::unique_ptr<CRNN_Setting> crnn_Setting) :
		m_Setting(std::move(ocr_setting)),
		m_DBNet(std::move(dbnet_setting)),
		m_CRNN(std::move(crnn_Setting))
	{
	}

	cv::Mat OCR::PreprocessImage(const cv::Mat& image, OCR_PreprocessInformation& preprocess_information)
	{
		/* 填充 */
		
		cv::Mat padded_image;
		auto padding = preprocess_information.m_Padding;
		cv::copyMakeBorder(image, padded_image, padding, padding, padding, padding, cv::BORDER_ISOLATED, cv::Scalar{ 0, 0, 0 });

		preprocess_information.m_IsWidthScaled = false;
		preprocess_information.m_OriginalWidth = padded_image.cols;
		preprocess_information.m_OriginalHeight = padded_image.rows;
		preprocess_information.m_ScaledWidth = padded_image.cols;
		preprocess_information.m_ScaledHeight = padded_image.rows;
		preprocess_information.m_WidthScaleRatio = 1.0f;

		int long_side_length = std::max(padded_image.cols, padded_image.rows); /* 长边 */

		float basic_scale_ratio = 1.0f;

		/* 缩放 */
		auto max_side_length = preprocess_information.m_MaxSideLength;
		if (long_side_length > max_side_length && max_side_length > 0)
		{
			basic_scale_ratio = static_cast<float>(max_side_length) / static_cast<float>(long_side_length);
			preprocess_information.m_IsWidthScaled = true;
			preprocess_information.m_IsHeightScaled = true;
			preprocess_information.m_ScaledWidth = static_cast<int>(basic_scale_ratio * preprocess_information.m_OriginalWidth);
			preprocess_information.m_ScaledHeight = static_cast<int>(basic_scale_ratio * preprocess_information.m_OriginalHeight);
			preprocess_information.m_WidthScaleRatio = basic_scale_ratio;
		}

		/* 边长对齐为 32 的整数倍，加速推理 */

		if (preprocess_information.m_ScaledWidth % 32 != 0)
		{
			preprocess_information.m_IsWidthScaled = true;
			preprocess_information.m_ScaledWidth = preprocess_information.m_ScaledWidth / 32 * 32;
			preprocess_information.m_WidthScaleRatio = static_cast<float>(preprocess_information.m_ScaledWidth) / static_cast<float>(preprocess_information.m_OriginalWidth);
		}

		if (preprocess_information.m_ScaledHeight % 32 != 0)
		{
			preprocess_information.m_IsHeightScaled = true;
			preprocess_information.m_ScaledHeight = preprocess_information.m_ScaledHeight / 32 * 32;
			preprocess_information.m_HeightScaleRatio = static_cast<float>(preprocess_information.m_ScaledHeight) / static_cast<float>(preprocess_information.m_OriginalHeight);
		}

		bool scaled = preprocess_information.m_IsWidthScaled || preprocess_information.m_IsHeightScaled;


		if (!scaled)
		{
			return padded_image;
		}
		else
		{
			cv::Mat scaled_image;
			cv::resize(padded_image, scaled_image, cv::Size(preprocess_information.m_ScaledWidth, preprocess_information.m_ScaledHeight));
			return scaled_image;
		}
	}
	std::vector<OCR_Result> OCR::Detect(const cv::Mat& image)
	{
		OCR_PreprocessInformation preprocess_information {
			.m_Padding = m_Setting->m_Padding,
			.m_MaxSideLength = m_Setting->m_MaxSideLength
		};

		auto preprocessed_image = PreprocessImage(image, preprocess_information);
		#ifdef _DEBUG
		cv::imwrite("preprocess.bmp", preprocessed_image); /* 预处理图像 */
		#endif
		auto detection_results = m_DBNet.Run(preprocessed_image);
		#ifdef _DEBUG
		cv::Mat annotated_image;
		preprocessed_image.copyTo(annotated_image);
		for (const auto& det : detection_results)
		{
			DrawTextBox(annotated_image, det.m_RoI);
		}
		cv::imwrite("initial_annotation.bmp", annotated_image); /* 原始标注 */
		#endif

		std::vector<OCR_Result> ocr_results;
	
		for (auto& detection_result : detection_results)
		{
			/* 检测框置信度需超过阈值 */
			if (detection_result.m_ROIConfidence < m_Setting->m_DetectionConfidenceThreshold)
			{
				continue;
			}
			int left = detection_result.m_RoI[0].x;
			int top = detection_result.m_RoI[0].y;
			int right = detection_result.m_RoI[0].x;
			int bottom = detection_result.m_RoI[0].y;
			/* 还原 ROI 在原图中的坐标 */
			for (int i = 0; i < 4; i++)
			{
				/* 根据缩放情况确定 ROI 在填充图中的坐标 */
				detection_result.m_RoI[i].x = preprocess_information.m_IsWidthScaled ? detection_result.m_RoI[i].x / preprocess_information.m_WidthScaleRatio : detection_result.m_RoI[i].x;
				detection_result.m_RoI[i].y = preprocess_information.m_IsHeightScaled ? detection_result.m_RoI[i].y / preprocess_information.m_HeightScaleRatio : detection_result.m_RoI[i].y;
				/* 去除填充 */
				detection_result.m_RoI[i].x = std::clamp(detection_result.m_RoI[i].x - m_Setting->m_Padding, 0, preprocess_information.m_OriginalWidth - 1);
				detection_result.m_RoI[i].y = std::clamp(detection_result.m_RoI[i].y - m_Setting->m_Padding, 0, preprocess_information.m_OriginalHeight - 1);
				left = std::min(detection_result.m_RoI[i].x, left);
				top = std::min(detection_result.m_RoI[i].y, top);
				right = std::max(detection_result.m_RoI[i].x, right);
				bottom = std::max(detection_result.m_RoI[i].y, bottom);				
			}
            /* 网格对齐矩形比原始矩形要大，需要确保其位于图像内 */
			right = std::clamp(right, 0, image.cols);
			bottom = std::clamp(bottom, 0, image.rows);
			left = std::clamp(left, 0, right);
			top = std::clamp(top, 0, bottom);
			auto axis_aligned_roi = image(cv::Rect(left, top, right - left, bottom - top)); /* 网格对齐的、包含 RoI 的矩形框 */
			auto recognition_result = m_CRNN.Run(axis_aligned_roi);

			/* 这里需要判空，否则计算 mean_confidence 会出现除零的情况，导致结果无穷大 */
            /* 同时，也可以剔除未识别到任何字符的冗余结果 */
			if (recognition_result.m_CharConfidences.empty())
			{
				continue;
			}
			auto max_confidence = *std::ranges::max_element(recognition_result.m_CharConfidences.begin(), recognition_result.m_CharConfidences.end());
			if (max_confidence < m_Setting->m_RecognitionConfidenceThreshold) /* 识别结果最大置信度需超过阈值，即识别的文本中需要有至少一个字符具有很高的置信度 */
			{
				continue;
			}
			ocr_results.emplace_back(OCR_Result{
					std::move(detection_result),
	std::move(recognition_result)
			});
		}
		#ifdef _DEBUG
		for (const auto& r : ocr_results)
		{
			cv::Mat filtered_annotation(image);
			DrawTextBox(filtered_annotation, r.m_DetectionResult.m_RoI);
			cv::imwrite("final_annotation.bmp", filtered_annotation); /* 最终标注 */
			Console::Debug(std::format("识别结果：{}", r.m_ReognitionResult.m_Text));
		}
		#endif
		return ocr_results;
	}
	cv::Scalar ComputeMeanInROI(const cv::Mat& mat, const std::vector<std::vector<cv::Point>>& roi)
	{
		cv::Mat roi_mask = cv::Mat::zeros(mat.size(), CV_8UC1);
		cv::fillPoly(roi_mask, roi,  cv::Scalar(255));
		return cv::mean(mat, roi_mask);
	}
} // namespace CSOL_Utilities
