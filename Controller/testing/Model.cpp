#include <gtest/gtest.h>

#include "TestUtilities.hpp"

#include <Classifier.hpp>

using namespace CSOL_Utilities;

TEST(ModelTest, LoadTest)
{
    auto model_dir = GetSourceDirectory() / "Models" / "artifacts";
    Classifier classifier(model_dir, "ResNet18-800x600");
    auto mat = cv::imread(R"(C:\Users\Silver\BaiduSyncdisk\Pictures\Screen Shots\2026\2026-01-01_00-24-05_860.png)");
    auto result = classifier.Run(mat);
    std::cout << "result: " << result << std::endl;
}
