// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "Base.h"
#include "game.h"
#include "UI.h"

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartHook, NULL, 0, NULL);

        if (hThread)
            CloseHandle(hThread);
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        UnloadHook(); // 卸载钩子
    }
    return TRUE;
}

