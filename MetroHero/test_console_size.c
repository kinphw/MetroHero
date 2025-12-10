#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

int main() {
    // Enable Virtual Terminal Processing (just in case)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    
    // Simulate Alt+Enter (Fullscreen) - same as game
    printf("Switching to Fullscreen in 2 seconds...\n");
    Sleep(2000);
    
    keybd_event(VK_MENU, 0x38, 0, 0);
    keybd_event(VK_RETURN, 0x1c, 0, 0);
    keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
    
    Sleep(1000); // Wait for transition
    
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    
    int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    
    // Check Largest Console Window Size
    COORD maxWindow = GetLargestConsoleWindowSize(hOut);
    
    printf("\n=== Console Dimensions ===\n");
    printf("Current Window Size: %d x %d\n", cols, rows);
    printf("Max Possible Size:   %d x %d\n", maxWindow.X, maxWindow.Y);
    printf("Buffer Size:         %d x %d\n", csbi.dwSize.X, csbi.dwSize.Y);
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
