// #include <windows.h> // Removed for Raylib
#include "core/game.h"
#include "core/ui/ui.h" // For ui_init/close
#include "core/system/launcher.h"
#include "core/audio/audio.h" // Added
#include "debug/debug.h"

int main(void) {
    // debug_console_info();

    // 1. Raylib/UI 초기화 (프로그램 전체 수명주기 동안 유지)
    ui_init();
    audio_init(); // Added

    // 2. 메인 메뉴 루프
    while (1) {
        GameMode mode = game_menu();
        
        if (mode == GAME_NEW) {
            game_run_new_session();
        } 
        else if (mode == GAME_LOAD) {
            // TODO: Implement load logic
            // game_run_load_session();
        }
        else {
            // GAME_EXIT
            break;
        }
    }

    audio_close(); // Added
    ui_close();
    return 0;
}
