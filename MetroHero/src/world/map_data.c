#include "map_data.h"
#include <string.h>
#include <stdlib.h>

#include "../stages/stage_01/stage_01.h"
#include "../stages/stage_02/stage_02.h"
#include "../stages/stage_03/stage_03.h"

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
