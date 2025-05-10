#pragma once

#include <Windows.h>
#include <aho_corasick/aho_corasick.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <opencv2/core.hpp>
#include "Command.hpp"
#include "Module.hpp"
#include "OCR.hpp"


namespace CSOL_Utilities
{
	class OCRIdleEngine : public Module
	{
	public:
		OCRIdleEngine(std::string json_keywords, std::filesystem::path ocr_detection_model_path,
					  std::filesystem::path ocr_recognition_model_path, std::filesystem::path ocr_key_file_path,
					  int n_threads, std::shared_ptr<ExecutorCommand> cmd);
		~OCRIdleEngine();
		virtual void work() override;
		void toggle_extended_mode(bool flag)
		{
			m_extended.store(flag, std::memory_order::release);
		}

	protected:
		static constexpr std::string KEYWORD_CATEGORY_LOBBY = "LOBBY";
		static constexpr std::string KEYWORD_CATEGORY_ROOM = "ROOM";
		static constexpr std::string KEYWORD_CATEGORY_LOADING = "LOADING";
		static constexpr std::string KEYWORD_CATEGORY_IN_GAME = "IN_GAME";
		nlohmann::json m_Keywords; /* 关键词 */
		std::atomic_bool m_extended = false; /* 扩展模式是否启用 */
		HMODULE m_hGamingTool = nullptr; /* 游戏工具 */
		HWND m_hGameWindow = nullptr; /* 游戏窗口 */
		std::shared_ptr<ExecutorCommand> m_cmd; /* 命令 */
		IN_GAME_STATE m_igs; /* 游戏内状态 */
		std::chrono::system_clock::time_point m_tp; /* 状态解析时刻 */
		std::vector<uint8_t> m_image_buffer; /* 保存游戏截图 */
		cv::Mat m_image; /* 游戏截图 */
		OCRParam m_ocr_param; /* OCR 参数 */
		OCR m_ocr; /* OCR 引擎 */
		aho_corasick::trie trie; /* 关键词匹配引擎 */
		std::function<void (HWND)> m_RemoveWindowBorder;
	private:
		void update_state(IN_GAME_STATE state, std::chrono::system_clock::time_point tp) noexcept
		{
			m_igs = state;
			m_tp = tp;
		}
		std::string recognize();
		void Dispatch();
		void Analyze();
	};
} // namespace CSOL_Utilities
