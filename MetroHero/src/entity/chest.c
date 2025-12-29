#include "chest.h"

void chest_init(Chest* c, int x, int y, char tile, const char* itemType, const char* itemName, const char* imagePath) {
    c->x = x;
    c->y = y;
    c->tile = tile;
    c->isOpened = 0;
    c->itemType = itemType;
    c->itemName = itemName;
    c->imagePath = imagePath;
    
    // Initialize Event
    // memset(&c->event, 0, sizeof(c->event)); // Requires string.h
    // Manual init to 0
    c->event.reqFlag = 0;
    c->event.reqVal = 0;
    c->event.setFlag = 0;
    c->event.setVal = 0;
    c->event.failMsg = 0;
}
