#include "../world/map.h"
#include "player.h"


void player_init(Player* p) {
    p->x = MAP_W / 2;
    p->y = MAP_H / 2;

    p->weaponName = "목검";
    p->armorName = "천 갑옷";
    p->item1 = "HP 포션";
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
