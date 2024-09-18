#include <Windows.h>

class CCSOLUtilEvent
{
public:
    CCSOLUtilEvent();
    ~CCSOLUtilEvent() noexcept;
private:
    enum { START, SUSPEND, EXIT } m_EventType;
    HANDLE m_hMutex;
};