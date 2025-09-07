#include "OCR.hpp"

namespace CSOL_Utilities
{
	cv::Mat AdjustTargetImg(cv::Mat& src, int dstWidth, int dstHeight)
	{
		cv::Mat srcResize;
		float scale = (float)dstHeight / (float)src.rows;
		int angleWidth = int((float)src.cols * scale);
		cv::resize(src, srcResize, cv::Size(angleWidth, dstHeight));
		cv::Mat srcFit = cv::Mat(dstHeight, dstWidth, CV_8UC3, cv::Scalar(255, 255, 255));
		if (angleWidth < dstWidth)
		{
			cv::Rect rect(0, 0, srcResize.cols, srcResize.rows);
			srcResize.copyTo(srcFit(rect));
		}
		else
		{
			cv::Rect rect(0, 0, dstWidth, dstHeight);
			srcResize(rect).copyTo(srcFit);
		}
		return srcFit;
	}

	void DrawTextBox(cv::Mat& box_img, const std::array<cv::Point, 4>& box)
	{
		const auto color = cv::Scalar(255, 0, 255);
		cv::line(box_img, box[0], box[1], color, 2);
		cv::line(box_img, box[1], box[2], color, 2);
		cv::line(box_img, box[2], box[3], color, 2);
		cv::line(box_img, box[3], box[0], color, 2);
	}

	cv::Mat matRotateClockWise180(cv::Mat src)
	{
		flip(src, src, 0);
		flip(src, src, 1);
		return src;
	}

	cv::Mat matRotateClockWise90(cv::Mat src)
	{
		transpose(src, src);
		flip(src, src, 1);
		return src;
	}
} // namespace CSOL_Utilities
