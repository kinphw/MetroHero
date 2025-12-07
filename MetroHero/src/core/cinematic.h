#ifndef CINEMATIC_H
#define CINEMATIC_H

// ============================================
// 시네마틱 시스템 - 스타워즈 스타일 풀스크린 대화창
// ============================================

// 시네마틱 타입
typedef enum {
    CINEMATIC_INTRO,        // 게임 시작 인트로
    CINEMATIC_STAGE_CLEAR,  // 스테이지 클리어
    CINEMATIC_STAGE_START,  // 스테이지 시작
    CINEMATIC_ENDING_GOOD,  // 좋은 엔딩
    CINEMATIC_ENDING_BAD,   // 나쁜 엔딩 (사망)
    CINEMATIC_CUSTOM        // 커스텀 이벤트
} CinematicType;

// 텍스트 표시 스타일
typedef enum {
    STYLE_NORMAL,           // 일반 텍스트
    STYLE_TITLE,            // 제목 (크게, 중앙 정렬)
    STYLE_SUBTITLE,         // 부제목
    STYLE_SCROLL_UP,        // 아래→위 스크롤 (스타워즈)
    STYLE_TYPEWRITER,       // 타이핑 효과
    STYLE_FADE_IN           // 페이드 인
} TextStyle;

// 시네마틱 라인 구조체
typedef struct {
    const char* text;       // 텍스트 내용
    TextStyle style;        // 표시 스타일
    int delayAfter;         // 이 라인 후 대기시간 (ms), 0이면 키 입력 대기
} CinematicLine;

// 시네마틱 구조체
typedef struct {
    const char* title;              // 상단 제목 (NULL이면 표시 안함)
    const CinematicLine* lines;     // 텍스트 라인 배열
    int lineCount;                  // 라인 수
    int scrollSpeed;                // 스크롤 속도 (ms), 기본값 50
    int showSkipHint;               // "[ESC] 스킵" 표시 여부
    const char* borderColor;        // 테두리 색상 (ANSI 코드)
    const char* textColor;          // 기본 텍스트 색상
} Cinematic;

// ============================================
// 메인 함수
// ============================================

// 시네마틱 재생 (범용)
void cinematic_play(const Cinematic* cine);

// 미리 정의된 시네마틱 재생
void cinematic_play_intro(void);
void cinematic_play_stage_start(int stageNum);
void cinematic_play_stage_clear(int stageNum);
void cinematic_play_ending(int endingType);

// ============================================
// 유틸리티 함수
// ============================================

// 화면 전체 지우고 테두리 그리기
void cinematic_draw_frame(const char* borderColor);

// 중앙 정렬 텍스트 출력
void cinematic_print_centered(int y, const char* text, const char* color);

// 타이핑 효과로 텍스트 출력
void cinematic_print_typewriter(int x, int y, const char* text, int charDelay);

// 스크롤 텍스트 (스타워즈 스타일)
void cinematic_scroll_text(const char** lines, int lineCount, int speed);

// 키 입력 대기 (SPACE: 계속, ESC: 스킵)
// 반환값: 0 = 계속, 1 = 스킵
int cinematic_wait_key(void);

// 페이드 효과 (밝기 조절)
void cinematic_fade_in(int duration);
void cinematic_fade_out(int duration);

#endif
