#ifndef EVENT_H
#define EVENT_H

#define MAX_EVENT_FLAGS 128
#define MAX_FLAG_KEY_LEN 32

typedef struct {
    char key[MAX_FLAG_KEY_LEN];
    int value;
} EventFlag;

typedef struct {
    EventFlag flags[MAX_EVENT_FLAGS];
    int count;
} EventRegistry;

struct GameState; // Forward decl

// Core API
int event_get_flag(const EventRegistry* registry, const char* key);
void event_set_flag(EventRegistry* registry, const char* key, int value);
void event_add_flag(EventRegistry* registry, const char* key, int amount);

// Helper for GameState
void event_init_registry(EventRegistry* registry);

#endif
