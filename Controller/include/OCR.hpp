#pragma once

namespace CSOL_Utilities
{
	struct OCR_PreprocessInformation
	{
		unsigned short padding;
		unsigned int max_side_length;
		bool is_width_scaled;
		bool is_height_scaled;
		int original_width;
		int original_height;
		int scaled_width;
		int scaled_height;
		float width_scale_ratio;
		float height_scale_ratio;
	};

	/// 筛选过后的检测结果和识别结果
	struct OCR_Result
	{
		std::array<cv::Point, 4> box;
		std::string text;
	};

	template <typename T>
	concept IsValidPointType = requires(T t) {
		{ t.x } -> std::convertible_to<double>;
		{ t.y } -> std::convertible_to<double>;
	};

	class OCR_Detector
	{
	public:
		OCR_Detector(std::filesystem::path json_path);
		OCR_Detector(const OCR_Detector&) = delete;
		OCR_Detector(OCR_Detector&&) = default;
		~OCR_Detector() = default;
		std::vector<std::array<cv::Point, 4>> Run(const cv::Mat& image);
	private:
		template <typename Iterable>
		requires IsValidPointType<typename std::iterator_traits<typename Iterable::const_iterator>::value_type>
		double CalculatePolygonDilationOffset(const Iterable& polygon);
		std::vector<std::array<cv::Point, 4>> MakeDetectionResult(const cv::Mat& possibility_map, const cv::Mat& dilateMat);
		std::unique_ptr<Ort::Session> session;
		std::unique_ptr<Ort::Env> env_;
		Ort::SessionOptions sessionOptions;
		std::vector<Ort::AllocatedStringPtr> input_names_;
		std::vector<Ort::AllocatedStringPtr> output_names_;

		std::filesystem::path model_path_;
		float dilation_offset_ratio_ = 1.5f;
		float binarization_probability_threshold_ = 0.2f;
		float confidence_threshold_ = 0.6f;
		std::array<float, 3> mean_ = {0.485f * 255.0f, 0.456f * 255.0f, 0.406f * 255.0f};
		std::array<float, 3> norm_ = {1.0f / 0.229f / 255.0f, 1.0f / 0.248f / 255.0f, 1.0f / 0.225f / 255.0f};
	};

	class OCR_Recognizer
	{
	public:
		OCR_Recognizer(std::filesystem::path json_path);
		OCR_Recognizer(const OCR_Recognizer&) = delete;
		OCR_Recognizer(OCR_Recognizer&&) = default;
		~OCR_Recognizer();
		std::string Run(const cv::Mat& src);

	private:
		std::filesystem::path model_path_;
		std::filesystem::path dictionary_path_;
		bool isOutputDebugImg = false;
		std::unique_ptr<Ort::Session> session;
		std::unique_ptr<Ort::Env> env_;
		Ort::SessionOptions session_opts_;
		std::vector<Ort::AllocatedStringPtr> input_names_;
		std::vector<Ort::AllocatedStringPtr> output_names_;
		const int dstHeight = 48;
		std::string ScoreToText(const std::vector<float>& score, size_t h, size_t w);
		std::vector<std::string> dictionary_; // 语言字典
		std::array<float, 3> mean_ = {127.5f, 127.5f, 127.5f};
		std::array<float, 3> norm_ = {1.0f / 127.5f, 1.0f / 127.5f, 1.0f / 127.5f};
		float confidence_threshold_ = 0.6f;
	};

	class OCR
	{
	public:
		OCR(
			OCR_Detector&& detector,
			OCR_Recognizer&& recognizer
		);
		~OCR() noexcept = default;
		std::vector<OCR_Result> Detect(const cv::Mat& mat);
	private:
		static cv::Mat PreprocessImage(const cv::Mat& image, OCR_PreprocessInformation& preprocess_information);
		OCR_Detector detector;
		OCR_Recognizer recognizer;
		const unsigned short padding_ = 50;
		const unsigned int max_side_length_ = 1024;
	};


	template <std::size_t N>
	std::vector<float> SubstractMeanNormalize(const cv::Mat& image, const std::array<float, N>& mean, const std::array<float, N>& norm)
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

	cv::Mat AdjustTargetImg(cv::Mat& src, int dstWidth, int dstHeight);
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
	double OCR_Detector::CalculatePolygonDilationOffset(const Iterable& polygon)
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
		return dilation_offset_ratio_ * area / perimeter;
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
