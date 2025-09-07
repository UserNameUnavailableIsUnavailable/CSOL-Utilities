namespace CSOL_Utilities
{
    class Classifier
    {
    public:
        Classifier(std::filesystem::path json_path);
        ~Classifier() = default;
        // 运行图像分类
        /// @param image 输入图像
        /// @return 类别名称。
        /// @note 若置信度低于阈值，返回空字符串。
        std::string Run(const cv::Mat& image);
    private:
        std::vector<float> Preprocess(const cv::Mat& image);
        std::unordered_map<int, std::string> labels_; // <标签索引，标签名称>
        std::unique_ptr<Ort::Session> session_;
        std::unique_ptr<Ort::Env> env_;
        std::vector<std::string> input_names_;
        std::vector<std::string> output_names_;
        std::vector<const char*> input_names_c_str_;
        std::vector<const char*> output_names_c_str_;
        Ort::SessionOptions session_options_;
        std::array<float, 3> mean_ = {0.485f, 0.456f, 0.406f}; // ResNet-50 标准化均值
        std::array<float, 3> std_ = {0.229f, 0.224f, 0.225f};  // ResNet-50 标准化标准差
        std::filesystem::path model_path_;
        float confidence_threshold_ = 0.95f; // 置信度阈值，softmax 归一化概率超过此值才接受
        int input_width_ = 800;  // 输入图像宽度
        int input_height_ = 600; // 输入图像高度
        int input_channels_ = 3; // 输入图像通道数
    };
}