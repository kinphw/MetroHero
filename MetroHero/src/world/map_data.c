#include "map_data.h"
#include <string.h>
#include <stdlib.h>

#include "glyph.h"
#include "../stages/stage_01/stage_01.h"
#include "../stages/stage_02/stage_02.h"
#include "../stages/stage_03/stage_03.h"

// ========================
// Global Tile Palette
// ========================
// ========================
// Global Tile Palette
// ========================
const TileDef GLOBAL_TILE_PALETTE[] = {
    // Floors (Walkable)
    { '.', "\033[48;5;240m..\033[0m",   1, "Tiled Floor" },
    { ',', "\033[32m,,\033[0m",        1, "Grass Floor" },
    { '-', "\033[48;5;235m--\033[0m",   1, "Paved Floor" },
    { '_', "\033[93m..\033[0m",        1, "Dirt Floor" },
    { '~', "\033[44m  \033[0m",        1, "Water" },

    // Walls (Blocked)
    { '#', "\033[48;5;236m  \033[0m",  0, "Gray Wall" },
    { '%', "\033[48;5;124m  \033[0m",  0, "Brick Wall" },
    { '&', "\033[48;5;22m  \033[0m",   0, "Iron Wall" },
    { '$', "\033[48;5;123m  \033[0m",  0, "Glass Wall" },
    
    // Objects
    { '+', "🚪",                      0, "Closed Door" },
    { '/', "\033[30m▒▒\033[0m",        1, "Open Door" },
    { 'T', "🌲",                      0, "Tree" },
    { 'B', "🚧",                      0, "Barricade" },
    { '=', "||",                      1, "Rail" },
    { 'S', "▼▼",                      1, "Stairs Down" },
    { 'U', "▲▲",                      1, "Stairs Up" },

    // Special
    { '@', "  ",                      1, "Spawn Point" }, // Default floor
    
    // Chests (0-9) - Blocked
    { '0', "📦",  0, "Chest 0" },
    { '1', "📦",  0, "Chest 1" },
    { '2', "📦",  0, "Chest 2" },
    { '3', "📦",  0, "Chest 3" },
    { '4', "📦",  0, "Chest 4" },
    { '5', "📦",  0, "Chest 5" },
    { '6', "📦",  0, "Chest 6" },
    { '7', "📦",  0, "Chest 7" },
    { '8', "📦",  0, "Chest 8" },
    { '9', "📦",  0, "Chest 9" },
};
const int GLOBAL_TILE_PALETTE_COUNT = sizeof(GLOBAL_TILE_PALETTE) / sizeof(GLOBAL_TILE_PALETTE[0]);

// ========================
// 공통 로드 함수
// ========================
static void load_map_from_lines(Map* m, const char** mapData, int height) {
    m->height = height;
    m->width = strlen(mapData[0]);

    for (int y = 0; y < m->height && y < MAX_MAP_H; y++) {
        for (int x = 0; x < m->width && x < MAX_MAP_W; x++) {
            m->tiles[y][x] = mapData[y][x];
        }
    }
}

// ========================
// 데이터 접근 함수
// ========================

const StageData* get_stage_data(int stageNumber) {
    switch (stageNumber) {
        case 1: return &STAGE_01_DATA;
        case 2: return &STAGE_02_DATA;
        case 3: return &STAGE_03_DATA;
        default: return NULL;
    }
}

void load_map(Map* m, int stageNumber) {
    const StageData* data = get_stage_data(stageNumber);
    if (data) {
        load_map_from_lines(m, data->mapLines, data->mapHeight);
    } else {
        // Fallback or empty map
        // For safety, maybe clear map?
        m->height = 0;
        m->width = 0;
    }
}

const ChestConfig* get_chest_config(int stage, int* outCount) {
    const StageData* data = get_stage_data(stage);
    if (data) {
        *outCount = data->chestCount;
        return data->chests;
    }
    *outCount = 0;
    return NULL;
}
