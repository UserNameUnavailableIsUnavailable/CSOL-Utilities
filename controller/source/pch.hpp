#pragma once

#include <string>
#include <filesystem>
#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <climits>
#include <condition_variable>
#include <csignal>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <sstream>
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <expected>

#include <winrt/Windows.System.Diagnostics.h>

// #include <onnxruntime_cxx_api.h>
// #include <onnxruntime_run_options_config_keys.h>
// #include <onnxruntime_session_options_config_keys.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
