#include "console.h"
#include "buffer.h"

// Console specific functions are disabled for Raylib build

void console_clear_fast(void) {
    // No-op
}

void console_goto(int x, int y) {
    // No-op
}

void ui_init(void) {
    ui_init_buffer();
}
