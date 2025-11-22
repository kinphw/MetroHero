#include <stdio.h>
#include <windows.h>
#include "map.h"
#include "../entity/player.h"
#include "../core/ui.h"   // ★ 반드시 필요
#include "map_data.h"  // ★ 추가

// -------------------------------
// 전각(2바이트) 문자 설정
// -------------------------------
static const char* tile_to_glyph(char t) {
    switch (t) {
    case '.':  // 바닥
        return "..";        // 전각 느낌의 가벼운 점
    case '#':  // 벽
        return "██";        // 전각 블록
    case '=':  // 철도 레일
        return "==";        // 전각 라인
    default:
        return "  ";        // 빈칸
    }
}

// -------------------------------
// 맵 초기화
// -------------------------------
void map_init(Map* m) {

    load_map_station1(m);

    /*for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            m->tiles[y][x] = '.';
        }
    }*/
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
    for (int y = 0; y < MAP_H; y++) {
        console_goto(startX, startY + y);

        for (int x = 0; x < MAP_W; x++) {
            if (x == p->x && y == p->y)
                printf(">>");
            else
                printf("%s", tile_to_glyph(m->tiles[y][x]));
        }
    }
}





// -------------------------------
// 이동 가능 여부 검사
// -------------------------------
int map_is_walkable(const Map* m, int x, int y) {
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H)
        return 0;

    return (m->tiles[y][x] == '.');
}
