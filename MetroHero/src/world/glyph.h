#ifndef GLYPH_H
#define GLYPH_H

// ========================
// 렌더링 문자 상수 정의
// ========================

// 맵 타일
#define GLYPH_FLOOR    "  "    // 바닥
#define GLYPH_WALL     "██"    // 벽
#define GLYPH_RAIL     "=="    // 철도 레일
#define GLYPH_EMPTY    "  "    // 빈 공간
#define GLYPH_DOOR     "++"    // 문
#define GLYPH_STAIRS   "▼▼"    // 계단

// 엔티티
#define GLYPH_PLAYER   "옷"    // 플레이어
#define GLYPH_ENEMY    "적"    // 적
#define GLYPH_NPC      "민"    // NPC
#define GLYPH_ITEM     "물"    // 아이템

// 기타
#define GLYPH_TREE     "♣♣"    // 나무
#define GLYPH_WATER    "≈≈"    // 물

// ========================
// 타일 문자 (맵 데이터용)
// ========================
#define TILE_FLOOR     '.'
#define TILE_WALL      '#'
#define TILE_RAIL      '='
#define TILE_DOOR      '+'
#define TILE_SPAWN     '@'     // ★ 스폰 포인트

#endif