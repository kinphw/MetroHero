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
    //case '#':
    //    return GLYPH_WALL;
    case '#':
        return GLYPH_WALL;  // 단일 패턴
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

int is_enemy_tile(char t) {
    return (t == 'a' || t == 'b');   // 몬스터 목록 지정
}

// ★ 맵에서 적 찾아서 초기화
void map_load_enemies(Map* m) {
    m->enemyCount = 0;

    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            char tile = m->tiles[y][x];

            // a~z 범위의 적 타일 찾기
            if (is_enemy_tile(tile)) {
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

void map_load_chests(Map* m) {
    m->chestCount = 0;

    int cfgCount = 0;
    const ChestConfig* cfg = get_chest_config(m->stageNumber, &cfgCount);

    if (cfg == NULL) return;

    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {

            char tile = m->tiles[y][x];

            // 상자 타일이 아닌 경우 skip
            if (tile < 'A' || tile > 'Z') continue;

            // 해당 tile의 config 찾기
            for (int i = 0; i < cfgCount; i++) {
                if (cfg[i].tile == tile) {

                    chest_init(
                        &m->chests[m->chestCount],
                        x, y,
                        cfg[i].itemType,
                        cfg[i].itemName
                    );
                    m->chestCount++;

                    //m->tiles[y][x] = '.'; // 상자 타일을 바닥으로 변경
                    break;
                }
            }
        }
    }
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

            // 1) 플레이어
            if (x == p->x && y == p->y) {
                printf(GLYPH_PLAYER);
                continue;
            }

            // 2) 적
            Enemy* enemy = map_get_enemy_at((Map*)m, x, y);
            if (enemy != NULL) {
                printf("%s", enemy_to_glyph(enemy->type));
                continue;
            }

            // 3) ★★★ 상자(Chest) 체크 — 여기 새로 추가하는 부분 ★★★
            Chest* chest = map_get_chest_at((Map*)m, x, y);
            if (chest != NULL && !chest->isOpened) {
                printf(GLYPH_CHEST);   // 예: 📦
                continue;
            }

            // 4) 기본 타일
            printf("%s", tile_to_glyph(m->tiles[y][x]));
        }
    }
}

int map_is_walkable(const Map* m, int x, int y) {
    if (x < 0 || x >= m->width || y < 0 || y >= m->height)
        return 0;

    char t = m->tiles[y][x];

    // ★ 상자(A~Z)는 이동 불가
    if (t >= 'A' && t <= 'Z')
        return 0;

    return (t == '.');
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

Chest* map_get_chest_at(Map* m, int x, int y) {
    for (int i = 0; i < m->chestCount; i++) {
        if (!m->chests[i].isOpened &&
            m->chests[i].x == x && m->chests[i].y == y) {

            return &m->chests[i];
        }
    }
    return NULL;
}

Chest* map_get_adjacent_chest(Map* m, int px, int py) {
    int dirs[4][2] = {
        {0,-1}, {0,1}, {-1,0}, {1,0}
    };

    for (int i = 0; i < 4; i++) {
        int nx = px + dirs[i][0];
        int ny = py + dirs[i][1];

        Chest* chest = map_get_chest_at(m, nx, ny);
        if (chest != NULL) return chest;
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
    //map_load_enemies(m);  // ★ 이 줄 추가!

    map_load_enemies(m);
    map_load_chests(m);
}


void map_draw_viewport(const Map* m, const Player* p,
    int startX, int startY,
    int viewW, int viewH)
{
    // 플레이어 중심 뷰 계산
    int viewX = p->x - viewW / 2;
    int viewY = p->y - viewH / 2;

    // 맵 범위를 벗어나면 클램프
    if (viewX < 0) viewX = 0;
    if (viewY < 0) viewY = 0;
    if (viewX + viewW > m->width)  viewX = m->width - viewW;
    if (viewY + viewH > m->height) viewY = m->height - viewH;
    if (viewX < 0) viewX = 0;
    if (viewY < 0) viewY = 0;

    for (int sy = 0; sy < viewH; sy++) {
        int my = viewY + sy;

        console_goto(startX, startY + sy);

        for (int sx = 0; sx < viewW; sx++) {
            int mx = viewX + sx;

            // 맵 범위 밖이면 공백
            if (mx < 0 || mx >= m->width ||
                my < 0 || my >= m->height)
            {
                printf("  ");
                continue;
            }

            // 플레이어 출력
            if (mx == p->x && my == p->y) {
                printf(GLYPH_PLAYER);
                continue;
            }

            // 적 출력
            Enemy* enemy = map_get_enemy_at((Map*)m, mx, my);
            if (enemy != NULL) {
                printf("%s", enemy_to_glyph(enemy->type));
                continue;
            }

            // 상자 출력
            Chest* chest = map_get_chest_at((Map*)m, mx, my);
            if (chest != NULL && !chest->isOpened) {
                printf(GLYPH_CHEST);
                continue;
            }

            // 기본 타일 출력
            printf("%s", tile_to_glyph(m->tiles[my][mx]));
        }
    }
}
