#pragma once

// pre-compile header
// This file is included beforehand in other source files (see CMakeLists.txt), so you needn't include this file explicitly.

#include <Windows.h>
#include <tlhelp32.h>
#undef min
#undef max

#include <format>
#include <iostream>
#include <csignal>
#include <condition_variable>
#include <stop_token>
#include <mutex>
#include <memory>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <climits>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <numeric>
#include <filesystem>
#include <unordered_map>
#include <string_view>
#include <type_traits>
#include <functional>
#include <cassert>
#include <algorithm>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <onnxruntime/onnxruntime_cxx_api.h>
#include <onnxruntime/onnxruntime_session_options_config_keys.h>
#include <onnxruntime/onnxruntime_run_options_config_keys.h>
#include <clipper2/clipper.core.h>
#include <clipper2/clipper.offset.h>
#include <nlohmann/json.hpp>
#include <aho_corasick/aho_corasick.hpp>
