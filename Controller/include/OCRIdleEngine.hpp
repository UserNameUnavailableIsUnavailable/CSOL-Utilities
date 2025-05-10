#pragma once

#include <filesystem>
#include "IdleEngine.hpp"
#include "OCR.hpp"
#include "opencv2/core/mat.hpp"
#include <aho_corasick/aho_corasick.hpp>
#include <nlohmann/json.hpp>
#include <stop_token>

namespace CSOL_Utilities
{
    struct OCRBackboneInformation
    {
        std::filesystem::path DBNetPath;
        std::filesystem::path CRNNPath;
        std::filesystem::path DictPath;
        std::filesystem::path KeywordsPath;
    };

    class OCRIdleEngine : public IdleEngine
    {
    public:
        OCRIdleEngine(GameProcessInformation game_process_information, OCRBackboneInformation ocr_backbone_information);
    private:
		static constexpr std::string_view KEYWORD_CATEGORY_LOBBY = "LOBBY";
		static constexpr std::string_view KEYWORD_CATEGORY_ROOM = "ROOM";
		static constexpr std::string_view KEYWORD_CATEGORY_LOADING = "LOADING";
		static constexpr std::string_view KEYWORD_CATEGORY_IN_GAME = "IN_GAME";
		nlohmann::json m_Keywords; /* 关键词 */
		std::atomic_bool m_extended = false; /* 扩展模式是否启用 */
		IN_GAME_STATE m_igs; /* 游戏内状态 */
		std::chrono::system_clock::time_point m_tp; /* 状态解析时刻 */
        OCR m_OCR;
        cv::Mat m_Image;
        std::vector<uint8_t> m_ImageBuffer;
        aho_corasick::trie trie;
		std::function<void (HWND)> m_RemoveWindowBorder;
    private:
        virtual void RecognizeGameState(std::stop_token st) override;
		void update_state(IN_GAME_STATE state, std::chrono::system_clock::time_point tp) noexcept
		{
			m_igs = state;
			m_tp = tp;
		}
		std::string Recognize();
		void Dispatch();
		void Analyze();
    };
}
