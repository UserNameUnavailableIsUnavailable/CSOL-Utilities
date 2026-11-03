#include "Classifier.hpp"
#include "Command.hpp"
#include "IdleEngine.hpp"

namespace CSOL_Utilities
{
class ClassifierIdleEngine : public IdleEngine
{
  public:
    ClassifierIdleEngine(std::unique_ptr<GameProcessInformation> game_process_information,
                         std::filesystem::path classifier_model_json_path);
    virtual void ResetAfterSwitchMode() noexcept override
    {
        interface_type_.store(GAME_INTERFACE_TYPE::UNKNOWN, std::memory_order_release);
    }
    virtual void ResetAfterReconnection() noexcept override
    {
        interface_type_.store(GAME_INTERFACE_TYPE::LOGIN, std::memory_order_release);
    }

  protected:
    virtual void Discriminate() override;

  private:
    Command::TYPE command_type_ = Command::TYPE::CMD_NOP;       // 当前命令类型
    std::chrono::system_clock::time_point interface_timepoint_; /* 最近一次状态解析时刻 */
    static constexpr std::string_view INTERFACE_TYPE_LOBBY = "LOBBY";
    static constexpr std::string_view INTERFACE_TYPE_ROOM = "ROOM";
    static constexpr std::string_view INTERFACE_TYPE_LOADING = "LOADING";
    static constexpr std::string_view INTERFACE_TYPE_INGAME = "IN_GAME";
    static constexpr std::string_view INTERFACE_TYPE_RESULTS = "RESULTS";
    std::unique_ptr<Classifier> classifier_;          /* 图像分类器 */
    std::atomic<GAME_INTERFACE_TYPE> interface_type_; /* 游戏内状态 */
};
} // namespace CSOL_Utilities