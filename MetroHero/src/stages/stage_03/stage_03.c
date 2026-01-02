#include "stage_03.h"
#include <stdlib.h>

// ============================================
// Stage 3: Placeholder
// ============================================

static const char* MAP_LINES[] = { "#####", "#...#", "#####" };
static const SubMapConfig FLOORS[] = { { MAP_LINES, 3, NULL, 0 } };

const StageData STAGE_03_DATA = { 
    .stageId = 3, 
    .floors = FLOORS, 
    .floor_count = 1 
};
