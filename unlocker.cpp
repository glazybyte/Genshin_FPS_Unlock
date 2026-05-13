#include <windows.h>
#include <commdlg.h>
#include <string>
#include <vector>
#include <iostream>
#pragma comment(lib, "Comdlg32.lib")

bool InjectDlls(HANDLE processHandle, const std::vector<std::wstring>& dllPaths)
{
    if (dllPaths.empty())
        return true;

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");

    LPVOID loadLibrary =
        (LPVOID)GetProcAddress(kernel32, "LoadLibraryW");

    if (!loadLibrary)
        return false;

    LPVOID remoteBuffer = VirtualAllocEx(
        processHandle,
        nullptr,
        0x1000,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (!remoteBuffer)
        return false;

    for (const auto& dllPath : dllPaths)
    {
        std::wcout << L"Injecting: " << dllPath << std::endl;

        SIZE_T sizeBytes =
            (dllPath.length() + 1) * sizeof(wchar_t);

        if (!WriteProcessMemory(
            processHandle,
            remoteBuffer,
            dllPath.c_str(),
            sizeBytes,
            nullptr))
        {
            std::wcout << L"WriteProcessMemory failed: "
                       << GetLastError() << std::endl;

            VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);
            return false;
        }

        HANDLE thread = CreateRemoteThread(
            processHandle,
            nullptr,
            0,
            (LPTHREAD_START_ROUTINE)loadLibrary,
            remoteBuffer,
            0,
            nullptr
        );

        if (!thread)
        {
            std::wcout << L"CreateRemoteThread failed: "
                       << GetLastError() << std::endl;

            VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);
            return false;
        }

        WaitForSingleObject(thread, INFINITE);

        DWORD remoteResult = 0;
        GetExitCodeThread(thread, &remoteResult);

        std::wcout << L"LoadLibraryW returned: 0x"
                   << std::hex
                   << remoteResult
                   << std::endl;

        CloseHandle(thread);

        std::vector<BYTE> zeroBuffer(sizeBytes, 0);

        WriteProcessMemory(
            processHandle,
            remoteBuffer,
            zeroBuffer.data(),
            sizeBytes,
            nullptr
        );
    }

    VirtualFreeEx(processHandle, remoteBuffer, 0, MEM_RELEASE);

    return true;
}
bool IsRunningAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;

    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(
            &NtAuthority,
            2,
            SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS,
            0, 0, 0, 0, 0, 0,
            &adminGroup)) {

        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    return isAdmin;
}
std::wstring PickExeFile()
{
    wchar_t fileName[MAX_PATH] = {};

    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);

    ofn.lpstrFilter =
        L"Executable Files (*.exe)\0*.exe\0"
        L"All Files (*.*)\0*.*\0";

    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;

    ofn.Flags =
        OFN_FILEMUSTEXIST |
        OFN_PATHMUSTEXIST;

    if (GetOpenFileNameW(&ofn))
        return fileName;

    return L"";
}
std::wstring CurrentDirectory()
{
    wchar_t path[MAX_PATH];

    GetModuleFileNameW(
        nullptr,
        path,
        MAX_PATH
    );

    std::wstring exePath = path;

    size_t pos = exePath.find_last_of(L"\\/");

    return exePath.substr(0, pos);
}
int main()
{
    if (!IsRunningAsAdmin()) {
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);

        ShellExecuteA(NULL, "runas", path, NULL, NULL, SW_SHOWNORMAL);
        return 0;
    }
    SetConsoleTitleW(L"Genshin FPS Unlocker by Glazybyte");

    std::wstring cd = CurrentDirectory();

    
    std::wstring dllPath = cd + L"\\payload.dll";

    std::wcout<<"Select GenshinImpact.exe to continue: "<<std::endl;
    system("pause");

    std::wstring exePath = PickExeFile();

    if (exePath.empty())
    {
        std::wcout << L"No EXE selected.\n";
        return 0;
    }

   
    std::wstring workingDir = exePath;

    size_t lastSlash = workingDir.find_last_of(L"\\/");

    if (lastSlash != std::wstring::npos)
    {
        workingDir = workingDir.substr(0, lastSlash);
    }

    STARTUPINFOW si = {};
    si.cb = sizeof(si);

    PROCESS_INFORMATION processInfo = {};

    DWORD creationFlags = CREATE_SUSPENDED;

    BOOL created = CreateProcessW(
        exePath.c_str(),
        nullptr,
        nullptr,
        nullptr,
        FALSE,
        creationFlags,
        nullptr,
        workingDir.c_str(),
        &si,
        &processInfo
    );

    if (!created)
    {
        std::wcout << L"Failed to launch Game. Error: " << GetLastError() << std::endl;

        return 0;
    }

    // std::wcout << L"Process launched.\n";
    // std::wcout << L"PID: "<< processInfo.dwProcessId << std::endl;

    std::vector<std::wstring> dlls =
    {
        dllPath
    };

    if (!InjectDlls(processInfo.hProcess, dlls))
    {
        // std::wcout << L"Injection failed.\n";
    }
    else
    {
        // std::wcout << L"Injection successful.\n";
    }


    ResumeThread(processInfo.hThread);

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);

    // system("pause");

    return 0;
}