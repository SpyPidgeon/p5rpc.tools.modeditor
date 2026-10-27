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

std::array<const char*, 11>* potentialWndNames;
HWND GetGameWindow();
extern HWND gameWindow;

HWND GetGameWindow()
{
    HWND windowHandle = NULL;

    if (potentialWndNames == nullptr)
        return GetForegroundWindow();

    for (auto wndName : *potentialWndNames)
    {
        if (wndName == nullptr)
            continue;

        printf("[P5R Mod Editor] Checking window name: %s\n", wndName);
        int len = MultiByteToWideChar(CP_UTF8, 0, wndName, -1, nullptr, 0);
        std::wstring result(len - 1, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, wndName, -1, &result[0], len);

        windowHandle = FindWindowW(nullptr, result.c_str());

        if (windowHandle != NULL)
            return windowHandle;
    }

    if (windowHandle == NULL)
    {
        printf("[P5R Mod Editor] No valid window string found! Defaulting to GetForegroundWindow.");
        return GetForegroundWindow();
    }
}

void InitScan()
{
    std::string pattern;
    pattern = "0F B7 C1 48 8D 0D ? ? ? ? 48 69 C0 A0 02 00 00 48 01 C8";
    GetDatUnit = (GetDatUnitByID)(PatternScan(GetModuleHandle(NULL), pattern));

    pattern = "E8 ?? ?? ?? ?? 0F B6 4B ?? 40 84 FF";
    DWORD_PTR potentialWndNamesAddress = PatternScan(GetModuleHandle(NULL), pattern);

    potentialWndNamesAddress = GetAddressFromFuncCall(potentialWndNamesAddress);
    potentialWndNamesAddress = GetAddressFromMOV(potentialWndNamesAddress);

    if (potentialWndNamesAddress != NULL)
        potentialWndNamesAddress += 6;

    potentialWndNames = (std::array<const char*, 11>*)(potentialWndNamesAddress);

    gameWindow = GetGameWindow();

    for (auto panel : panels)
    {
        panel->ScanValues();
    }

    Exportable::currentDirectory = GetDLLPath("");

    pattern = "48 8B C4 48 81 EC 98 00 00 00 80 3D ? ? ? ? 00";
    oMouseState = (MouseState)PatternScan(GetModuleHandle(NULL), pattern);

    pattern = "48 8B 8D ?? ?? ?? ?? 0F 28 CF 48 81 8D ?? ?? ?? ?? 80 00 00 00";
    DWORD_PTR moveUpdateCall = PatternScan(GetModuleHandle(NULL), pattern);
    oMoveUpdate = (FldPCMoveUpdate)GetAddressFromFuncCall(moveUpdateCall + 21);

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

