#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "signaturescan.h"
#include "skillstructs.h"
#include <type_traits>
#include "imguistyling.h"
#include "dx11_detour.h"
#include "panels.h"
#include "gamehooks.h"
#include "exportable.h"

HMODULE dll_handle;
extern ImFont* windowFont;

void InitScan()
{
    std::string pattern;
    pattern = "0F B7 C1 48 8D 0D ? ? ? ? 48 69 C0 A0 02 00 00 48 01 C8";
    GetDatUnit = (GetDatUnitByID)(PatternScan(GetModuleHandle(NULL), pattern));

    for (auto panel : panels)
    {
        panel->ScanValues();
    }

    Exportable::currentDirectory = GetDLLPath("");

    pattern = "48 8B C4 48 81 EC 98 00 00 00 80 3D ? ? ? ? 00";
    oMouseState = (MouseState)PatternScan(GetModuleHandle(NULL), pattern);

    pattern = "40 53 48 83 EC 50 48 8B 59 ?? 0F 29 74 24 ?? 0F 28 F1";
    oMoveUpdate = (FldPCMoveUpdate)PatternScan(GetModuleHandle(NULL), pattern);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)oMouseState, MouseStateHook);
    DetourAttach(&(PVOID&)oMoveUpdate, FldPCMoveUpdateHook);
    DetourTransactionCommit();
}

extern bool quit;
int WINAPI ModMenuMain()
{
    while (true)
    {
        Sleep(50);
        if (GetAsyncKeyState(VK_F1))
            break;
    }

    InitScan();

    IMGUI_CHECKVERSION();

    if (!DX11Hook())
    {
        printf("[P5R Mod Editor] Failed to hook dx11\n");
        return 1;
    }

    std::sort(panels.begin(), panels.end(), [](const Panel* a,const Panel* b)
        {
            return a->sortingPriority < b->sortingPriority;
        });

    printf("[P5R Mod Editor] Loaded Mod Editor\n");
    SetImReflectConfig();

    return 0;
}

extern "C" __declspec(dllexport) void __stdcall Init()
{
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ModMenuMain, 0,0,0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        dll_handle = hModule;
        FILE* newOut;
        freopen_s(&newOut, "CONOUT$", "w", stdout);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

