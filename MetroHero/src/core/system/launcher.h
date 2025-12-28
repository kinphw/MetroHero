#ifndef LAUNCHER_H
#define LAUNCHER_H

typedef enum {
    GAME_EXIT = 0,
    GAME_NEW = 1,
    GAME_LOAD = 2
} GameMode;

// 메인 메뉴 루프 실행. 
// 사용자의 선택(새 게임, 이어하기, 종료)을 반환.
GameMode game_menu(void);

#endif
