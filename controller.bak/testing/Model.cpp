#include <gtest/gtest.h>

#include "TestUtilities.hpp"

#include <Classifier.hpp>

using namespace CSOL_Utilities;

TEST(ModelTest, CPUExecutionProvider)
{
    const std::vector<std::string> expected_providers{"CPUExecutionProvider"};
    EXPECT_EQ(Ort::GetAvailableProviders(), expected_providers);
}

TEST(ModelTest, LoadTest)
{
    auto model_dir = GetSourceDirectory() / "Models" / "artifacts";
    ASSERT_TRUE(std::filesystem::is_regular_file(model_dir / "ResNet18-800x600.onnx"));
    std::ifstream metadata_file(model_dir / "ResNet18-800x600.json");
    ASSERT_TRUE(metadata_file.is_open());
    const auto metadata = nlohmann::json::parse(metadata_file);
    const auto labels = metadata.at("labels").get<std::vector<std::string>>();
    ASSERT_FALSE(labels.empty());

    Classifier classifier(model_dir, "ResNet18-800x600");
    const cv::Mat image(600, 800, CV_8UC3, cv::Scalar(0, 0, 0));
    const auto result = classifier.Run(image);
    EXPECT_TRUE(result.empty() || std::find(labels.begin(), labels.end(), result) != labels.end());

    Classifier classifier_from_json(model_dir / "ResNet18-800x600.json");
    EXPECT_EQ(classifier_from_json.Run(image), result);
}
