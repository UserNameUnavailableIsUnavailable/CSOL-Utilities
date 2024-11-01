#include <CControlUnit.hpp>

namespace CSOL_Utilities
{
class CMainControlUnit : public CControlUnit
{
public:
    template <typename Callable, typename... VarArg>
    CMainControlUnit(Callable&& c, VarArg... va);
    ~CMainControlUnit();
private:
};
} /* namespace CSOL_Utilities */