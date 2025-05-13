#pragma once

#include "IdleEngine.hpp"
#include "OCR.hpp"

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
		std::unordered_map<std::string, std::vector<std::string>> m_Keywords; /* 关键词 */
		IN_GAME_STATE m_igs; /* 游戏内状态 */
		std::chrono::system_clock::time_point m_tp; /* 状态解析时刻 */
        std::unique_ptr<OCR> m_OCR;
        cv::Mat m_Image;
        std::vector<uint8_t> m_ImageBuffer;
        aho_corasick::trie trie;
		std::function<void (HWND)> m_RemoveWindowBorder;
    private:
        virtual void RecognizeGameState(std::stop_token st) override;
		void update_state(IN_GAME_STATE state) noexcept
		{
            if (m_igs == state) return; /* 前后两个状态相同，不发生更新，这样保持此状态开始时刻不变 */
			m_igs = state;
			m_tp = std::chrono::system_clock::now();
		}
		void Recognize(std::vector<std::string>&);
		void Dispatch();
		void Analyze();
    };
}
