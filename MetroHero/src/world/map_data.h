#ifndef MAP_DATA_H
#define MAP_DATA_H

#include "map.h"

typedef struct {
	char tile;              // 'A', 'B', 'C' ...
	const char* itemType;   // "weapon", "armor", "item"
	const char* itemName;   // "초보자 검"
} ChestConfig;

const ChestConfig* get_chest_config(int stage , int* outCount);

// ★ 단일 함수로 변경
void load_map(Map* m , int stageNumber);

#endif