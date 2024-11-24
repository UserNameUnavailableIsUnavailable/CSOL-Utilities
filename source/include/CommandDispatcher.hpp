#include "ControlUnit.hpp"
#include "MailboxTypes.hpp"
#include "CSOL_Utilities.hpp"
#include <filesystem>
#include <fstream>

namespace CSOL_Utilities
{
class CommandDispatcher : public ControlUnit<Command>
{
friend void DispatchCommand(CommandDispatcher&);
public:
    explicit CommandDispatcher(ControlUnit& superior, std::filesystem::path p = std::filesystem::path(U"Executor") / U"$~cmd.lua");
    ~CommandDispatcher() noexcept = default;
private:
    std::ofstream m_Out;
    std::filesystem::path m_Path;
};
void DispatchCommand(CommandDispatcher&);
}