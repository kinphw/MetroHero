#include "player.h"

void player_init(Player* p) {
    p->x = MAP_W / 2;
    p->y = MAP_H / 2;
}

void player_move(Player* p, const Map* m, int cmd) {
    int nx = p->x;
    int ny = p->y;

    switch (cmd) {
    case 'w': ny--; break;
    case 's': ny++; break;
    case 'a': nx--; break;
    case 'd': nx++; break;
    }

    if (map_is_walkable(m, nx, ny)) {
        p->x = nx;
        p->y = ny;
    }
}
