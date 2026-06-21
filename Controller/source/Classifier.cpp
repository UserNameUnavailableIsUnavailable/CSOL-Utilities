#include "Classifier.hpp"
#include <nlohmann/json.hpp>
#include "ModelUtilities.hpp"
#include "Utilities.hpp"

using namespace CSOL_Utilities;

void Classifier::LoadMetadata(Classifier& classifier, const std::filesystem::path& metadata_path)
{
    std::ifstream ifs(metadata_path);
    if (!ifs.is_open())
    {
        throw std::runtime_error(std::format("Failed to open {}", ConvertUtf16ToUtf8(metadata_path.wstring())));
    }
    nlohmann::json metadata;
    ifs >> metadata;
    // JSON template
    // {
    //     "labels": [
    //         { "0": "foo" },
    //         { "1": "bar" }
    //     ],
    //    "confidence_threshold": 0.95,
    //    "mean": [0.485, 0.456, 0.406],
    //    "std": [0.229, 0.224, 0.225],
    //    "width": 800,
    //    "height": 600
    // }
    classifier.input_width_ = metadata.value<int>("width", classifier.input_width_);
    classifier.input_height_ = metadata.value<int>("height", classifier.input_height_);
    classifier.std_ = metadata.value<std::array<float, 3>>("std", classifier.std_);
    classifier.mean_ = metadata.value<std::array<float, 3>>("mean", classifier.mean_);
    classifier.confidence_threshold_ = metadata.value<float>("confidence_threshold", classifier.confidence_threshold_);
    metadata["labels"].get_to(classifier.labels_);
}

void Classifier::LoadModel(Classifier& classfier, const std::filesystem::path& model_path)
{
    // 加载模型
    if (!std::filesystem::is_regular_file(model_path))
    {
        throw std::runtime_error(std::format("{} does not exist or is not a regular file", ConvertUtf16ToUtf8(model_path.wstring())));
    }
    // 并行推理
    classfier.session_options_.SetExecutionMode(ExecutionMode::ORT_PARALLEL);
    // 禁用空转，降低开销
    classfier.session_options_.AddConfigEntry(kOrtSessionOptionsConfigAllowIntraOpSpinning, "0");
    classfier.session_options_.AddConfigEntry(kOrtSessionOptionsConfigAllowInterOpSpinning, "0");
    // 图优化级别拉满
    classfier.session_options_.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
    classfier.env_ = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_ERROR, model_path.stem().string().c_str());
    classfier.session_ = std::make_unique<Ort::Session>(*classfier.env_, model_path.wstring().c_str(), classfier.session_options_);
    classfier.input_names_ = GetInputNames(*classfier.session_);
    classfier.output_names_ = GetOutputNames(*classfier.session_);
    for (auto &name : classfier.input_names_)
    {
        classfier.input_names_c_str_.emplace_back(name.c_str());
    }
    for (auto &name : classfier.output_names_)
    {
        classfier.output_names_c_str_.emplace_back(name.c_str());
    }
}

Classifier::Classifier(std::filesystem::path json_path)
{
    LoadMetadata(*this, json_path);
    auto model_path = json_path.replace_extension(".onnx");
    LoadModel(*this, model_path);
}

Classifier::Classifier(std::filesystem::path model_dir, std::string model_name)
{
    std::wstring model_name_w = ConvertUtf8ToUtf16(model_name);
    std::filesystem::path metadata_json_path = model_dir / (model_name_w + L".json");
    LoadMetadata(*this, metadata_json_path);
    std::filesystem::path model_path = model_dir / (model_name_w + L".onnx");
    LoadModel(*this, model_path);
}

std::vector<float> Classifier::Preprocess(const cv::Mat &image)
{
    cv::Mat processed_image;
    // 上下左右各填充 50 像素
    auto pad = 50;
    cv::copyMakeBorder(image, processed_image, pad, pad, pad, pad, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    // 调整大小
    cv::resize(processed_image, processed_image, cv::Size(input_width_, input_height_));
    // 移除可能存在的 aplha 通道
    if (processed_image.channels() == 4)
    {
        cv::cvtColor(processed_image, processed_image, cv::COLOR_BGRA2BGR);
    }
#ifdef _DEBUG
    cv::imwrite("preprocessed_image.bmp", processed_image);
#endif
    // reshape to 1D
    processed_image = processed_image.reshape(1, 1);
    std::vector<float> raw_image;
    // ToTensor
    processed_image.convertTo(raw_image, CV_32FC1, 1.0f / 255.0f);
    std::vector<float> raw_tensor(input_channels_ * input_height_ * input_width_);
    // HWC to CHW and Normalize
    for (std::size_t channel = 0; channel < input_channels_; ++channel)
    {
        for (std::size_t i = channel; i < raw_image.size(); i += input_channels_)
        {
            raw_tensor[channel * input_height_ * input_width_ + i / input_channels_] =
                (raw_image[i] - mean_[channel]) / std_[channel];
        }
    }
    return raw_tensor;
}

std::string Classifier::Run(const cv::Mat &image)
{
    std::vector<float> raw_tensor = Preprocess(image);
    assert(!raw_tensor.empty());
    std::array<int64_t, 4> shape{1, input_channels_, input_height_, input_width_};
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    // 重要：创建 tensor 后，tensor 内部保存 raw_tensor 的指针，因此 raw_tensor
    // 变量必须在 tensor 生命周期内有效
    auto input_tensor =
        Ort::Value::CreateTensor<float>(memory_info, raw_tensor.data(), raw_tensor.size(), shape.data(), shape.size());
    assert(input_tensor.IsTensor());
    auto output_tensors =
        session_->Run(Ort::RunOptions(), input_names_c_str_.data(), &input_tensor, input_names_c_str_.size(),
                      output_names_c_str_.data(), output_names_c_str_.size());
    float *output_tensor = output_tensors.front().GetTensorMutableData<float>();
    // softmax 归一化
    std::vector<float> probabilities(labels_.size());
    for (size_t i = 0; i < labels_.size(); ++i)
    {
        probabilities[i] = std::exp(output_tensor[i]);
    }
    float sum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0f);
    for (auto &p : probabilities)
    {
        p /= sum;
    }
    auto max_item = std::max_element(probabilities.begin(), probabilities.end());
    // 置信度检测
    if (*max_item < confidence_threshold_)
    {
        return {}; // 置信度过低，返回空字符串
    }
    return labels_[std::distance(probabilities.begin(), max_item)];
}
