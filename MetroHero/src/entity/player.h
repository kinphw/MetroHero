#ifndef PLAYER_H
#define PLAYER_H

#include "../world/map.h"

typedef struct {
    int x, y;
} Player;

void player_init(Player* p);
void player_move(Player* p, const Map* m, int cmd);

#endif
