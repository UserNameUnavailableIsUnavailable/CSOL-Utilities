#pragma once

namespace CSOL_Utilities
{
	struct OCR_PreprocessInformation
	{
		std::uint16_t m_Padding;
		int m_MaxSideLength;
		bool m_IsWidthScaled;
		bool m_IsHeightScaled;
		int m_OriginalWidth;
		int m_OriginalHeight;
		int m_ScaledWidth;
		int m_ScaledHeight;
		float m_WidthScaleRatio;
		float m_HeightScaleRatio;
	};

	/// 文本检测结果
	struct OCR_DetectionResult
	{
		std::array<cv::Point, 4> m_RoI;
		float m_ROIConfidence;
	};

	/// 文本识别结果
	struct OCR_RecognitionResult
	{
		std::string m_Text;
		std::vector<float> m_CharConfidences; /* 每个字符的置信度 */
	};

	/// 筛选过后的检测结果和识别结果
	struct OCR_Result
	{
		OCR_DetectionResult m_DetectionResult;
		OCR_RecognitionResult m_ReognitionResult;
	};

	template <typename T>
	concept IsValidPointType = requires(T t) {
		{ t.x } -> std::convertible_to<double>;
		{ t.y } -> std::convertible_to<double>;
	};

	struct DBNet_Setting
	{
		std::filesystem::path m_ModelPath;
		float m_DilationOffsetRatio = 1.5f; /* 膨胀补偿系数比例 */
		float m_BinarizationProbabilityThreshold = 0.2f; /* 概率阈值，用于将概率图二值化，进而获取掩码图 */
	};

	struct CRNN_Setting
	{
		std::filesystem::path m_ModelPath;
		std::filesystem::path m_DictionaryPath;
	};

	struct OCR_Setting
	{
		uint16_t m_Padding = 50;
		int m_MaxSideLength = 1024; /* 边长最大值，超过该值进行缩放 */
		float m_DetectionConfidenceThreshold = 0.6f; /* 检测结果置信度阈值，达到此阈值的文本标注框将被接受 */
		float m_RecognitionConfidenceThreshold = 0.6f; /* 识别结果置信度阈值 */
	};

	class DBNet
	{
	public:
		DBNet(std::unique_ptr<DBNet_Setting> setting);
		~DBNet();
		std::vector<OCR_DetectionResult> Run(const cv::Mat& image);
	private:
		template <typename Iterable>
		requires IsValidPointType<typename std::iterator_traits<typename Iterable::const_iterator>::value_type>
		double CalculatePolygonDilationOffset(const Iterable& polygon);
		std::vector<OCR_DetectionResult> MakeDetectionResult(const cv::Mat& possibility_map, const cv::Mat& dilateMat);
		std::unique_ptr<DBNet_Setting> m_Setting;
		std::unique_ptr<Ort::Session> session;
		Ort::Env env = Ort::Env(ORT_LOGGING_LEVEL_ERROR, "DBNet");
		Ort::SessionOptions sessionOptions;
		std::vector<Ort::AllocatedStringPtr> inputNamesPtr;
		std::vector<Ort::AllocatedStringPtr> outputNamesPtr;
		static constexpr const float MEAN[3]{0.485f * 255.0f, 0.456f * 255.0f, 0.406f * 255.0f};
		static constexpr const float NORM[3]{1.0f / 0.229f / 255.0f, 1.0f / 0.248f / 255.0f, 1.0f / 0.225f / 255.0f};
	};

	class CRNN
	{
	public:
		CRNN(std::unique_ptr<CRNN_Setting> setting);
		~CRNN();
		OCR_RecognitionResult Run(const cv::Mat& src);

	private:
		std::unique_ptr<CRNN_Setting> m_Setting;
		bool isOutputDebugImg = false;
		std::unique_ptr<Ort::Session> session;
		Ort::Env env = Ort::Env(ORT_LOGGING_LEVEL_ERROR, "CRNN");
		Ort::SessionOptions sessionOptions;
		int m_nThreads = 0;
		std::vector<Ort::AllocatedStringPtr> inputNamesPtr;
		std::vector<Ort::AllocatedStringPtr> outputNamesPtr;
		const float meanValues[3]{127.5f, 127.5f, 127.5f};
		const float normValues[3]{1.0f / 127.5f, 1.0f / 127.5f, 1.0f / 127.5f};
		const int dstHeight = 48;
		std::vector<std::string> keys;
		OCR_RecognitionResult scoreToTextLine(const std::vector<float>& outputData, size_t h, size_t w);
	};

	// struct Angle
	// {
	// 	int index;
	// 	float score;
	// };

	// class AngleNet
	// {
	// public:
	// 	AngleNet(std::filesystem::path p, int nThreads);
	// 	~AngleNet();
	// 	std::vector<Angle> GetAngles(std::vector<cv::Mat>& partImgs, bool doAngle, bool mostAngle);

	// private:
	// 	int m_number_of_threads;
	// 	bool isOutputAngleImg = false;
	// 	std::unique_ptr<Ort::Session> session;
	// 	Ort::Env env = Ort::Env(ORT_LOGGING_LEVEL_ERROR, "AngleNet");
	// 	Ort::SessionOptions sessionOptions;
	// 	std::vector<Ort::AllocatedStringPtr> inputNamesPtr;
	// 	std::vector<Ort::AllocatedStringPtr> outputNamesPtr;
	// 	const float meanValues[3]{127.5f, 127.5f, 127.5f};
	// 	const float normValues[3]{1.0f / 127.5f, 1.0f / 127.5f, 1.0f / 127.5f};
	// 	const int dstWidth = 192;
	// 	const int dstHeight = 48;
	// 	Angle getAngle(cv::Mat& src);
	// };

	class OCR
	{
	public:
		OCR(
			std::unique_ptr<OCR_Setting> ocr_setting,
			std::unique_ptr<DBNet_Setting> dbnet_setting, std::unique_ptr<CRNN_Setting> crnn_Setting
		);
		~OCR() noexcept = default;
		std::vector<OCR_Result> Detect(const cv::Mat& mat);
	private:
		static std::vector<cv::Mat> GetImagePatches(cv::Mat& src, std::vector<OCR_DetectionResult>& textBoxes);
		static cv::Mat PreprocessImage(const cv::Mat& image, OCR_PreprocessInformation& preprocess_information);
		// std::vector<OCR_DetectionResult> FilterDetectionResult(const cv::Mat& possibility_map, const cv::Mat& dilateMat);
		std::unique_ptr<OCR_Setting> m_Setting;
		DBNet m_DBNet;
		CRNN m_CRNN;
	};


	template <std::size_t N>
	std::vector<float> SubstractMeanNormalize(const cv::Mat& image, const float (&mean)[N], const float (&norm)[N])
	{
		auto tensor_size = image.cols * image.rows * image.channels();
		std::vector<float> tensor(tensor_size);
		auto num_channels = image.channels();
		assert(N == num_channels);
		auto image_size = image.cols * image.rows;

		for (size_t pixel_index = 0; pixel_index < image_size; pixel_index++)
		{
			for (size_t channel_index = 0; channel_index < N; ++channel_index)
			{
				float data = image.data[pixel_index * num_channels + channel_index] * norm[channel_index] - mean[channel_index] * norm[channel_index];
				tensor[channel_index * image_size + pixel_index] = data;
			}
		}
		return tensor;
	}

	// std::vector<int> GetAngleIndexes(std::vector<Angle>& angles);
	cv::Mat AdjustTargetImg(cv::Mat& src, int dstWidth, int dstHeight);
	std::vector<Ort::AllocatedStringPtr> GetInputNames(Ort::Session& session);
	std::vector<Ort::AllocatedStringPtr> GetOutputNames(Ort::Session& session);
	float EvaluateBox(const std::array<cv::Point2f, 4>& boxes, const cv::Mat& pred);
	inline int GetThickness(const cv::Mat& box_image)
	{
		int min_side_length = std::min(box_image.cols, box_image.rows);
		int thickness = min_side_length / 1000 + 2;
		return thickness;
	}

	void DrawTextBox(cv::Mat& boxImg, cv::RotatedRect& rect, int thickness);

	void DrawTextBox(cv::Mat& boxImg, const std::array<cv::Point, 4>& box);
	cv::Mat GetRotateCropImage(const cv::Mat& src, std::array<cv::Point, 4> box);
	OCR_PreprocessInformation GetScaleParam(cv::Mat& src, const float scale);
	OCR_PreprocessInformation GetScaleParam(cv::Mat& src, const int targetSize);
	cv::Mat RotateClockWise90(cv::Mat src);
	cv::Mat RotateClockWise180(cv::Mat src);
	std::array<cv::Point2f, 4> GetMinBoxes(const cv::RotatedRect& boxRect, float& maxSideLen);
	
	/// 计算多边形的扩张补偿系数，控制标注框大小。
	/// @param polygon 多边形点列
	/// @return 松弛度
	/// @remark 详见 https://arxiv.org/pdf/1911.08947。
	template <typename Iterable>
	requires IsValidPointType<typename std::iterator_traits<typename Iterable::const_iterator>::value_type>
	double DBNet::CalculatePolygonDilationOffset(const Iterable& polygon)
	{
		auto begin = std::begin(polygon);
		auto end = std::end(polygon);
		auto size = std::distance(begin, end);

		if (size < 3)
			return 0.0;

		double area = 0.0; /* 多边形面积 */
		double perimeter = 0.0; /* 多边形周长 */

		for (size_t i = 0; i < size; i++)
		{
			auto current = std::next(begin, i);
			auto next = std::next(begin, (i + 1) % size);

			area += current->x * next->y - current->y * next->x; /* 面积计算：鞋带公式（Shoelace Formula） */
			perimeter += std::sqrt(std::pow(current->x - next->x, 2) + std::pow(current->y - next->y, 2));
		}
		area = std::abs(area) / 2.0;
		return m_Setting->m_DilationOffsetRatio * area / perimeter;
	}


	/// 获取能够容纳指定多边形的最小矩形（使用 Clipper2）
	/// @param polygon 多边形（元素为点的可迭代容器）
	template <typename Iterable>
	requires IsValidPointType<typename std::iterator_traits<typename Iterable::const_iterator>::value_type>
	cv::RotatedRect ComputePolygonBoundingBox(const Iterable& polygon, double delta)
	{
		using namespace Clipper2Lib;

		PathD path;

		for (const auto& pt : polygon)
		{
			path.emplace_back(pt.x, pt.y);
		}

		// Convert PathD to Path64
		Path64 path64;
		for (const auto& pt : path)
		{
			path64.emplace_back(static_cast<int64_t>(std::round(pt.x)), static_cast<int64_t>(std::round(pt.y)));
		}

		ClipperOffset clipper_offset;
		clipper_offset.AddPath(path64, JoinType::Round, EndType::Polygon);

		Paths64 solution;
		clipper_offset.Execute(static_cast<int64_t>(std::round(delta)), solution);

		std::vector<cv::Point2f> points;
		for (const auto& sol_path : solution)
		{
			for (const auto& pt : sol_path)
			{
				points.emplace_back(static_cast<float>(pt.x), static_cast<float>(pt.y));
			}
		}

		if (points.empty())
		{
			return cv::RotatedRect(cv::Point2f(0, 0), cv::Size2f(1, 1), 0);
		}
		else
		{
			return cv::minAreaRect(points);
		}
	}

	/// 计算 RoI 包含的像素点的均值。
	/// @param mat 图像
	/// @param roi 包含像素点的多边形区域边界
	cv::Scalar ComputeMeanInROI(const cv::Mat& mat, const std::vector<std::vector<cv::Point>>& roi);

} // namespace CSOL_Utilities
