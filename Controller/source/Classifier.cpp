#include "Classifier.hpp"
#include "Utilities.hpp"
#include "Exception.hpp"
#include "ModelUtilities.hpp"
#include <nlohmann/json.hpp>

using namespace CSOL_Utilities;

Classifier::Classifier(std::filesystem::path json_path)
{
    if (!std::filesystem::is_regular_file(json_path))
    {
        throw Exception(Translate("ResNet::ERROR_FileNotFound@1", ConvertUtf16ToUtf8(json_path)));
    }
    std::ifstream json_fstream(json_path);
    if (!json_fstream)
    {
        throw Exception(Translate("ResNet::ERROR_FailedToReadLabelFile@1", ConvertUtf16ToUtf8(json_path)));
    }
    nlohmann::json metadata_json;
    json_fstream >> metadata_json;
    // JSON 样例
    // {
    //     "model_path": "resnet18.onnx",
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
    if (!metadata_json.contains("model_path"))
    {
        throw Exception(Translate("ERROR_MandatoryFieldMissing@1", "model_path"));
    }
    model_path_ = metadata_json["model_path"].get<std::string>();
    if (model_path_.is_relative()) // 若是相对路径，则相对于 JSON 文件所在目录
    {
        model_path_ = json_path.parent_path() / model_path_;
    }
    if (metadata_json.contains("confidence_threshold"))
    {
        confidence_threshold_ = metadata_json["confidence_threshold"].get<float>();
    }
    if (!metadata_json.contains("labels")) // labels 为必填项
    {
        throw Exception(Translate("ERROR_MandatoryFieldMissing@1", "labels"));
    }
    for (auto& item : metadata_json["labels"])
    {
        for (auto& [key, value] : item.items())
        {
            int index = std::stoi(key);
            labels_.emplace(index, value.get<std::string>());
        }
    }
    if (metadata_json.contains("mean"))
    {
        auto mean_array = metadata_json["mean"].get<std::vector<float>>();
        for (size_t i = 0; i < 3 && i < mean_array.size(); ++i)
        {
            mean_[i] = mean_array[i];
        }
    }
    if (metadata_json.contains("std"))
    {
        auto std_array = metadata_json["std"].get<std::vector<float>>();
        for (size_t i = 0; i < 3 && i < std_array.size(); ++i)
        {
            std_[i] = std_array[i];
        }
    }
    if (metadata_json.contains("width"))
    {
        input_width_ = metadata_json["width"].get<int>();
    }
    if (metadata_json.contains("height"))
    {
        input_height_ = metadata_json["height"].get<int>();
    }
    // 加载模型
    if (!std::filesystem::is_regular_file(model_path_))
    {
        throw Exception(Translate("ERROR_FileNotFound@1", ConvertUtf16ToUtf8(model_path_)));
    }
    // 并行推理
    session_options_.SetExecutionMode(ExecutionMode::ORT_PARALLEL);
    // 禁用空转，降低开销
    session_options_.AddConfigEntry(kOrtSessionOptionsConfigAllowIntraOpSpinning, "0");
    session_options_.AddConfigEntry(kOrtSessionOptionsConfigAllowInterOpSpinning, "0");
    // 图优化级别拉满
    session_options_.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
    env_ = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_ERROR, model_path_.stem().string().c_str());
    session_ = std::make_unique<Ort::Session>(*env_, model_path_.wstring().c_str(), session_options_);
    auto input_names = session_->GetInputNames();
    auto output_names = session_->GetOutputNames();
    std::swap(input_names_, input_names);
    std::swap(output_names_, output_names);
    for (auto& name : input_names_)
    {
        input_names_c_str_.emplace_back(name.c_str());
    }
    for (auto& name : output_names_)
    {
        output_names_c_str_.emplace_back(name.c_str());
    }
}

std::vector<float> Classifier::Preprocess(const cv::Mat& image)
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

std::string Classifier::Run(const cv::Mat& image)
{
    std::vector<float> raw_tensor = Preprocess(image);
    assert(!raw_tensor.empty());
    std::array<int64_t, 4> shape{1, input_channels_, input_height_, input_width_};
    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    // 重要：创建 tensor 后，tensor 内部保存 raw_tensor 的指针，因此 raw_tensor 变量必须在 tensor 生命周期内有效
    auto input_tensor = Ort::Value::CreateTensor<float>(memory_info, raw_tensor.data(), raw_tensor.size(), shape.data(), shape.size());
    assert(input_tensor.IsTensor());
    auto output_tensors = session_->Run(
        Ort::RunOptions(),
        input_names_c_str_.data(),
        &input_tensor,
        input_names_c_str_.size(),
        output_names_c_str_.data(),
        output_names_c_str_.size()
    );
    float* output_tensor = output_tensors.front().GetTensorMutableData<float>();
    // softmax 归一化
    std::vector<float> probabilities(labels_.size());
    for (size_t i = 0; i < labels_.size(); ++i)
    {
        probabilities[i] = std::exp(output_tensor[i]);
    }
    float sum = std::accumulate(probabilities.begin(), probabilities.end(), 0.0f);
    for (auto& p : probabilities)
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
