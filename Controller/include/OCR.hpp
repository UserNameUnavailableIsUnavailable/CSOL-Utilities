#pragma once

#include <onnxruntime/onnxruntime_cxx_api.h>
#include <filesystem>
#include "opencv2/core/mat.hpp"
#include "opencv2/core/types.hpp"

namespace CSOL_Utilities
{
	struct ScaleParam
	{
		int srcWidth;
		int srcHeight;
		int dstWidth;
		int dstHeight;
		float ratioWidth;
		float ratioHeight;
	};

	struct TextBlock
	{
		std::vector<cv::Point> boxPoint;
		float boxScore;
		// int angleIndex;
		// float angleScore;
		std::string text;
		std::vector<float> charScores;
	};

	struct OCRResult
	{
		std::vector<TextBlock> textBlocks;
		cv::Mat boxImg;
		std::string content;
	};

	struct TextBox
	{
		std::vector<cv::Point> boxPoint;
		float score;
	};

	struct Angle
	{
		int index;
		float score;
	};

	struct TextLine
	{
		std::string text;
		std::vector<float> charScores;
		// double time;
	};

	class DBNet
	{
	public:
		DBNet(std::filesystem::path p, int nThreads);
		~DBNet();
		std::vector<TextBox> getTextBoxes(cv::Mat& src, ScaleParam& s, float boxScoreThresh, float boxThresh,
										  float unClipRatio);

	private:
		std::unique_ptr<Ort::Session> session;
		Ort::Env env = Ort::Env(ORT_LOGGING_LEVEL_ERROR, "DbNet");
		Ort::SessionOptions sessionOptions;
		int numThread = 0;
		std::vector<Ort::AllocatedStringPtr> inputNamesPtr;
		std::vector<Ort::AllocatedStringPtr> outputNamesPtr;
		const float meanValues[3] = {0.485 * 255, 0.456 * 255, 0.406 * 255};
		const float normValues[3] = {1.0 / 0.229 / 255.0, 1.0 / 0.248 / 255.0, 1.0 / 0.225 / 255.0};
	};

	class CRNN
	{
	public:
		CRNN(std::filesystem::path recognition_model_path, std::filesystem::path dict_file_path, int nThreads);
		~CRNN();
		std::vector<TextLine> GetTextLines(std::vector<cv::Mat>& partImg);

	private:
		bool isOutputDebugImg = false;
		std::unique_ptr<Ort::Session> session;
		Ort::Env env = Ort::Env(ORT_LOGGING_LEVEL_ERROR, "CrnnNet");
		Ort::SessionOptions sessionOptions;
		int m_nThreads = 0;
		std::vector<Ort::AllocatedStringPtr> inputNamesPtr;
		std::vector<Ort::AllocatedStringPtr> outputNamesPtr;
		const float meanValues[3] = {127.5, 127.5, 127.5};
		const float normValues[3] = {1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5};
		const int dstHeight = 48;
		std::vector<std::string> keys;
		TextLine scoreToTextLine(const std::vector<float>& outputData, size_t h, size_t w);
		TextLine GetTextLine(const cv::Mat& src);
	};

	class AngleNet
	{
	public:
		AngleNet(std::filesystem::path p, int nThreads);
		~AngleNet();
		std::vector<Angle> GetAngles(std::vector<cv::Mat>& partImgs, bool doAngle, bool mostAngle);

	private:
		int m_number_of_threads;
		bool isOutputAngleImg = false;
		std::unique_ptr<Ort::Session> session;
		Ort::Env env = Ort::Env(ORT_LOGGING_LEVEL_ERROR, "AngleNet");
		Ort::SessionOptions sessionOptions;
		std::vector<Ort::AllocatedStringPtr> inputNamesPtr;
		std::vector<Ort::AllocatedStringPtr> outputNamesPtr;
		const float meanValues[3] = {127.5, 127.5, 127.5};
		const float normValues[3] = {1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5};
		const int dstWidth = 192;
		const int dstHeight = 48;
		Angle getAngle(cv::Mat& src);
	};

	struct OCRParam
	{
		int nPadding = 50;
		int nMaxSideLength = 2048;
		float fBoxScoreThreshold = 0.6f;
		float fBoxThreshold = 0.3f;
		float fUnclipRatio = 2.0f;
		// bool bAdjustAngle;
		// bool bMostLikelyAngle;
	};

	class OCR
	{
	public:
		OCR(std::filesystem::path det_model_path, std::filesystem::path rec_model_path, std::filesystem::path dict_file_path, int nThreads);
		~OCR() noexcept = default;
		OCRResult Detect(cv::Mat& mat, OCRParam& param);

	private:
		bool isOutputConsole = false;
		bool isOutputPartImg = false;
		bool isOutputResultTxt = false;
		bool isOutputResultImg = false;
		DBNet m_DBNet;
	// 鉴于检测的是游戏窗口，很少存在文字歪斜情况，故不需要 AngleNet 进行角度校正，去除角度校正模块后还可进一步减少推理开销。
		// AngleNet angleNet;
		CRNN m_CRNN;
		int m_nThreads;
		OCRResult Detect(cv::Mat& src, cv::Rect& originRect, ScaleParam& scale, float boxScoreThresh = 0.6f,
						 float boxThresh = 0.3f, float unClipRatio = 2.0f);
	};

	std::vector<cv::Mat> GetImagePatches(cv::Mat& src, std::vector<TextBox>& textBoxes);
	std::vector<float> SubstractMeanNormalize(cv::Mat& src, const float* meanVals, const float* normVals);
	std::vector<int> GetAngleIndexes(std::vector<Angle>& angles);
	cv::Mat AdjustTargetImg(cv::Mat& src, int dstWidth, int dstHeight);
	std::vector<Ort::AllocatedStringPtr> GetInputNames(Ort::Session& session);
	std::vector<Ort::AllocatedStringPtr> GetOutputNames(Ort::Session& session);
	float BoxScoreFast(const std::vector<cv::Point2f>& boxes, const cv::Mat& pred);
	int GetThickness(cv::Mat& boxImg);

	void DrawTextBox(cv::Mat& boxImg, cv::RotatedRect& rect, int thickness);

	void DrawTextBox(cv::Mat& boxImg, const std::vector<cv::Point>& box, int thickness);
	cv::Mat PadImage(cv::Mat& img, int padding);
	void DrawTextBoxes(cv::Mat& boxImg, std::vector<TextBox>& textBoxes, int thickness);
	cv::Mat GetRotateCropImage(const cv::Mat& src, std::vector<cv::Point> box);
	ScaleParam GetScaleParam(cv::Mat& src, const float scale);
	ScaleParam GetScaleParam(cv::Mat& src, const int targetSize);
	cv::Mat RotateClockWise90(cv::Mat src);
	cv::Mat RotateClockWise180(cv::Mat src);
	std::vector<cv::Point2f> GetMinBoxes(const cv::RotatedRect& boxRect, float& maxSideLen);
	cv::RotatedRect Unclip(std::vector<cv::Point2f> box, float unClipRatio);
} // namespace CSOL_Utilities
