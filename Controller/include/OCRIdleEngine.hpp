#pragma once

#include "IdleEngine.hpp"
#include "OCR.hpp"

namespace CSOL_Utilities
{
    struct OCRBackboneInformation
    {
        std::filesystem::path deteor_model_json_path;
        std::filesystem::path recognizer_model_json_path;
        std::filesystem::path keywords_json_path;
    };

    class OCRIdleEngine : public IdleEngine
    {
    public:
        OCRIdleEngine(std::unique_ptr<GameProcessInformation> game_process_information, std::unique_ptr<OCRBackboneInformation> ocr_backbone_information);
        OCRIdleEngine(
            std::filesystem::path deteor_model_json_path,
            std::filesystem::path recognizer_model_json_path,
            std::filesystem::path keywords_json_path
        );
        virtual void ResetAfterSwitchMode() noexcept override { game_interface_type_.store(GAME_INTERFACE_TYPE::UNKNOWN, std::memory_order_release); }
        virtual void ResetAfterReconnection() noexcept override { game_interface_type_.store(GAME_INTERFACE_TYPE::LOGIN, std::memory_order_release); }
    private:
		static constexpr std::string_view KEYWORD_CATEGORY_LOBBY = "LOBBY";
		static constexpr std::string_view KEYWORD_CATEGORY_ROOM = "ROOM";
		static constexpr std::string_view KEYWORD_CATEGORY_LOADING = "LOADING";
		static constexpr std::string_view KEYWORD_CATEGORY_INGAME = "IN_GAME";
		std::unordered_map<std::string, std::vector<std::string>> m_Keywords; /* 关键词 */
		std::atomic<GAME_INTERFACE_TYPE> game_interface_type_ = GAME_INTERFACE_TYPE::UNKNOWN; /* 游戏内状态 */
		std::chrono::system_clock::time_point timepoint_; /* 状态解析时刻 */
        std::unique_ptr<OCR> ocr_;
        cv::Mat game_screenshot_;
        std::vector<uint8_t> image_buf_;
        aho_corasick::trie trie_;
		std::function<void (HWND)> remove_window_border_;
    private:
        virtual void Discriminate() override;
		void SetInGameState(GAME_INTERFACE_TYPE state) noexcept
		{
            if (game_interface_type_.load(std::memory_order_acquire) == state) return; /* 前后两个状态相同，不发生更新，这样保持此状态开始时刻不变 */
			game_interface_type_.store(state, std::memory_order_release);
			timepoint_ = std::chrono::system_clock::now();
		}
        GAME_INTERFACE_TYPE GetInGameState() noexcept
        {
            return game_interface_type_.load(std::memory_order_release);
        }
		void Recognize(std::vector<std::string>&);
		void Dispatch();
		void Analyze();
    };
}
