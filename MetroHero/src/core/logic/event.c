#define _CRT_SECURE_NO_WARNINGS
#include "event.h"
#include <string.h>
#include <stdio.h>

void event_init_registry(EventRegistry* registry) {
    registry->count = 0;
    memset(registry->flags, 0, sizeof(registry->flags));
}

static int find_flag_index(const EventRegistry* registry, const char* key) {
    if (!key || strlen(key) == 0) return -1;
    
    for (int i = 0; i < registry->count; i++) {
        if (strcmp(registry->flags[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

int event_get_flag(const EventRegistry* registry, const char* key) {
    if (!key || strlen(key) == 0) return 0; // Default 0
    
    int idx = find_flag_index(registry, key);
    if (idx != -1) {
        return registry->flags[idx].value;
    }
    return 0; // Not found = 0
}

void event_set_flag(EventRegistry* registry, const char* key, int value) {
    if (!key || strlen(key) == 0) return;

    int idx = find_flag_index(registry, key);
    if (idx != -1) {
        // Update existing
        registry->flags[idx].value = value;
        // printf("Event Updated: [%s] -> %d\n", key, value);
    } else {
        // Create new
        if (registry->count < MAX_EVENT_FLAGS) {
            strncpy(registry->flags[registry->count].key, key, MAX_FLAG_KEY_LEN - 1);
            registry->flags[registry->count].value = value;
            registry->count++;
            // printf("Event Created: [%s] -> %d\n", key, value);
        } else {
            printf("Event Registry Full! Cannot add [%s]\n", key);
        }
    }
}

void event_add_flag(EventRegistry* registry, const char* key, int amount) {
    int current = event_get_flag(registry, key);
    event_set_flag(registry, key, current + amount);
}
