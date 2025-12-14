#include "chest.h"

void chest_init(Chest* c, int x, int y, char tile, const char* itemType, const char* itemName, const char* imagePath) {
    c->x = x;
    c->y = y;
    c->tile = tile;
    c->isOpened = 0;
    c->itemType = itemType;
    c->itemName = itemName;
    c->imagePath = imagePath;
}
