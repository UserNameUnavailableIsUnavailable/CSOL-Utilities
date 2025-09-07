#include "OCR.hpp"
#include "Console.hpp"

namespace CSOL_Utilities
{
	OCR::OCR(
		OCR_Detector&& detector,
		OCR_Recognizer&& recognizer
	): detector(std::move(detector)), recognizer(std::move(recognizer))
	{
	}

	cv::Mat OCR::PreprocessImage(const cv::Mat& image, OCR_PreprocessInformation& preprocess_information)
	{
		/* 填充 */
		cv::Mat padded_image;
		auto padding = preprocess_information.padding;
		cv::copyMakeBorder(image, padded_image, padding, padding, padding, padding, cv::BORDER_ISOLATED, cv::Scalar{ 0, 0, 0 });

		preprocess_information.is_width_scaled = false;
		preprocess_information.original_width = padded_image.cols;
		preprocess_information.original_height = padded_image.rows;
		preprocess_information.scaled_width = padded_image.cols;
		preprocess_information.scaled_height = padded_image.rows;
		preprocess_information.width_scale_ratio = 1.0f;

		int long_side_length = std::max(padded_image.cols, padded_image.rows); /* 长边 */

		float basic_scale_ratio = 1.0f;

		/* 缩放 */
		auto max_side_length = preprocess_information.max_side_length;
		if (long_side_length > max_side_length && max_side_length > 0)
		{
			basic_scale_ratio = static_cast<float>(max_side_length) / static_cast<float>(long_side_length);
			preprocess_information.is_width_scaled = true;
			preprocess_information.is_height_scaled = true;
			preprocess_information.scaled_width = static_cast<int>(basic_scale_ratio * preprocess_information.original_width);
			preprocess_information.scaled_height = static_cast<int>(basic_scale_ratio * preprocess_information.original_height);
			preprocess_information.width_scale_ratio = basic_scale_ratio;
		}

		/* 边长对齐为 32 的整数倍，加速推理 */

		if (preprocess_information.scaled_width % 32 != 0)
		{
			preprocess_information.is_width_scaled = true;
			preprocess_information.scaled_width = preprocess_information.scaled_width / 32 * 32;
			preprocess_information.width_scale_ratio = static_cast<float>(preprocess_information.scaled_width) / static_cast<float>(preprocess_information.original_width);
		}

		if (preprocess_information.scaled_height % 32 != 0)
		{
			preprocess_information.is_height_scaled = true;
			preprocess_information.scaled_height = preprocess_information.scaled_height / 32 * 32;
			preprocess_information.height_scale_ratio = static_cast<float>(preprocess_information.scaled_height) / static_cast<float>(preprocess_information.original_height);
		}

		bool scaled = preprocess_information.is_width_scaled || preprocess_information.is_height_scaled;


		if (!scaled)
		{
			return padded_image;
		}
		else
		{
			cv::Mat scaled_image;
			cv::resize(padded_image, scaled_image, cv::Size(preprocess_information.scaled_width, preprocess_information.scaled_height));
			return scaled_image;
		}
	}
	std::vector<OCR_Result> OCR::Detect(const cv::Mat& image)
	{
		OCR_PreprocessInformation preprocess_information {
			.padding = padding_,
			.max_side_length = max_side_length_
		};

		auto preprocessed_image = PreprocessImage(image, preprocess_information);
		#ifdef _DEBUG
		cv::imwrite("preprocess.bmp", preprocessed_image); /* 预处理图像 */
		#endif
		auto boxes = detector.Run(preprocessed_image);
		#ifdef _DEBUG
		cv::Mat annotated_image;
		preprocessed_image.copyTo(annotated_image);
		for (const auto& box : boxes)
		{
			DrawTextBox(annotated_image, box);
		}
		cv::imwrite("initial_annotation.bmp", annotated_image); /* 原始标注 */
		#endif

		std::vector<OCR_Result> ocr_results;
	
		for (auto& box : boxes)
		{
			int left = box[0].x;
			int top = box[0].y;
			int right = box[0].x;
			int bottom = box[0].y;
			for (int i = 0; i < 4; i++)
			{
				left = std::min(box[i].x, left);
				top = std::min(box[i].y, top);
				right = std::max(box[i].x, right);
				bottom = std::max(box[i].y, bottom);				
			}
            /* 网格对齐矩形比原始矩形要大，需要确保其位于图像内 */
			right = std::clamp(right, 0, image.cols);
			bottom = std::clamp(bottom, 0, image.rows);
			left = std::clamp(left, 0, right);
			top = std::clamp(top, 0, bottom);
			auto axis_aligned_roi = preprocessed_image(cv::Rect(left, top, right - left, bottom - top)); /* 网格对齐的、包含 RoI 的矩形框 */
			#ifdef _DEBUG
			cv::imwrite("patch.bmp", axis_aligned_roi);
			#endif
			auto recognition_result = recognizer.Run(axis_aligned_roi);

			ocr_results.emplace_back(OCR_Result{
				std::move(box),
				std::move(recognition_result)
			});
		}
		#ifdef _DEBUG
		for (const auto& r : ocr_results)
		{
			cv::Mat filtered_annotation(image);
			DrawTextBox(filtered_annotation, r.box);
			cv::imwrite("final_annotation.bmp", filtered_annotation); /* 最终标注 */
			Console::Debug(std::format("识别结果：{}", r.text));
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
