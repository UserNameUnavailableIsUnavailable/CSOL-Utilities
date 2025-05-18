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
        virtual void ResetStateAfterSwitchMode() noexcept override { m_InGameState.store(IN_GAME_STATE::IGS_UNKNOWN, std::memory_order_release); }
        virtual void ResetStateAfterReconection() noexcept override { m_InGameState.store(IN_GAME_STATE::IGS_LOGIN, std::memory_order_release); }
    private:
		static constexpr std::string_view KEYWORD_CATEGORY_LOBBY = "LOBBY";
		static constexpr std::string_view KEYWORD_CATEGORY_ROOM = "ROOM";
		static constexpr std::string_view KEYWORD_CATEGORY_LOADING = "LOADING";
		static constexpr std::string_view KEYWORD_CATEGORY_IN_GAME = "IN_GAME";
		std::unordered_map<std::string, std::vector<std::string>> m_Keywords; /* 关键词 */
		std::atomic<IN_GAME_STATE> m_InGameState = IN_GAME_STATE::IGS_UNKNOWN; /* 游戏内状态，状态机首次启动时 */
		std::chrono::system_clock::time_point m_tp; /* 状态解析时刻 */
        std::unique_ptr<OCR> m_OCR;
        cv::Mat m_Image;
        std::vector<uint8_t> m_ImageBuffer;
        aho_corasick::trie trie;
		std::function<void (HWND)> m_RemoveWindowBorder;
    private:
        virtual void RecognizeGameState(std::stop_token st) override;
		void SetInGameState(IN_GAME_STATE state) noexcept
		{
            if (m_InGameState.load(std::memory_order_acquire) == state) return; /* 前后两个状态相同，不发生更新，这样保持此状态开始时刻不变 */
			m_InGameState.store(state, std::memory_order_release);
			m_tp = std::chrono::system_clock::now();
		}
        IN_GAME_STATE GetInGameState() noexcept
        {
            return m_InGameState.load(std::memory_order_release);
        }
		void Recognize(std::vector<std::string>&);
		void Dispatch();
		void Analyze();
    };
}
