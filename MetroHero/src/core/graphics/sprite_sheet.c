#include "sprite_sheet.h"
#include <stdio.h>

void sprite_load(SpriteSheet* sheet, const char* path, int rows, int cols) {
    if (!sheet) return;

    sheet->texture = LoadTexture(path);
    sheet->rows = rows;
    sheet->cols = cols;

    if (sheet->texture.id == 0) {
        printf("Failed to load sprite sheet: %s\n", path);
        sheet->frameWidth = 0;
        sheet->frameHeight = 0;
        return;
    }
    
    // 텍스처 필터 (선명하게)
    SetTextureFilter(sheet->texture, TEXTURE_FILTER_POINT); // Or BILINEAR based on art style

    sheet->frameWidth = sheet->texture.width / cols;
    sheet->frameHeight = sheet->texture.height / rows;
    
    printf("Sprite Loaded: %dx%d (Frame: %dx%d)\n", 
        sheet->texture.width, sheet->texture.height, sheet->frameWidth, sheet->frameHeight);
}

void sprite_draw(const SpriteSheet* sheet, int row, int col, int x, int y, int w, int h) {
    if (!sheet || sheet->texture.id == 0) return;
    if (row < 0 || row >= sheet->rows || col < 0 || col >= sheet->cols) return;

    Rectangle src = {
        (float)(col * sheet->frameWidth),
        (float)(row * sheet->frameHeight),
        (float)sheet->frameWidth,
        (float)sheet->frameHeight
    };
    
    Rectangle dest = {
        (float)x, (float)y,
        (float)w, (float)h
    };
    
    Vector2 origin = { 0, 0 };
    DrawTexturePro(sheet->texture, src, dest, origin, 0.0f, WHITE);
}

void sprite_unload(SpriteSheet* sheet) {
    if (sheet && sheet->texture.id != 0) {
        UnloadTexture(sheet->texture);
        sheet->texture.id = 0;
    }
}
