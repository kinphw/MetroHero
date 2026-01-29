#define _CRT_SECURE_NO_WARNINGS
#include "save.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../../entity/item.h"
#include "../ui/ui.h" // For logs
#include "../../world/glyph.h" // For COLOR codes
#include "../../world/map.h" // Added for Map access

#define SAVE_FILE_NAME "save.dat"
#define SAVE_MAGIC "HERO"
#define SAVE_VERSION 2

// Helper: Write string with fixed size
static void write_string(FILE* f, const char* str) {
    char buf[32] = {0};
    if (str) strncpy(buf, str, 31);
    fwrite(buf, sizeof(char), 32, f);
}

// Helper: Read string with fixed size
static void read_string(FILE* f, char* buf, int size) {
    fread(buf, sizeof(char), size, f);
    buf[size - 1] = '\0'; // Ensure null term
}

int save_file_exists(void) {
    FILE* f = fopen(SAVE_FILE_NAME, "rb");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

void save_game(GameState* state) {
    FILE* f = fopen(SAVE_FILE_NAME, "wb");
    if (!f) {
        ui_add_log(COLOR_RED "저장 실패: 파일을 열 수 없습니다." COLOR_RESET);
        return;
    }

    // 1. Header
    fwrite(SAVE_MAGIC, 1, 4, f);
    int version = SAVE_VERSION;
    fwrite(&version, sizeof(int), 1, f);

    // 2. Global State
    fwrite(&state->currentFloor, sizeof(int), 1, f);
    fwrite(&state->questState, sizeof(int), 1, f);
    fwrite(&state->eventRegistry, sizeof(EventRegistry), 1, f); // POD
    
    // Global Strings (Quest) - Use full size 1024
    fwrite(state->activeQuestMsg, sizeof(char), 1024, f);
    fwrite(state->pendingQuestMsg, sizeof(char), 1024, f);

    // 3. Player Basic Stats (POD portion)
    // We serialize manually to avoid pointer issues with SpriteSheet etc.
    Player* p = &state->player;
    fwrite(&p->x, sizeof(int), 1, f);
    fwrite(&p->y, sizeof(int), 1, f);
    fwrite(&p->dirX, sizeof(int), 1, f);
    fwrite(&p->dirY, sizeof(int), 1, f);
    fwrite(&p->hp, sizeof(int), 1, f);
    fwrite(&p->maxHp, sizeof(int), 1, f);
    fwrite(&p->baseMaxHp, sizeof(int), 1, f);
    fwrite(&p->level, sizeof(int), 1, f);
    fwrite(&p->exp, sizeof(int), 1, f);
    fwrite(&p->expNext, sizeof(int), 1, f);
    // Attack is derived from item, but base stats needed
    fwrite(&p->baseAttackMin, sizeof(int), 1, f);
    fwrite(&p->baseAttackMax, sizeof(int), 1, f);

    // 4. Inventory
    int count = p->inventory.count;
    fwrite(&count, sizeof(int), 1, f);
    for (int i = 0; i < count; i++) {
        const char* name = "";
        if (p->inventory.items[i]) name = p->inventory.items[i]->name;
        write_string(f, name);
    }

    // 5. Equipment
    const char* weaponName = "";
    if (p->equippedWeapon) weaponName = p->equippedWeapon->name;
    write_string(f, weaponName);

    const char* armorName = "";
    if (p->equippedArmor) armorName = p->equippedArmor->name;
    write_string(f, armorName);

    // 6. MAP & ENTITIES (Full Serialization)
    // A. Tiles (Preserve terrain changes like Secret Rock)
    fwrite(&state->map.width, sizeof(int), 1, f);
    fwrite(&state->map.height, sizeof(int), 1, f);
    for (int i = 0; i < state->map.height; i++) {
        fwrite(state->map.tiles[i], sizeof(char), state->map.width, f);
    }

    // B. Enemies (Preserve Death & Position)
    fwrite(&state->map.enemyCount, sizeof(int), 1, f);
    for (int i = 0; i < state->map.enemyCount; i++) {
        Enemy* e = &state->map.enemies[i];
        fwrite(&e->x, sizeof(int), 1, f);
        fwrite(&e->y, sizeof(int), 1, f);
        fwrite(&e->hp, sizeof(int), 1, f);
        fwrite(&e->isDead, sizeof(int), 1, f);
        fwrite(&e->randomMoveTimer, sizeof(float), 1, f); // Random move timer
    }

    // C. Chests (Preserve Open State)
    fwrite(&state->map.chestCount, sizeof(int), 1, f);
    for (int i = 0; i < state->map.chestCount; i++) {
        Chest* c = &state->map.chests[i];
        fwrite(&c->isOpened, sizeof(int), 1, f);
    }

    // D. Doors (Preserve Open State)
    fwrite(&state->map.doorCount, sizeof(int), 1, f);
    for (int i = 0; i < state->map.doorCount; i++) {
        Door* d = &state->map.doors[i];
        fwrite(&d->isOpen, sizeof(int), 1, f);
    }

    fclose(f);
    ui_add_log(COLOR_BRIGHT_GREEN "게임이 저장되었습니다." COLOR_RESET);
}

int load_game(GameState* state) {
    FILE* f = fopen(SAVE_FILE_NAME, "rb");
    if (!f) return 0;

    // 1. Header Check
    char magic[4];
    fread(magic, 1, 4, f);
    if (strncmp(magic, SAVE_MAGIC, 4) != 0) {
        fclose(f);
        return 0; // Invalid Format
    }

    int version;
    fread(&version, sizeof(int), 1, f);
    if (version != SAVE_VERSION) {
        fclose(f);
        return 0; // Version Mismatch (for now)
    }

    // 2. Global State
    fread(&state->currentFloor, sizeof(int), 1, f);
    fread(&state->questState, sizeof(int), 1, f);
    fread(&state->eventRegistry, sizeof(EventRegistry), 1, f);

    // Global Strings (Quest)
    fread(state->activeQuestMsg, sizeof(char), 1024, f);
    fread(state->pendingQuestMsg, sizeof(char), 1024, f);

    // 3. Player Stats
    Player* p = &state->player;
    fread(&p->x, sizeof(int), 1, f);
    fread(&p->y, sizeof(int), 1, f);
    fread(&p->dirX, sizeof(int), 1, f);
    fread(&p->dirY, sizeof(int), 1, f);
    fread(&p->hp, sizeof(int), 1, f);
    fread(&p->maxHp, sizeof(int), 1, f);
    fread(&p->baseMaxHp, sizeof(int), 1, f);
    fread(&p->level, sizeof(int), 1, f);
    fread(&p->exp, sizeof(int), 1, f);
    fread(&p->expNext, sizeof(int), 1, f);
    fread(&p->baseAttackMin, sizeof(int), 1, f);
    fread(&p->baseAttackMax, sizeof(int), 1, f);

    // 4. Inventory
    inventory_init(&p->inventory);
    int count;
    fread(&count, sizeof(int), 1, f);
    for (int i = 0; i < count; i++) {
        char name[32];
        read_string(f, name, 32);
        const Item* it = item_get(name);
        if (it) inventory_add(&p->inventory, it);
    }

    // 5. Equipment
    char buf[32];
    
    // Weapon
    read_string(f, buf, 32);
    p->equippedWeapon = NULL;
    if (strlen(buf) > 0) p->equippedWeapon = item_get(buf);

    // Armor
    read_string(f, buf, 32);
    p->equippedArmor = NULL;
    if (strlen(buf) > 0) p->equippedArmor = item_get(buf);

    // 6. MAP RECONSTRUCTION
    // A. Tiles
    int mw, mh;
    fread(&mw, sizeof(int), 1, f);
    fread(&mh, sizeof(int), 1, f);
    
    // ★ Initialize Map HERE using the loaded Floor Index
    // This resets enemies/tiles to default from static config
    map_init(&state->map, 1, state->currentFloor);
    state->initializedFloors[state->currentFloor] = 1;

    // Overwrite Tiles (if dimensions match)
    if (mw == state->map.width && mh == state->map.height) {
        for (int i = 0; i < state->map.height; i++) {
            fread(state->map.tiles[i], sizeof(char), state->map.width, f);
        }
    } else {
        // Warning: Map size changed? Skip tile data or error?
        // Assuming consistent version matching.
        // Skip for saftey if implementing robustly, but for now read to advance ptr
         for (int i = 0; i < mh; i++) {
             char temp[1024]; // dummy
             fread(temp, sizeof(char), mw, f);
         }
    }

    // B. Enemies
    int enemyCount;
    fread(&enemyCount, sizeof(int), 1, f);
    for (int i = 0; i < enemyCount; i++) {
        int x, y, hp, isDead;
        float timer;
        fread(&x, sizeof(int), 1, f);
        fread(&y, sizeof(int), 1, f);
        fread(&hp, sizeof(int), 1, f);
        fread(&isDead, sizeof(int), 1, f);
        fread(&timer, sizeof(float), 1, f);

        // Sync with existing array (Index matching)
        if (i < state->map.enemyCount) {
             Enemy* e = &state->map.enemies[i];
             e->x = x;
             e->y = y;
             e->hp = hp;
             e->isDead = isDead;
             e->randomMoveTimer = timer;
             
             // Sync isAlive (If dead, inactive)
             if (e->isDead) e->isAlive = 0;
             else e->isAlive = 1; // Or keep as initialized? Init puts it at 1. Correct.
        }
    }

    // C. Chests
    int chestCount;
    fread(&chestCount, sizeof(int), 1, f);
    for (int i = 0; i < chestCount; i++) {
        int isOpened;
        fread(&isOpened, sizeof(int), 1, f);
        if (i < state->map.chestCount) {
            state->map.chests[i].isOpened = isOpened;
        }
    }

    // D. Doors
    int doorCount;
    fread(&doorCount, sizeof(int), 1, f);
    for (int i = 0; i < doorCount; i++) {
        int isOpen;
        fread(&isOpen, sizeof(int), 1, f);
         if (i < state->map.doorCount) {
            state->map.doors[i].isOpen = isOpen;
            // ★ Sync Tile for Secret Door
            if (isOpen && state->map.doors[i].symbol == '?') {
                state->map.tiles[state->map.doors[i].y][state->map.doors[i].x] = '>';
            }
            if (isOpen) {
                 char msg[64];
                 snprintf(msg, sizeof(msg), "Door %d Open Loaded", i);
                 // ui_add_log(msg); 
            }
        }
    }

    // Recalculate derived stats
    player_update_stats(p);

    fclose(f);
    return 1; 
}
