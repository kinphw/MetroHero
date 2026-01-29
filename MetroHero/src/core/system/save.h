#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include "context.h"

// Save current game state to "save.dat"
void save_game(GameState* state);

// Load game state from "save.dat"
// Returns 1 on success, 0 on failure (file not found or corruption)
int load_game(GameState* state);

// Check if save file exists
int save_file_exists(void);

#endif
