#ifndef MAP_HEADER
#define MAP_HEADER

#include "../entity/player.h"

#define MAX_MAP_W 100  // 최대 크기만 정의
#define MAX_MAP_H 50

//#define MAP_W 40
//#define MAP_H 20

//typedef struct {
//    char tiles[MAP_H][MAP_W];
//} Map;

typedef struct {
	int width;
	int height;
	int stageNumber;  // ★ 현재 스테이지 번호 추가 (선택사항)
	char tiles[ MAX_MAP_H ][ MAX_MAP_W ];
} Map;

void map_init(Map* m , int stageNumber);  // ★ 인자 추가
void map_draw_at(const Map* m , const Player* p , int startX , int startY);
int map_is_walkable(const Map* m, int x, int y);
const char* tile_to_glyph(char t);  // ★ 추가

#endif
