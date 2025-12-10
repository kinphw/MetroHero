#define _CRT_SECURE_NO_WARNINGS
#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Cell* screenBuffer = NULL;
int bufferWidth = 0;
int bufferHeight = 0;
char globalColor[32] = "\033[0m"; 

void ui_init_buffer(void) {
    // 1. Setup Console
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    
    // Set UTF-8 Output
    SetConsoleOutputCP(CP_UTF8);

    // Fullscreen setup (Alt+Enter simulation)
    keybd_event(VK_MENU, 0x38, 0, 0);
    keybd_event(VK_RETURN, 0x1c, 0, 0);
    keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
    Sleep(100);

    // 2. Allocate Buffer
    bufferWidth = 168; 
    bufferHeight = 42; 
    screenBuffer = (Cell*)malloc(sizeof(Cell) * bufferWidth * bufferHeight);
    
    // Hide Cursor
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 1;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &info);

    ui_clear_buffer();
}

void ui_clear_buffer(void) {
    if (!screenBuffer) return;
    for (int i = 0; i < bufferWidth * bufferHeight; i++) {
        strcpy(screenBuffer[i].ch, " ");
        strcpy(screenBuffer[i].color, "\033[0m"); 
        screenBuffer[i].isDoubleWidth = 0;
    }
    strcpy(globalColor, "\033[0m"); // Reset global color state
}

void ui_present(void) {
    if (!screenBuffer) return;

    // Estimate buffer size: Width * Height * (Char(4) + Color(12)) + Newlines
    static char* outBuf = NULL;
    static int outBufSize = 0;
    int requiredSize = bufferWidth * bufferHeight * 20; 
    
    if (outBuf == NULL || outBufSize < requiredSize) {
        free(outBuf);
        outBuf = (char*)malloc(requiredSize);
        outBufSize = requiredSize;
    }

    char* p = outBuf;
    
    // Reset cursor to top-left
    p += sprintf(p, "\033[H"); 
    
    char lastColor[32] = "";
    
    for (int y = 0; y < bufferHeight; y++) {
        for (int x = 0; x < bufferWidth; x++) {
            int idx = y * bufferWidth + x;
            Cell* c = &screenBuffer[idx];
            
            if (c->isDoubleWidth == -1) continue; // Skip padding
            
            // Only print color if changed
            if (strcmp(c->color, lastColor) != 0) {
                p += sprintf(p, "\033[0m%s", c->color); // Reset before applying new color to avoid BG bleed
                strcpy(lastColor, c->color);
            }
            
            p += sprintf(p, "%s", c->ch);
        }
        if (y < bufferHeight - 1) {
             p += sprintf(p, "\n");
        }
    }
    
    // Write whole frame at once
    DWORD written;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), outBuf, (DWORD)(p - outBuf), &written, NULL);
}

void ui_draw_char_at(int x, int y, const char* c, const char* color) {
    if (x < 0 || x >= bufferWidth || y < 0 || y >= bufferHeight) return;
    
    int index = y * bufferWidth + x;
    
    strncpy(screenBuffer[index].ch, c, 4);
    screenBuffer[index].ch[4] = '\0';
    
    if (color) strcpy(screenBuffer[index].color, color);
    else strcpy(screenBuffer[index].color, "\033[0m");
    
    screenBuffer[index].isDoubleWidth = 0; 
}
