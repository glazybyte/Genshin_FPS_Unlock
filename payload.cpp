#include <windows.h>
#include <thread>

DWORD WINAPI MainThread(LPVOID lpParam)
{
    while (true)
    {
        MessageBoxW(
            nullptr,
            L"FPS Uncapped! Enjoy!",
            L"FPS Unlocker by Glazybyte",
            MB_OK
        );
        
        uintptr_t base =(uintptr_t)GetModuleHandleW(NULL);
        uintptr_t addr1 = base + 0x4B4087C;
        uintptr_t addr2 = base + 0x4DC3A18;
        uintptr_t addr3 = base + 0x4DC6328;
        uintptr_t addr4 = base + 0x4DC8A0C;
        while(true){
            *(int*)addr1 = 999;
            *(int*)addr2 = 999;
            *(int*)addr3 = 999;
            *(int*)addr4 = 999;
            Sleep(6700);
        }
    }

    return 0;
}

BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);

        HANDLE thread = CreateThread(
            nullptr,
            0,
            MainThread,
            nullptr,
            0,
            nullptr
        );

        if (thread)
            CloseHandle(thread);

        break;
    }
    }

    return TRUE;
}