// dllmain.cpp : Defines the entry point for the DLL application.
#define WIN32_LEAN_AND_MEAN             
#include <windows.h>
#include <iostream>

#ifdef _DEBUG
#define CONSOLE
#endif

// Move the initialization sequence of this entire file to the "library" stage
#pragma init_seg(lib)

//Global variables
HMODULE self = nullptr;
FILE* pStdIn = nullptr;
FILE* pStdOut = nullptr;
FILE* pStdErr = nullptr;
PVOID pHandleVec = nullptr;

struct Win32ConsoleSetup {
    Win32ConsoleSetup() {
#ifdef CONSOLE //If in debug version enable console.
        AllocConsole();
        freopen_s(&pStdIn, "CONIN$", "r", stdin);
        freopen_s(&pStdOut, "CONOUT$", "w", stdout);
        freopen_s(&pStdErr, "CONOUT$", "w", stderr);
        SetConsoleTitleW(L"Brick Rigs Property Runtime - Developer");
        SetConsoleOutputCP(CP_UTF8);
#endif // _DEBUG
    }
};
// This global object will now be constructed ahead of standard global variables
Win32ConsoleSetup global_console_bootstrapper;

#include <Hooking/MinHook/MinHook.h>
#include <BR-SDK.hpp>
#include "Hooks.hpp"

#pragma region brickrust

#define PLUGIN_NAME_BRICKRUST "Property Runtime"

struct ModInfo
{
    const char* name;
    const char* description;
    const char* version;
    const char* game_version;
    const char* authors;
};

extern "C" {
    __declspec(dllexport) ModInfo mod_info()
    {
        return ModInfo{
            .name = PLUGIN_NAME_BRICKRUST,
            .description = "Enables bricks to use the reflection based property system!",
            .version = "1.0.0",
            .game_version = "1.10.7",
            .authors = "American_Stig (tbgit) @Discord"
        };
    }

    __declspec(dllexport) void mod_init()
    {
        return;//Nothing atm
    }
}

//Definied in execption_handler.cpp
LONG WINAPI UpgradedExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo);

//Safley unloads the mod. Delay added to help with execution.
DWORD WINAPI UnloadThread(LPVOID lpParam) {
    Sleep(10);
    FreeLibraryAndExitThread(self, 0);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
    HMODULE hModule = static_cast<HMODULE>(lpReserved);
    self = hModule;

#ifdef CONSOLE
    std::cout << "Brick Rigs Property Runtime - American_Stig (tbgit) @Discord" << std::endl;
    std::cout << "API Reference: " << "https://github.com/tubaplayerdis/BR-PropertyRuntime" << std::endl;
    std::cout << "Property Runtime will cause FREEZES Sometimes - Press ENTER to fix" << std::endl;
    std::cout << "Property Runtime is in developer mode - Press F6 to uninject" << std::endl;
#endif

    std::cout << Signature::InternalResolveSignature("E8 ?? ?? ?? ?? 0F BA EE 08", Signature::TEXT, "BrickRigsModKitSteam-BrickRigs.dll", true) << std::endl;

    BR_SDK_Init();
    MH_Initialize();
    Hooks::SetupCreateObjectHook();

#ifdef CONSOLE
    while (true)
    {
        if (GetAsyncKeyState(VK_F7) & 0x8000)
        {
            std::cout << "uhh" << std::endl;
        }

        if (GetAsyncKeyState(VK_F6) & 0x8000)
        {
            break;
        }
    }

    CreateThread(nullptr, 0, UnloadThread, nullptr, 0, nullptr);
#endif

    return 0;
}

void CleanUp(HMODULE hModule)
{
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();

#ifdef CONSOLE
    fclose(pStdIn);
    fclose(pStdOut);
    fclose(pStdErr);
    SetStdHandle(STD_INPUT_HANDLE, nullptr);
    SetStdHandle(STD_OUTPUT_HANDLE, nullptr);
    SetStdHandle(STD_ERROR_HANDLE, nullptr);
    FreeConsole();
    PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
#endif

    RemoveVectoredExceptionHandler(pHandleVec);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        pHandleVec = AddVectoredExceptionHandler(1, UpgradedExceptionHandler);
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
    }

    if (reason == DLL_PROCESS_DETACH)
    {
        CleanUp(self);
    }
    return TRUE;
}
