// In a source file that will definitely be linked
#include <gtest/gtest.h>

class MyEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
    #ifdef _DEBUG
        spdlog::set_level(spdlog::level::debug);
    #endif // _DEBUG
    }
};

// Use global variable initialization to trigger registration
// This line will execute before gtest_main's main starts
::testing::Environment* const env = 
    ::testing::AddGlobalTestEnvironment(new MyEnvironment);