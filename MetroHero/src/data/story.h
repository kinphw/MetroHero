#ifndef STORY_H
#define STORY_H

#include "../core/cinematic.h"
#include "../world/glyph.h"

// ============================================
// 게임 인트로 시나리오
// ============================================

static const CinematicLine INTRO_LINES[] = {
    { "", STYLE_NORMAL, 500 },
    { "오래 전...", STYLE_TYPEWRITER, 800 },
    { "", STYLE_NORMAL, 300 },
    { "수원 지하철 어딘가에서...", STYLE_TYPEWRITER, 1000 },
    { "", STYLE_NORMAL, 500 },
    { "", STYLE_NORMAL, 300 },
    { "도시의 심장부를 연결하는 지하 세계,", STYLE_TYPEWRITER, 800 },
    { "그곳에는 평범한 시민들만 있는 것이 아니었다.", STYLE_TYPEWRITER, 1000 },
    { "", STYLE_NORMAL, 500 },
    { "어둠 속에서 깨어난 존재들...", STYLE_TYPEWRITER, 800 },
    { "버려진 역사의 미로...", STYLE_TYPEWRITER, 800 },
    { "그리고 단 한 명의 영웅.", STYLE_TYPEWRITER, 1000 },
    { "", STYLE_NORMAL, 800 },
    { "당신은 지하철의 수호자가 되어", STYLE_TYPEWRITER, 600 },
    { "이 도시의 평화를 되찾아야 한다.", STYLE_TYPEWRITER, 0 },
};

static const Cinematic STORY_INTRO = {
    "★  M E T R O   H E R O  ★",
    INTRO_LINES,
    sizeof(INTRO_LINES) / sizeof(INTRO_LINES[0]),
    30,     // 타이핑 속도
    1,      // 스킵 힌트 표시
    COLOR_BRIGHT_CYAN,
    COLOR_WHITE
};

// ============================================
// 스테이지 시작 시나리오
// ============================================

// 스테이지 1: 성균관대역
static const CinematicLine STAGE1_START_LINES[] = {
    { "", STYLE_NORMAL, 300 },
    { "[ STAGE 1 ]", STYLE_TITLE, 500 },
    { "", STYLE_NORMAL, 200 },
    { "성균관대역", STYLE_SUBTITLE, 800 },
    { "", STYLE_NORMAL, 500 },
    { "오래된 역사, 어둠 속에 잠든 터널...", STYLE_TYPEWRITER, 600 },
    { "이곳에서 당신의 여정이 시작된다.", STYLE_TYPEWRITER, 0 },
};

static const Cinematic STORY_STAGE1_START = {
    NULL,
    STAGE1_START_LINES,
    sizeof(STAGE1_START_LINES) / sizeof(STAGE1_START_LINES[0]),
    25,
    1,
    COLOR_YELLOW,
    COLOR_WHITE
};

// 스테이지 2
static const CinematicLine STAGE2_START_LINES[] = {
    { "", STYLE_NORMAL, 300 },
    { "[ STAGE 2 ]", STYLE_TITLE, 500 },
    { "", STYLE_NORMAL, 200 },
    { "화서역 환승 통로", STYLE_SUBTITLE, 800 },
    { "", STYLE_NORMAL, 500 },
    { "미로처럼 얽힌 통로...", STYLE_TYPEWRITER, 600 },
    { "더 강력한 적들이 기다리고 있다.", STYLE_TYPEWRITER, 0 },
};

static const Cinematic STORY_STAGE2_START = {
    NULL,
    STAGE2_START_LINES,
    sizeof(STAGE2_START_LINES) / sizeof(STAGE2_START_LINES[0]),
    25,
    1,
    COLOR_BRIGHT_YELLOW,
    COLOR_WHITE
};

// 스테이지 3
static const CinematicLine STAGE3_START_LINES[] = {
    { "", STYLE_NORMAL, 300 },
    { "[ STAGE 3 ]", STYLE_TITLE, 500 },
    { "", STYLE_NORMAL, 200 },
    { "서울역 지하 심층부", STYLE_SUBTITLE, 800 },
    { "", STYLE_NORMAL, 500 },
    { "모든 것의 시작점...", STYLE_TYPEWRITER, 600 },
    { "최종 결전이 기다린다.", STYLE_TYPEWRITER, 0 },
};

static const Cinematic STORY_STAGE3_START = {
    NULL,
    STAGE3_START_LINES,
    sizeof(STAGE3_START_LINES) / sizeof(STAGE3_START_LINES[0]),
    25,
    1,
    COLOR_BRIGHT_RED,
    COLOR_WHITE
};

// 스테이지 배열
static const Cinematic STORY_STAGES[] = {
    // 인덱스 0 = 스테이지 1
    {
        NULL,
        STAGE1_START_LINES,
        sizeof(STAGE1_START_LINES) / sizeof(STAGE1_START_LINES[0]),
        25, 1, COLOR_YELLOW, COLOR_WHITE
    },
    // 인덱스 1 = 스테이지 2
    {
        NULL,
        STAGE2_START_LINES,
        sizeof(STAGE2_START_LINES) / sizeof(STAGE2_START_LINES[0]),
        25, 1, COLOR_BRIGHT_YELLOW, COLOR_WHITE
    },
    // 인덱스 2 = 스테이지 3
    {
        NULL,
        STAGE3_START_LINES,
        sizeof(STAGE3_START_LINES) / sizeof(STAGE3_START_LINES[0]),
        25, 1, COLOR_BRIGHT_RED, COLOR_WHITE
    }
};

// ============================================
// 스테이지 클리어 시나리오
// ============================================

static const CinematicLine STAGE1_CLEAR_LINES[] = {
    { "", STYLE_NORMAL, 300 },
    { "★ STAGE CLEAR ★", STYLE_TITLE, 800 },
    { "", STYLE_NORMAL, 500 },
    { "성균관대역을 정화했다!", STYLE_TYPEWRITER, 600 },
    { "", STYLE_NORMAL, 300 },
    { "하지만 어둠은 더 깊은 곳에서 기다리고 있다...", STYLE_TYPEWRITER, 0 },
};

static const CinematicLine STAGE2_CLEAR_LINES[] = {
    { "", STYLE_NORMAL, 300 },
    { "★ STAGE CLEAR ★", STYLE_TITLE, 800 },
    { "", STYLE_NORMAL, 500 },
    { "환승 통로를 돌파했다!", STYLE_TYPEWRITER, 600 },
    { "", STYLE_NORMAL, 300 },
    { "최종 목적지가 눈앞에 있다...", STYLE_TYPEWRITER, 0 },
};

static const CinematicLine STAGE3_CLEAR_LINES[] = {
    { "", STYLE_NORMAL, 300 },
    { "★ STAGE CLEAR ★", STYLE_TITLE, 800 },
    { "", STYLE_NORMAL, 500 },
    { "모든 어둠을 물리쳤다!", STYLE_TYPEWRITER, 600 },
    { "", STYLE_NORMAL, 300 },
    { "서울 지하철에 다시 평화가 찾아왔다.", STYLE_TYPEWRITER, 0 },
};

static const Cinematic STORY_STAGE_CLEAR[] = {
    {
        NULL,
        STAGE1_CLEAR_LINES,
        sizeof(STAGE1_CLEAR_LINES) / sizeof(STAGE1_CLEAR_LINES[0]),
        30, 1, COLOR_BRIGHT_GREEN, COLOR_WHITE
    },
    {
        NULL,
        STAGE2_CLEAR_LINES,
        sizeof(STAGE2_CLEAR_LINES) / sizeof(STAGE2_CLEAR_LINES[0]),
        30, 1, COLOR_BRIGHT_GREEN, COLOR_WHITE
    },
    {
        NULL,
        STAGE3_CLEAR_LINES,
        sizeof(STAGE3_CLEAR_LINES) / sizeof(STAGE3_CLEAR_LINES[0]),
        30, 1, COLOR_BRIGHT_YELLOW, COLOR_WHITE
    }
};

// ============================================
// 엔딩 시나리오
// ============================================

// 좋은 엔딩
static const CinematicLine ENDING_GOOD_LINES[] = {
    { "", STYLE_NORMAL, 500 },
    { "당신은 해냈다.", STYLE_TYPEWRITER, 1000 },
    { "", STYLE_NORMAL, 500 },
    { "지하철의 어둠은 물러가고,", STYLE_TYPEWRITER, 800 },
    { "시민들은 다시 안전하게 이동할 수 있게 되었다.", STYLE_TYPEWRITER, 1000 },
    { "", STYLE_NORMAL, 500 },
    { "하지만 영웅의 이야기는", STYLE_TYPEWRITER, 600 },
    { "누구에게도 알려지지 않았다...", STYLE_TYPEWRITER, 1000 },
    { "", STYLE_NORMAL, 800 },
    { "그것이 진정한 영웅의 길.", STYLE_TYPEWRITER, 0 },
};

static const Cinematic STORY_ENDING_GOOD = {
    "★  T H E   E N D  ★",
    ENDING_GOOD_LINES,
    sizeof(ENDING_GOOD_LINES) / sizeof(ENDING_GOOD_LINES[0]),
    35,
    1,
    COLOR_BRIGHT_YELLOW,
    COLOR_WHITE
};

// 나쁜 엔딩 (사망)
static const CinematicLine ENDING_BAD_LINES[] = {
    { "", STYLE_NORMAL, 500 },
    { "어둠이 당신을 삼켰다...", STYLE_TYPEWRITER, 1000 },
    { "", STYLE_NORMAL, 500 },
    { "지하철의 수호자는 쓰러졌고,", STYLE_TYPEWRITER, 800 },
    { "도시는 영원한 어둠 속에 남겨졌다.", STYLE_TYPEWRITER, 1000 },
    { "", STYLE_NORMAL, 800 },
    { "하지만 희망은 사라지지 않는다.", STYLE_TYPEWRITER, 600 },
    { "언젠가 새로운 영웅이 나타날 것이다...", STYLE_TYPEWRITER, 0 },
};

static const Cinematic STORY_ENDING_BAD = {
    "G A M E   O V E R",
    ENDING_BAD_LINES,
    sizeof(ENDING_BAD_LINES) / sizeof(ENDING_BAD_LINES[0]),
    40,
    1,
    COLOR_BRIGHT_RED,
    COLOR_GRAY
};

#endif
