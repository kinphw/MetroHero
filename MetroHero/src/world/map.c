#include <stdio.h>
#include <windows.h>
#include "map.h"
#include "../entity/player.h"
#include "../core/ui.h"   // ★ 반드시 필요
#include "map_data.h"  // ★ 추가
#include "glyph.h"  // ★ 추가

// 타일 문자를 렌더링 문자로 변환
const char* tile_to_glyph(char t) {
    switch (t) {
    case '.':
        return GLYPH_FLOOR;
    case '#':
        return GLYPH_WALL;
    case '=':
        return GLYPH_RAIL;
    case '+':
        return GLYPH_DOOR;
    case 'S':
        return GLYPH_STAIRS;
    default:
        return GLYPH_EMPTY;
    }
}

// -------------------------------
// 맵 초기화
// -------------------------------
void map_init(Map* m, int stageNumber) {
    m->stageNumber = stageNumber;  // ★ 선택사항
    load_map(m, stageNumber);
}

// -------------------------------
// 맵 렌더링
// -------------------------------
//void map_draw(const Map* m, const Player* p) {
//    for (int y = 0; y < MAP_H; y++) {
//        for (int x = 0; x < MAP_W; x++) {
//
//            if (p->x == x && p->y == y) {
//                // 플레이어 위치
//                printf(">>");   // 전각 플레이어 아이콘
//            }
//            else {
//                printf("%s", tile_to_glyph(m->tiles[y][x]));
//            }
//
//        }
//        putchar('\n');
//    }
//}

void map_draw_at(const Map* m, const Player* p, int startX, int startY) {
    for (int y = 0; y < m->height; y++) {
        console_goto(startX, startY + y);

        for (int x = 0; x < m->width; x++) {
            if (x == p->x && y == p->y)
                printf(GLYPH_PLAYER);  // ★ 상수 사용
            else
                printf("%s", tile_to_glyph(m->tiles[y][x]));
        }
    }
}





// -------------------------------
// 이동 가능 여부 검사
// -------------------------------
int map_is_walkable(const Map* m, int x, int y) {
    if (x < 0 || x >= m->width || y < 0 || y >= m->height)  // ★ 동적 크기 사용
        return 0;

    return (m->tiles[y][x] == '.');
}
