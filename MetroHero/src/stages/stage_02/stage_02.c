#include "stage_02.h"
#include <stdlib.h>

// ============================================
// Stage 2: Placeholder
// ============================================

static const char* MAP_LINES[] = { "#####", "#...#", "#####" };
static const SubMapConfig FLOORS[] = { { MAP_LINES, 3, NULL, 0 } };

const StageData STAGE_02_DATA = { 
    .stageId = 2, 
    .floors = FLOORS, 
    .floor_count = 1 
};
