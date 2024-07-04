#include <Windows.h>

unsigned long long qwTimeElapsed;

void ResetScript();
void SuspendScript();
void ResumeScript();
void PauseScript();

void StartGame();
void ChooseClass();
void UseMelee();
void UseWeapon();
void ConfirmRound();

void CombineParts();
void PurchaseItem();

int main()
{
    while (true)
    {
        if (GetAsyncKeyState(VK_RALT) && GetAsyncKeyState(VK_RCONTROL) && GetAsyncKeyState(VK_NUMPAD0))
        {

            break;
        }
        else if (GetAsyncKeyState(VK_RALT) && GetAsyncKeyState(VK_RCONTROL) && GetAsyncKeyState(VK_NUMPAD1))
        {

        }
        else if (GetAsyncKeyState(VK_RALT) && GetAsyncKeyState(VK_RCONTROL) && GetAsyncKeyState(VK_NUMPAD3))
        {

        }
        Sleep(5);
    }
}