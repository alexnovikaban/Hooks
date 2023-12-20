#include "pch.h"
#include <Windows.h>
#include <iostream>

const char* text = "jrn";
DWORD x = 0x400;
DWORD y = 0x400;
DWORD original_print_addr = 0x00419880;
DWORD hook_print_addr     = 0x0040BE7E;
DWORD print_return_addr   = 0x0040BE83;


__declspec(naked) void print_code_cave() {
    __asm {
        call original_print_addr    // Print Func: Speed-Anzeige

        pushad
        mov ecx, text               // my string
        push y                      // y coord
        push x                      // x coord
        call original_print_addr    // Print Func: Unser Text
        add esp, 8                  // Clean Stack      

        popad
        jmp print_return_addr
    }
}

void hooking(DWORD hook_addr, int length, void(*codecave)) {
    BYTE* loc = (BYTE*)hook_addr;
    DWORD oldProtect;
    VirtualProtect(loc, length, PAGE_EXECUTE_READWRITE, &oldProtect);

    for (int i = 0; i < length; i++)
        *(loc + i) = 0x90;

    *loc = 0xE9;
    *(DWORD*)(loc + 1) = (DWORD)print_code_cave - ((DWORD)hook_print_addr + 5);
}

void HackThread(HMODULE hModule) {
    // Console
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    DWORD process_id = GetCurrentProcessId();
    std::cout << "Assault Cube - Hooks" << std::endl;
    std::cout << "====================" << std::endl;

    Sleep(1000);
    hooking(hook_print_addr, 5, print_code_cave);

    // Loop
    while (true) {
        // Close Internal
        if (GetAsyncKeyState(VK_NUMPAD9) & 1) {
            break;
        }

        Sleep(10);
    }

    // Cleanup
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, 0);
        break;
    }
    return TRUE;
}

