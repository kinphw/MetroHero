#ifndef MAP_HEADER
#define MAP_HEADER

#include "../entity/player.h"
#include "../entity/enemy.h"  // ★ 추가
#include "../entity/chest.h"

#define MAX_MAP_W 100  // 최대 크기만 정의
#define MAX_MAP_H 50

//#define MAP_W 40
//#define MAP_H 20

//typedef struct {
//    char tiles[MAP_H][MAP_W];
//} Map;

#define MAX_CHESTS 50

typedef struct {
	int width;
	int height;
	int stageNumber;  // ★ 현재 스테이지 번호 추가 (선택사항)
	int spawnX;  // ★ 스폰 위치 추가
	int spawnY;  // ★ 스폰 위치 추가
	char tiles[ MAX_MAP_H ][ MAX_MAP_W ];

	// ★ 적 관리
	Enemy enemies[ MAX_ENEMIES ];
	int enemyCount;

	Chest chests[ MAX_CHESTS ];
	int chestCount;
} Map;

void map_init(Map* m , int stageNumber);  // ★ 인자 추가
void map_draw_at(const Map* m , const Player* p , int startX , int startY);
int map_is_walkable(const Map* m, int x, int y);
const char* tile_to_glyph(char t);  // ★ 추가
void map_find_spawn(Map* m);  // ★ 추가
void map_load_enemies(Map* m);  // ★ 추가
Enemy* map_get_enemy_at(Map* m , int x , int y);  // ★ 추가
Enemy* map_get_adjacent_enemy(Map* m , int px , int py);  // ★ 추가
const char* map_get_enemy_direction(Map* m , int px , int py , Enemy* enemy);  // ★ 추가
const char* enemy_to_glyph(char type);

Chest* map_get_chest_at(Map* m , int x , int y);
Chest* map_get_adjacent_chest(Map* m , int px , int py);

#endif
