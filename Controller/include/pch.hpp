#pragma once

// pre-compile header
// This file is included beforehand in other source files (see CMakeLists.txt),
// so you needn't include this file explicitly.

#include <Windows.h>
#include <tlhelp32.h>
#undef min
#undef max

#include <clipper2/clipper.core.h>
#include <clipper2/clipper.offset.h>
#include <onnxruntime/onnxruntime_cxx_api.h>
#include <onnxruntime/onnxruntime_run_options_config_keys.h>
#include <onnxruntime/onnxruntime_session_options_config_keys.h>

#include <aho_corasick/aho_corasick.hpp>
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
#include <nlohmann/json.hpp>
#include <numeric>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>
#include <stop_token>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
