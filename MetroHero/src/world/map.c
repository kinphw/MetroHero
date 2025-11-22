#include <stdio.h>
#include <windows.h>
#include "map.h"
#include "../entity/player.h"
#include "../entity/enemy.h"  // ★ 추가
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

// ★ 적 타일을 렌더링 문자로 변환
const char* enemy_to_glyph(char type) {
    switch (type) {
    case 'a':
        return GLYPH_DOG;
    case 'b':
        return GLYPH_ORC;
    default:
        return "??";
    }
}

// ★ 스폰 포인트 찾기
void map_find_spawn(Map* m) {
    // 기본값 (중앙)
    m->spawnX = m->width / 2;
    m->spawnY = m->height / 2;

    // @ 찾기
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            if (m->tiles[y][x] == '@') {
                m->spawnX = x;
                m->spawnY = y;
                // @ 를 일반 바닥으로 변환
                m->tiles[y][x] = '.';
                return;  // 첫 번째 @ 만 사용
            }
        }
    }
}

// ★ 맵에서 적 찾아서 초기화
void map_load_enemies(Map* m) {
    m->enemyCount = 0;

    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            char tile = m->tiles[y][x];

            // a~z 범위의 적 타일 찾기
            if (tile >= 'a' && tile <= 'z') {
                if (m->enemyCount < MAX_ENEMIES) {
                    enemy_init(&m->enemies[m->enemyCount], tile, x, y);
                    m->enemyCount++;

                    // 적이 있던 자리는 바닥으로 변경
                    m->tiles[y][x] = '.';
                }
            }
        }
    }
}

// ★ 특정 위치에 있는 적 반환
Enemy* map_get_enemy_at(Map* m, int x, int y) {
    for (int i = 0; i < m->enemyCount; i++) {
        if (enemy_is_at(&m->enemies[i], x, y)) {
            return &m->enemies[i];
        }
    }
    return NULL;
}


// -------------------------------
// 맵 초기화
// -------------------------------
void map_init(Map* m, int stageNumber) {
    m->stageNumber = stageNumber;  // ★ 선택사항
    load_map(m, stageNumber);
    map_find_spawn(m);  // ★ 이 줄 추가!
    map_load_enemies(m);  // ★ 이 줄 추가!
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
            if (x == p->x && y == p->y) {
                // 플레이어
                printf(GLYPH_PLAYER);
            }
            else {
                // ★ 적이 있는지 확인
                Enemy* enemy = map_get_enemy_at((Map*)m, x, y);
                if (enemy != NULL) {
                    printf("%s", enemy_to_glyph(enemy->type));
                }
                else {
                    printf("%s", tile_to_glyph(m->tiles[y][x]));
                }
            }
        }
    }
}





int map_is_walkable(const Map* m, int x, int y) {
    if (x < 0 || x >= m->width || y < 0 || y >= m->height)
        return 0;

    // ★ 적이 있으면 이동 불가 (나중에 전투로 변경)
    Enemy* enemy = map_get_enemy_at((Map*)m, x, y);
    if (enemy != NULL) {
        return 0;  // 일단 이동 불가
    }

    return (m->tiles[y][x] == '.');
}


// ★ 플레이어 주변(상하좌우)에 적이 있는지 확인
Enemy* map_get_adjacent_enemy(Map* m, int px, int py) {
    // 상하좌우 4방향 체크
    int directions[4][2] = {
        {0, -1},  // 위
        {0, 1},   // 아래
        {-1, 0},  // 왼쪽
        {1, 0}    // 오른쪽
    };

    for (int i = 0; i < 4; i++) {
        int nx = px + directions[i][0];
        int ny = py + directions[i][1];

        Enemy* enemy = map_get_enemy_at(m, nx, ny);
        if (enemy != NULL) {
            return enemy;
        }
    }

    return NULL;
}

// ★ 적이 어느 방향에 있는지 반환
const char* map_get_enemy_direction(Map* m, int px, int py, Enemy* enemy) {
    if (enemy->x < px) return "왼쪽";
    if (enemy->x > px) return "오른쪽";
    if (enemy->y < py) return "위";
    if (enemy->y > py) return "아래";
    return "여기";
}