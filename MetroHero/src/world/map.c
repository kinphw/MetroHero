#include <stdio.h>
#include "map.h"
#include "../entity/player.h"

void map_init(Map* m) {
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++)
            m->tiles[y][x] = '.';
}

void map_draw(const Map* m, const Player* p) {
    for (int y = 0; y < MAP_H; y++) {
        for (int x = 0; x < MAP_W; x++) {
            if (p->x == x && p->y == y)
                putchar('@');
            else
                putchar(m->tiles[y][x]);
        }
        putchar('\n');
    }
}

int map_is_walkable(const Map* m, int x, int y) {
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H) return 0;
    return (m->tiles[y][x] == '.');
}
