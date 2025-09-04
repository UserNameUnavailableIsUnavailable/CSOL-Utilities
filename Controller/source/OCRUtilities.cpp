#include "OCR.hpp"

namespace CSOL_Utilities
{

	//std::vector<int> GetAngleIndexes(std::vector<Angle>& angles)
	//{
	//	std::vector<int> angleIndexes;
	//	angleIndexes.reserve(angles.size());
	//	for (size_t i = 0; i < angles.size(); ++i)
	//	{
	//		angleIndexes.push_back(angles[i].index);
	//	}
	//	return angleIndexes;
	//}

	std::vector<Ort::AllocatedStringPtr> GetInputNames(Ort::Session& session)
	{
		Ort::AllocatorWithDefaultOptions allocator;
		const size_t numInputNodes = session.GetInputCount();

		std::vector<Ort::AllocatedStringPtr> inputNamesPtr;
		inputNamesPtr.reserve(numInputNodes);
		std::vector<int64_t> input_node_dims;

		// iterate over all input nodes
		for (size_t i = 0; i < numInputNodes; i++)
		{
			auto inputName = session.GetInputNameAllocated(i, allocator);
			inputNamesPtr.push_back(std::move(inputName));
		}
		return inputNamesPtr;
	}

	std::vector<Ort::AllocatedStringPtr> GetOutputNames(Ort::Session& session)
	{
		Ort::AllocatorWithDefaultOptions allocator;
		const size_t numOutputNodes = session.GetOutputCount();

		std::vector<Ort::AllocatedStringPtr> outputNamesPtr;
		outputNamesPtr.reserve(numOutputNodes);
		std::vector<int64_t> output_node_dims;

		for (size_t i = 0; i < numOutputNodes; i++)
		{
			auto outputName = session.GetOutputNameAllocated(i, allocator);
			outputNamesPtr.push_back(std::move(outputName));
		}
		return outputNamesPtr;
	}

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
