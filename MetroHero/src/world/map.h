#ifndef MAP_H
#define MAP_H

#define MAP_W 40
#define MAP_H 15

typedef struct {
    char tiles[MAP_H][MAP_W];
} Map;

void map_init(Map* m);
void map_draw(const Map* m, const void* player); // player 구조체는 이후 정의할 것
int map_is_walkable(const Map* m, int x, int y);

#endif
