#include <Windows.h>
#include <iostream>

DWORD CALLBACK thread(LPVOID lpParam)
{
    HANDLE hEvent = lpParam;
    std::cout << "Event: " << hEvent << std::endl;
    while (WAIT_OBJECT_0 == WaitForSingleObject(hEvent, INFINITE))
    {
        std::cout << "event set." << std::endl;
    }
    ExitThread(ERROR_SUCCESS);
}

int main()
{
    HANDLE hEvent = CreateEvent(nullptr, true, false, "Local\\test_event");
    std::cout << "Event: " << hEvent << std::endl;
    HANDLE hThread = CreateThread(
        nullptr,
        4096,
        thread,
        hEvent,
        0, // create running
        nullptr
    );
    Sleep(5000);
    SetEvent(hEvent);
    ExitThread(ERROR_SUCCESS);
//  WaitForSingleObject(hThread, INFINITE);
//  CloseHandle(hThread);
//  CloseHandle(hEvent);
}
