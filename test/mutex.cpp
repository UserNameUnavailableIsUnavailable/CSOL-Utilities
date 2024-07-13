#include <Windows.h>
#include <errhandlingapi.h>
#include <synchapi.h>
#include <iostream>
int main()
{
    HANDLE hMutex = CreateMutexW(NULL, FALSE, L"Local\\mutex");
    if (!hMutex)
    {
        std::cout << "Create mutex failed. Error code: " << GetLastError() << std::endl;
    }
    auto ret = WaitForSingleObject(hMutex, 0);
    std::cout << "Wait result: " << ret << std::endl;
    if (!ReleaseMutex(hMutex))
    {
        std::cout << "Thread does not own the mutex." << std::endl;
    }
    ret = WaitForSingleObject(hMutex, INFINITE);
    std::cout << "Wait result: " << ret << std::endl;
    if (!ReleaseMutex(hMutex))
    {
        std::cout << "Thread does not own the mutex." << std::endl;
    }
    CloseHandle(hMutex);
    return 0;
}