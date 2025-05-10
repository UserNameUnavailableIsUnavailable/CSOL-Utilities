#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "OCR.hpp"

namespace CSOL_Utilities
{
	OCR::OCR(std::filesystem::path det_model_path,
			 // std::filesystem::path cls_model_path,
			 std::filesystem::path rec_model_path, std::filesystem::path dict_file_path, int nThreads) :
		m_nThreads(nThreads), m_DBNet(det_model_path, nThreads),
		// angleNet(cls_model_path, nThreads),
		m_CRNN(rec_model_path, dict_file_path, nThreads)
	{
	}

	OCRResult OCR::Detect(cv::Mat& img, OCRParam& param)
	{
		// cv::Mat rgb;
		// cvtColor(img, rgb, cv::COLOR_BGR2RGB); // 转为 RGB
		int originMaxSide = std::max(img.cols, img.rows);
		int resize;
		if (param.nMaxSideLength <= 0 || param.nMaxSideLength > originMaxSide)
		{
			resize = originMaxSide;
		}
		else
		{
			resize = param.nMaxSideLength;
		}
		resize += 2 * param.nPadding;
		cv::Rect padded_rect(param.nPadding, param.nPadding, img.cols, img.rows);
		cv::Mat padded_img = PadImage(img, param.nPadding);
		ScaleParam scale = GetScaleParam(padded_img, resize);
		OCRResult result;
		result =
			Detect(padded_img, padded_rect, scale, param.fBoxScoreThreshold, param.fBoxThreshold, param.fUnclipRatio);
		return result;
	}

	OCRResult OCR::Detect(cv::Mat& src, cv::Rect& originRect, ScaleParam& scale, float boxScoreThresh, float boxThresh,
						  float unClipRatio)
	{
		cv::Mat annotated_img = src.clone();
		int thickness = GetThickness(src);

		std::vector<TextBox> textBoxes = m_DBNet.getTextBoxes(src, scale, boxScoreThresh, boxThresh, unClipRatio);

		DrawTextBoxes(annotated_img, textBoxes, thickness);

		std::vector<cv::Mat> patches = GetImagePatches(src, textBoxes);

		// std::vector<Angle> angles;
		// angles = angleNet.GetAngles(patches, doAngle, mostAngle);

		// for (size_t i = 0; i < patches.size(); ++i)
		//{
		//     if (angles[i].index == 0)
		//     {
		//         patches.at(i) = matRotateClockWise180(patches[i]);
		//     }
		// }

		std::vector<TextLine> textLines = m_CRNN.GetTextLines(patches);

		std::vector<TextBlock> textBlocks;
		for (size_t i = 0; i < textLines.size(); ++i)
		{
			std::vector<cv::Point> boxPoint = std::vector<cv::Point>(4);
			int padding = originRect.x;
			boxPoint[0] = cv::Point(textBoxes[i].boxPoint[0].x - padding, textBoxes[i].boxPoint[0].y - padding);
			boxPoint[1] = cv::Point(textBoxes[i].boxPoint[1].x - padding, textBoxes[i].boxPoint[1].y - padding);
			boxPoint[2] = cv::Point(textBoxes[i].boxPoint[2].x - padding, textBoxes[i].boxPoint[2].y - padding);
			boxPoint[3] = cv::Point(textBoxes[i].boxPoint[3].x - padding, textBoxes[i].boxPoint[3].y - padding);
			TextBlock textBlock{boxPoint, textBoxes[i].score, /*angles[i].index, angles[i].score, */ textLines[i].text,
								textLines[i].charScores};
			textBlocks.emplace_back(textBlock);
		}

		cv::Mat rgbBoxImg, textBoxImg;

		if (originRect.x > 0 && originRect.y > 0)
		{
			annotated_img(originRect).copyTo(rgbBoxImg);
		}
		else
		{
			rgbBoxImg = annotated_img;
		}
		cv::cvtColor(rgbBoxImg, textBoxImg, cv::COLOR_RGB2BGR);

		std::string strRes;
		for (size_t i = 0; i < textBlocks.size(); ++i)
		{
			strRes.append(textBlocks[i].text);
			strRes.append("\n");
		}
#ifdef _DEBUG
		cv::imwrite("annotation.bmp", annotated_img);
#endif
		return OCRResult{std::move(textBlocks), std::move(textBoxImg), strRes};
	}
} // namespace CSOL_Utilities
