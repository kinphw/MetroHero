#include <stdio.h>
// #include <windows.h> // Removed for Raylib
#include "map.h"
#include "../entity/player.h"
#include "../entity/enemy.h"  // ★ 추가
#include "../core/ui/ui.h"   // ★ 반드시 필요
#include "map_data.h"  // ★ 추가
#include "glyph.h"  // ★ 추가

// ★ 타일 정의 조회 Helper
const TileDef* map_get_tile_def(char symbol) {
    for (int i = 0; i < GLOBAL_TILE_PALETTE_COUNT; i++) {
        if (GLOBAL_TILE_PALETTE[i].symbol == symbol) {
            return &GLOBAL_TILE_PALETTE[i];
        }
    }
    return NULL;
}

// 타일 문자를 렌더링 문자로 변환
const char* tile_to_glyph(char t) {
    const TileDef* def = map_get_tile_def(t);
    if (def) {
        return def->glyph;
    }
    return GLYPH_EMPTY;
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
// ★ 맵에서 적 찾아서 초기화
void map_load_enemies(Map* m) {
    m->enemyCount = 0;
    
    const StageData* stageData = get_stage_data(m->stageNumber);
    if (!stageData) return;

    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            char tile = m->tiles[y][x];

            // Check against defined enemies for this stage
            for (int i = 0; i < stageData->enemyCount; i++) {
                if (stageData->enemies[i].tile == tile) {
                     if (m->enemyCount < MAX_ENEMIES) {
                        enemy_init(&m->enemies[m->enemyCount], &stageData->enemies[i], x, y);
                        m->enemyCount++;

                        // 적이 있던 자리는 바닥으로 변경
                        m->tiles[y][x] = '.';
                    }
                    break;
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
            if (tile < '0' || tile > '9') continue;

            // 해당 tile의 config 찾기
            for (int i = 0; i < cfgCount; i++) {
                if (cfg[i].tile == tile) {

                    chest_init(
                        &m->chests[m->chestCount],
                        x, y,
                        tile,  // ★ Pass the tile character
                        cfg[i].itemType,
                        cfg[i].itemName,
                        cfg[i].imagePath
                    );
                    m->chestCount++;

                    m->tiles[y][x] = '.'; // 상자 타일을 바닥으로 변경
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



// ★ map_is_walkable 수정 - NPC도 이동 불가
int map_is_walkable(const Map* m, int x, int y) {
    if (x < 0 || x >= m->width || y < 0 || y >= m->height)
        return 0;

    char t = m->tiles[y][x];

    // 상자(숫자)는 이동 불가
    if (t >= '0' && t <= '9')
        return 0;

    // NPC가 있는 위치는 이동 불가
    for (int i = 0; i < m->npcCount; i++) {
        if (m->npcs[i].x == x && m->npcs[i].y == y)
            return 0;
    }

    // 기본 타일 이동 가능 여부 확인
    const TileDef* def = map_get_tile_def(t);
    if (def) {
        return def->walkable;
    }

    return 0;
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
        if (m->chests[i].x == x && m->chests[i].y == y) {
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
    map_load_npcs(m);  // ★ 추가
}


// ★ map_draw_viewport 수정 - NPC 렌더링 추가
void map_draw_viewport(const Map* m, const Player* p,
    int startX, int startY,
    int viewW, int viewH)
{
    int viewX = p->x - viewW / 2;
    int viewY = p->y - viewH / 2;

    if (viewX < 0) viewX = 0;
    if (viewY < 0) viewY = 0;
    if (viewX + viewW > m->width)  viewX = m->width - viewW;
    if (viewY + viewH > m->height) viewY = m->height - viewH;
    if (viewX < 0) viewX = 0;
    if (viewY < 0) viewY = 0;

    for (int sy = 0; sy < viewH; sy++) {
        int my = viewY + sy;
        int screenY = startY + sy;
        
        for (int sx = 0; sx < viewW; sx++) {
            int mx = viewX + sx;
            int screenX = startX + sx * 2; // Assuming 2 chars per tile

            if (mx < 0 || mx >= m->width ||
                my < 0 || my >= m->height)
            {
                ui_draw_str_at(screenX, screenY, "  ", NULL);
                continue;
            }

            // 1. Base Tile (Background) - Always draw first
            const TileDef* def = map_get_tile_def(m->tiles[my][mx]);
            if (def && def->imagePath) {
                ui_draw_tile(screenX, screenY, def->imagePath);
            } else if (def) {
                ui_draw_str_at(screenX, screenY, def->glyph, NULL);
            } else {
                ui_draw_str_at(screenX, screenY, "  ", NULL);
            }

            // 2. Chest (Overlay)
            Chest* chest = map_get_chest_at((Map*)m, mx, my);
            if (chest != NULL) {
                if (chest->isOpened) {
                     ui_draw_tile(screenX, screenY, "assets/chest_open.png");
                } else {
                    if (chest->imagePath) {
                         ui_draw_tile(screenX, screenY, chest->imagePath);
                    } else {
                         ui_draw_tile(screenX, screenY, "assets/chest_closed.png");
                    }
                }
            }

            // 3. Enemy (Overlay)
            Enemy* enemy = map_get_enemy_at((Map*)m, mx, my);
            if (enemy != NULL) {
                if (enemy->imagePath) {
                    ui_draw_tile(screenX, screenY, enemy->imagePath);
                } else {
                    ui_draw_str_at(screenX, screenY, enemy->glyph, NULL);
                }
            }

            // 4. NPC (Overlay)
            NPC* npc = map_get_npc_at((Map*)m, mx, my);
            if (npc != NULL) {
                if (npc->imagePath) {
                    ui_draw_tile(screenX, screenY, npc->imagePath);
                } else {
                    ui_draw_str_at(screenX, screenY, npc->glyph, NULL);
                }
            }

            // 5. Player (Overlay)
            if (mx == p->x && my == p->y) {
                // TODO: p->direction based images
                ui_draw_tile(screenX, screenY, "assets/person_down.png");
            }
        }
    }
}

// src/world/map.c - NPC 관련 함수 추가

// 기존 코드 유지하고 아래 함수들 추가

// ★ 맵에서 NPC 찾아서 초기화
void map_load_npcs(Map* m) {
    m->npcCount = 0;
    
    const StageData* stageData = get_stage_data(m->stageNumber);
    if (!stageData) return;

    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->width; x++) {
            char tile = m->tiles[y][x];

            for (int i = 0; i < stageData->npcCount; i++) {
                if (stageData->npcs[i].tile == tile) {
                    if (m->npcCount < MAX_NPCS) {
                        npc_init(&m->npcs[m->npcCount], &stageData->npcs[i], x, y);
                        m->npcCount++;

                        // NPC가 있던 자리는 바닥으로 변경
                        m->tiles[y][x] = '.';
                    }
                    break;
                }
            }
        }
    }
}

// ★ 특정 위치에 있는 NPC 반환
NPC* map_get_npc_at(Map* m, int x, int y) {
    for (int i = 0; i < m->npcCount; i++) {
        if (npc_is_at(&m->npcs[i], x, y)) {
            return &m->npcs[i];
        }
    }
    return NULL;
}

// ★ 플레이어 인접한 NPC 찾기
NPC* map_get_adjacent_npc(Map* m, int px, int py) {
    int dirs[4][2] = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0}
    };

    for (int i = 0; i < 4; i++) {
        int nx = px + dirs[i][0];
        int ny = py + dirs[i][1];

        NPC* npc = map_get_npc_at(m, nx, ny);
        if (npc != NULL) return npc;
    }

    return NULL;
}