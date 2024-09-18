#include <thread>
#include <mutex>

class CCSOLUtilThread : std::thread
{
public:
    
private:
    std::mutex m_Mutex;
};