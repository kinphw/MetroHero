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
    { '.', "assets/tile/floor.png", 1, "Tiled Floor" },
    { ',', "assets/tile/grass_floor.png", 1, "Grass Floor" },
    { '-', "assets/tile/gravel_floor.png", 1, "Gravel Floor" },
    { '_', "assets/tile/dirt_floor.png", 1, "Dirt Floor" },
    { '~', "assets/tile/water.png", 0, "Water" },

    // Walls (Blocked)
    { '#', "assets/tile/wall.png", 0, "Gray Wall" },
    { '%', "assets/wall.png", 0, "Brick Wall" },
    { '&', "assets/tile/metal_wall.png", 0, "Iron Wall" },
    { '$', "assets/tile/glass_wall.png", 0, "Glass Wall" },
    { 'O', "assets/tile/rock_obstacle.png", 0, "Rock" },
    { '!', "assets/tile/signpost.png", 0, "Signpost" },
    
    // Objects
    { '+', "assets/wall.png", 0, "Closed Door" }, // Placeholder
    { '/', "assets/tile/rock_obstacle.png", 0, "Rock obstacle" },  // Placeholder
    { 'T', "assets/tile/tree_dark.png", 0, "Dark Tree" },
    { 't', "assets/tile/tree_light.png", 0, "Light Tree" },
    { 'B', "assets/tile/barricade.png", 0, "Barricade" },
    { '=', "assets/tile/rail_horizontal.png", 1, "Rail (Horizontal)" },
    { '|', "assets/tile/rail_vertical.png", 1, "Rail (Vertical)" },
    { '<', "assets/tile/stairs_down.png", 1, "Stairs Down" },
    { '>', "assets/tile/stairs_up.png", 1, "Stairs Up" },

    // Special
    { '@', "assets/floor.png", 1, "Spawn Point" }, // Default floor
    
    // Chests (0-9) - Blocked
    // Note: Chest graphics are handled dynamically or via ChestConfig, but mapping here helps fallback
    { '0', "assets/entity/chest_closed.png", 0, "Chest 0" },
    { '1', "assets/entity/chest_closed.png", 0, "Chest 1" },
    { '2', "assets/entity/chest_closed.png", 0, "Chest 2" },
    { '3', "assets/entity/chest_closed.png", 0, "Chest 3" },
    { '4', "assets/entity/chest_closed.png", 0, "Chest 4" },
    { '5', "assets/entity/chest_closed.png", 0, "Chest 5" },
    { '6', "assets/entity/chest_closed.png", 0, "Chest 6" },
    { '7', "assets/entity/chest_closed.png", 0, "Chest 7" },
    { '8', "assets/entity/chest_closed.png", 0, "Chest 8" },
    { '9', "assets/entity/chest_closed.png", 0, "Chest 9" },
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

void load_map(Map* m, int stageNumber, int floorIndex) {
    const StageData* data = get_stage_data(stageNumber);
    if (data && floorIndex >= 0 && floorIndex < data->floor_count) {
        const SubMapConfig* floor = &data->floors[floorIndex];
        
        load_map_from_lines(m, floor->mapLines, floor->mapHeight);
        
        // Load Warp Data
        m->warps = floor->warps;
        m->warpCount = floor->warpCount;
        m->floorIndex = floorIndex;
    } else {
        // Fallback or empty map
        m->height = 0;
        m->width = 0;
        m->warpCount = 0;
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
