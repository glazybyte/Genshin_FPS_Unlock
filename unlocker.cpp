#include <windows.h>
#include <iostream>
using namespace std;
#include <tlhelp32.h>
#include <shellapi.h>
#include <limits>

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

uintptr_t GetModuleBaseAddress(DWORD pid, const wchar_t* moduleName) {
    uintptr_t baseAddress = 0;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);

    MODULEENTRY32W moduleEntry;
    moduleEntry.dwSize = sizeof(moduleEntry);

    if (Module32FirstW(snapshot, &moduleEntry)) {
        do {
            if (!_wcsicmp(moduleEntry.szModule, moduleName)) {
                baseAddress = (uintptr_t)moduleEntry.modBaseAddr;
                break;
            }
        } while (Module32NextW(snapshot, &moduleEntry));
    }

    CloseHandle(snapshot);
    return baseAddress;
}

bool FileExists(const char* path) {
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES &&
           !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

bool PickExe(char* outPath) {
    OPENFILENAMEA ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Executable Files (*.exe)\0*.exe\0";
    ofn.lpstrFile = outPath;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;

    return GetOpenFileNameA(&ofn);
}

DWORD LaunchProcessGetPID(const char* path) {
    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);

    if (!CreateProcessA(path, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cout << "Failed to launch. Error: " << GetLastError() << "\n";
        return 0;
    }

    DWORD pid = pi.dwProcessId;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return pid;
}

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

DWORD GetProcessIdByName(const wchar_t* processName) {
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32FirstW(snapshot, &pe)) {
        do {
            if (!_wcsicmp(pe.szExeFile, processName)) {
                CloseHandle(snapshot);
                return pe.th32ProcessID;
            }
        } while (Process32NextW(snapshot, &pe));
    }

    CloseHandle(snapshot);
    return 0;
}

int main(){
if (!IsRunningAsAdmin()) {
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);

        ShellExecuteA(NULL, "runas", path, NULL, NULL, SW_SHOWNORMAL);
        return 0;
    }
    // const char* defaultPath = "F:\\HoYoPlay\\games\\Genshin Impact game\\GenshinImpact.exe";

    // char path[MAX_PATH] = { 0 };

    // if (FileExists(defaultPath)) {
    //     std::cout << "Using default exe.\n";
    //     strcpy_s(path, defaultPath);
    // } else {
    //     std::cout << "Default exe not found. Please select one.\n";

    //     if (!PickExe(path)) {
    //         std::cout << "No file selected. Exiting.\n";
    //         return 1;
    //     }
    // }

    // std::cout << "Launching: " << path << "\n";

    // DWORD pid = LaunchProcessGetPID(path);

    // if (pid == 0) {
    //     std::cout << "Launch failed.\n";
    //     return 1;
    // }


    DWORD pid = GetProcessIdByName(L"GenshinImpact.exe");
    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);

    if(!hProcess) {
        cout << "Failed to open process\n";
        return 1;
    }
    uintptr_t base = GetModuleBaseAddress(pid, L"GenshinImpact.exe");

    uintptr_t addresses[] = {0x4B4087C, 0x4DC3A18, 0x4DC6328, 0x4DC8A0C};
    cout<< "Enabling uncapped"<<endl;
    for(int i=0;i<4;i++){
        uintptr_t address = base + addresses[i];
        int value = 0;
        ReadProcessMemory(hProcess, (LPCVOID)address, &value, sizeof(value), nullptr);
        int newValue = 999;
        WriteProcessMemory(hProcess, (LPVOID)address, &newValue, sizeof(newValue), nullptr);
    }
    

    cout << "Press any key to exit...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin.get();

}