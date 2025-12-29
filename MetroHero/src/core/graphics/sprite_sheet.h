#ifndef SPRITE_SHEET_H
#define SPRITE_SHEET_H

#include "raylib.h"

typedef struct {
    Texture2D texture;
    int rows;
    int cols;
    int frameWidth;
    int frameHeight;
} SpriteSheet;

void sprite_load(SpriteSheet* sheet, const char* path, int rows, int cols);
void sprite_draw(const SpriteSheet* sheet, int row, int col, int x, int y, int w, int h);
void sprite_unload(SpriteSheet* sheet);

#endif
