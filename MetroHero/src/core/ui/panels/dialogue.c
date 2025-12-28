#define _CRT_SECURE_NO_WARNINGS
#include "dialogue.h"
#include "../text/render.h"
#include "../text/glyph.h"
#include "../widgets/box.h"
#include "../../../world/glyph.h" // For Colors
#include <stdio.h>
#include <string.h>

#include "../layout.h"
static char log_buf[LOG_LINES][256];
static int log_index = 0;

void ui_draw_dialogue(const NPC* npc) {
    int x = DIALOGUE_X;
    int y = DIALOGUE_Y;
    int w = DIALOGUE_W;
    int h = DIALOGUE_H;
    
    ui_draw_box(x, y, w, h, "대화");

    // Dynamic Header Height
    int headerHeight = 2; // Default
    if (npc->faceImagePath) {
        headerHeight = 9; // ~144px space (9*16). Image is 128px.
    }

    // 1. Name
    // 1. Name
    char nameBuf[256];
    snprintf(nameBuf, sizeof(nameBuf), "💬 %s", npc->name);
    ui_draw_text_clipped(x + 1, y + 1, w - 2, nameBuf, NULL);

    // 2. Face Image (In expanded header)
    if (npc->faceImagePath) {
        int imgW = 128;
        int imgH = 128;
        int imgX = (x + w) * 8 - imgW - 16; // Right aligned with padding
        int imgY = (y * 16) + 16; // Top padding (y*16 is box top, +16px down)
        
        ui_draw_image(imgX, imgY, imgW, imgH, npc->faceImagePath);
    }

    // 3. Separator (Below Header)
    int sepPxX = x * 8;
    int sepPxW = w * 8;
    int sepPxY = (y + headerHeight) * 16 + 7;
    DrawRectangle(sepPxX, sepPxY, sepPxW, 2, WHITE);

    // Old face block removed since we moved it up
    int faceWidthBytes = 0; // No longer needed


    // 4. Dialogue Content (Full Width)
    const char* dialogue = npc->dialogues[npc->currentDialogue];
    int lineStart = 0;
    int lineNum = 0;
    // content start: headerHeight + 1
    // content end: h - 3 (bottom separator)
    int contentStart = headerHeight + 1;
    int maxLines = h - 3 - contentStart; // Remaining lines for text

    for (int i = contentStart; i < h - 3 && lineNum < maxLines; i++) {
        char lineBuf[256] = "";  
        
        int currentWidth = 0;
        const char* dialoguePtr = dialogue + lineStart;
        int maxTextWidth = w - 2; // Full width restored!

        int charsProcessed = 0;

        while (*dialoguePtr) {
            // ANSI Check
            if (*dialoguePtr == '\033' || *dialoguePtr == 0x1B) {
                // Copy ANSI code to buffer
                const char* ansiStart = dialoguePtr;
                dialoguePtr++;
                if (*dialoguePtr == '[') {
                    while (*dialoguePtr && *dialoguePtr != 'm') dialoguePtr++;
                    if (*dialoguePtr == 'm') dialoguePtr++;
                }
                int codeBytes = (int)(dialoguePtr - ansiStart);
                if (strlen(lineBuf) + codeBytes < sizeof(lineBuf) - 1) {
                    strncat(lineBuf, ansiStart, codeBytes);
                }
                charsProcessed += codeBytes;
                continue;
            }
            
            // Newline Check - Force line break behavior
            if (*dialoguePtr == '\n') {
                int charBytes = 1;
                charsProcessed += charBytes;
                // Don't advance dialoguePtr here because the outer loop needs lineStart update, 
                // but lineStart uses charsProcessed relative to start of loop?
                // Wait, lineStart is index. lineStart += charsProcessed at end of loop.
                // So yes, we just consume the newline byte and break.
                break; 
            }

            // Tab/Return Check - Replace with space
            if (*dialoguePtr == '\r' || *dialoguePtr == '\t') {
                int charBytes = 1;
                int charW = 1;
                
                if (currentWidth + charW > maxTextWidth) break;
                
                if (strlen(lineBuf) + 1 < sizeof(lineBuf) - 1) {
                    strcat(lineBuf, " ");
                }
                currentWidth += charW;
                charsProcessed += charBytes;
                dialoguePtr += charBytes;
                continue;
            }

            // Width Check
            int charW = 1;
            int charBytes = 1;
            
            ui_get_glyph_info(dialoguePtr, &charBytes, &charW);
             
             if (currentWidth + charW > maxTextWidth) break;
             
             if (strlen(lineBuf) + charBytes < sizeof(lineBuf) - 1) {
                 strncat(lineBuf, dialoguePtr, charBytes);
             }
             currentWidth += charW;
             charsProcessed += charBytes;
             dialoguePtr += charBytes;
        }
        
        ui_draw_text_clipped(x + 1, y + i, w - 2, lineBuf, NULL);
        
        lineStart += charsProcessed;
        lineNum++;
        if (dialogue[lineStart] == '\0') break;
    }

    // Bottom Separator (Raylib Line)
    int botSepPxY = (y + h - 3) * 16 + 7;
    DrawRectangle(sepPxX, botSepPxY, sepPxW, 2, WHITE);

    // Buttons
    char buttonText[64];
    if (npc->currentDialogue < npc->dialogueCount - 1) {
        if (npc->canTrade) snprintf(buttonText, sizeof(buttonText), " [0]다음  [T]거래  [X]닫기");
        else snprintf(buttonText, sizeof(buttonText), " [0]다음  [X]닫기");
    }
    else {
        if (npc->canTrade) snprintf(buttonText, sizeof(buttonText), " [0]끝  [T]거래  [X]닫기");
        else snprintf(buttonText, sizeof(buttonText), " [0]끝  [X]닫기");
    }
    ui_draw_text_clipped(x + 1, y + h - 2, w - 2, buttonText, NULL);
}

void ui_clear_dialogue_area(void) {
    int x = DIALOGUE_X;
    int y = DIALOGUE_Y;
    int w = DIALOGUE_W;
    int h = DIALOGUE_H;
    for (int i = 0; i < h; i++) {
        char spaces[128] = "";
        for (int j = 0; j < w; j++) strcat(spaces, " ");
        ui_draw_str_at(x, y + i, spaces, NULL);
    }
}

void ui_add_log(const char* msg) {
    snprintf(log_buf[log_index], sizeof(log_buf[log_index]), "%s", msg);
    log_index = (log_index + 1) % LOG_LINES;
}

void ui_draw_log(void) {
    int x = LOG_X;
    int y = LOG_Y;
    int w = LOG_W;
    int h = LOG_H;

    ui_draw_box(x, y, w, h, "로그");

    int start = (log_index - (h - 2) + LOG_LINES) % LOG_LINES;
    for (int i = 0; i < h - 2; i++) {
        // Log text with potential ANSI
        const char* logText = log_buf[(start + i) % LOG_LINES];

        // ★ 텍스트 그리기
        int actualWidth = ui_draw_text_clipped(x + 2, y + 1 + i, w - 4, logText, NULL);

        // ★ 남은 공간 공백 채우기
        int textEndX = x + 2 + actualWidth;
        for (int j = textEndX; j < x + w - 2; j++) {
            ui_draw_str_at(j, y + 1 + i, " ", NULL);
        }
    }
}

