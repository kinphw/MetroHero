#include "../entity/player.h"

#ifndef MAP_HEADER
#define MAP_HEADER

#define MAP_W 40
#define MAP_H 20

typedef struct {
    char tiles[MAP_H][MAP_W];
} Map;

void map_init(Map* m);
void map_draw_at(const Map* m , const Player* p , int startX , int startY);
int map_is_walkable(const Map* m, int x, int y);

#endif
