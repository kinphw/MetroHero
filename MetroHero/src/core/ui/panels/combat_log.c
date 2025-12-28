#define _CRT_SECURE_NO_WARNINGS
#include "combat_log.h"
#include "../layout.h"
#include "../text/render.h"
#include "../text/glyph.h"
#include "../widgets/box.h"
#include "../../../world/glyph.h"
#include <stdio.h>
#include <string.h>
#include "raylib.h" 

// Combat Log Buffer
static char combat_log_buf[COMBAT_LOG_LINES][256];
static int combat_log_index = 0;

void ui_add_combat_log(const char* msg) {
    snprintf(combat_log_buf[combat_log_index], sizeof(combat_log_buf[combat_log_index]), "%s", msg);
    combat_log_index = (combat_log_index + 1) % COMBAT_LOG_LINES;
}

void ui_draw_combat_log(void) {
    int x = COMBAT_LOG_X;
    int y = COMBAT_LOG_Y;
    int w = COMBAT_LOG_W;
    int h = COMBAT_LOG_H;

    ui_draw_box(x, y, w, h, "전투");

    int start = (combat_log_index - (h - 2) + COMBAT_LOG_LINES) % COMBAT_LOG_LINES;
    for (int i = 0; i < h - 2; i++) {
        // Log text with potential ANSI
        const char* logText = combat_log_buf[(start + i) % COMBAT_LOG_LINES];

        // ★ 텍스트 그리기
        int actualWidth = ui_draw_text_clipped(x + 2, y + 1 + i, w - 4, logText, NULL);

        // ★ 남은 공간 공백 채우기
        int textEndX = x + 2 + actualWidth;
        for (int j = textEndX; j < x + w - 2; j++) {
            ui_draw_str_at(j, y + 1 + i, " ", NULL);
        }
    }
}

// ★ 적 이미지 표시 (전투 박스 오른쪽 중앙)
void ui_draw_enemy_image(const char* imagePath) {
    if (!imagePath) return;

    int x = COMBAT_LOG_X;
    int w = COMBAT_LOG_W;
    int h = COMBAT_LOG_H;

    // 이미지 크기 및 위치 계산
    int imgSize = 120;

    // 박스 오른쪽에 위치 (픽셀 좌표)
    int boxRightPx = (x + w) * 8;  // 박스 오른쪽 끝
    int boxTopPx = COMBAT_LOG_Y * 16;  // 박스 상단
    int boxHeightPx = h * 16;

    // 오른쪽 중앙에 배치
    int imgX = boxRightPx - imgSize - 16;  // 오른쪽에서 16px 여백
    int imgY = boxTopPx + (boxHeightPx - imgSize) / 2;  // 수직 중앙

    ui_draw_image(imgX, imgY, imgSize, imgSize, imagePath);
}
