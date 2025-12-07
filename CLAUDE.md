# MetroHero

Windows 콘솔 기반 로그라이크 RPG 게임. 수원 지하철을 배경으로 한 턴제 던전 탐험 게임.

## 빌드

- **IDE**: Visual Studio (Windows)
- **프로젝트 파일**: `MetroHero/MetroHero.vcxproj`
- **언어**: C (C99)
- **플랫폼**: Windows 전용 (`<windows.h>`, `<conio.h>` 사용)

## 프로젝트 구조

```
MetroHero/src/
├── main.c              # 진입점
├── core/
│   ├── game.c/h        # 메인 게임 루프
│   ├── combat.c/h      # 전투 시스템
│   ├── ui.c/h          # UI 렌더링 (상태창, 장비창, 로그창)
│   └── cinematic.c/h   # 시네마틱/컷씬 시스템
├── entity/
│   ├── player.c/h      # 플레이어 (이동, 스탯)
│   ├── enemy.c/h       # 적 (타입별 스탯, 대사)
│   ├── npc.c/h         # NPC (대화, 거래)
│   └── chest.c/h       # 상자 (아이템)
├── world/
│   ├── map.c/h         # 맵 로딩, 뷰포트 렌더링
│   ├── map_data.c/h    # 스테이지별 맵 데이터
│   └── glyph.h         # 타일/색상 정의
├── data/
│   └── story.h         # 시네마틱 대사 데이터
└── debug/
    └── debug.c/h       # 디버깅 유틸리티
```

## 화면 레이아웃

```
120 x 30 콘솔
┌──────────────────────────────────┬────────────────┐
│                                  │ 상태창 (STATUS)│
│        뷰포트 (VIEWPORT)          │ HP, ATK, DEF   │
│        40칸 x 20줄               ├────────────────┤
│                                  │ 장비창 (EQUIP) │
│                                  │ 무기, 방어구    │
├──────────────────────────────────┴────────────────┤
│                  로그창 (LOG)                      │
│                  9줄                               │
└───────────────────────────────────────────────────┘
```

## 타일 규칙 (glyph.h)

| 문자 | 의미 |
|------|------|
| `.`  | 바닥 (FLOOR) |
| `#`  | 벽 (WALL) |
| `=`  | 철로 (RAIL) |
| `+`  | 문 (DOOR) |
| `@`  | 스폰 포인트 |
| `a-z`| 몹 (소문자) - a: 고양이, b: 로봇 등 |
| `A-Z`| NPC (대문자) - A: 상인, B: 경비원, C: 시민 |
| `0-9`| 상자 (CHEST) |

## 색상 시스템

ANSI 이스케이프 코드 사용 (`glyph.h` 참조):
- `COLOR_*`: 전경색 (RED, GREEN, YELLOW, CYAN 등)
- `COLOR_BRIGHT_*`: 밝은 전경색
- `BG_*`: 배경색
- `COLOR_RESET`: 색상 초기화

## 조작키

- `WASD` / 화살표: 이동
- `0`: 상호작용 (NPC 대화, 상자 열기)
- `Q`: 게임 종료
- `ESC`: 대화 종료

## 주요 시스템

### 전투
- 적에게 이동하면 자동 공격
- 공격력 범위: `attackMin` ~ `attackMax`
- 방어력으로 데미지 감소

### 대화
- `useDialogueBox`: 1이면 전용 대화창, 0이면 로그창
- `dialogues[]`: 대사 배열, `currentDialogue`로 진행

### 시네마틱
- 인트로, 스테이지 시작/클리어, 엔딩
- 타이핑 효과, 페이드, 스크롤 지원

## 코딩 컨벤션

- 함수명: `모듈_동작` (예: `player_move`, `map_init`, `ui_draw_stats`)
- 구조체: PascalCase (예: `Player`, `Enemy`, `Map`)
- 상수: UPPER_SNAKE_CASE (예: `MAX_ENEMIES`, `VIEWPORT_W`)
- 주석: 한글 사용, `★` 마커로 중요 변경사항 표시

## 주의사항

- Windows 전용 API 사용 (`_getch()`, `SetConsoleCursorPosition` 등)
- 한글 출력을 위해 전각 문자(2칸) 고려 필요
- `_CRT_SECURE_NO_WARNINGS` 정의됨 (Visual Studio 경고 비활성화)
