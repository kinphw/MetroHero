#ifndef MAP_DATA_H
#define MAP_DATA_H

#include "map.h"
#include "../stages/common.h"

// get_chest_config is kept for compatibility but implementation will change
const ChestConfig* get_chest_config(int stage , int* outCount);

// Global Tile Palette
extern const TileDef GLOBAL_TILE_PALETTE[];
extern const int GLOBAL_TILE_PALETTE_COUNT;

const StageData* get_stage_data(int stageNumber);

void load_map(Map* m , int stageNumber);

#endif