#ifndef MAP_HEADER
#define MAP_HEADER

#include "../entity/player.h"
#include "../entity/enemy.h"  // ★ 추가
#include "../entity/chest.h"
#include "../entity/npc.h"  // ★ 추가

#define MAX_MAP_W 200  // 최대 크기만 정의
#define MAX_MAP_H 120
#define MAX_CHESTS 50

#include "../stages/common.h" // For EventConfig

// ★ 문 관리
typedef struct {
    int x, y;
    int isOpen;
    EventConfig event; // Added
} Door;

#define MAX_DOORS 20

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

	// ★ NPC 추가
	NPC npcs[ MAX_NPCS ];
	int npcCount;

	// ★ 문 관리
	Door doors[ MAX_DOORS ];
	int doorCount;
} Map;

void map_init(Map* m , int stageNumber);
int map_is_walkable(const Map* m, int x, int y);
int map_check_los(Map* m, int x1, int y1, int x2, int y2); // ★ 시야 체크
// const char* tile_to_glyph(char t); // Removed
void map_find_spawn(Map* m);
void map_load_enemies(Map* m);
Enemy* map_get_enemy_at(Map* m , int x , int y);
Enemy* map_get_adjacent_enemy(Map* m , int px , int py);
const char* map_get_enemy_direction(Map* m , int px , int py , Enemy* enemy);  // ★ 추가
const char* enemy_to_glyph(char type);

Chest* map_get_chest_at(Map* m , int x , int y);
Chest* map_get_adjacent_chest(Map* m , int px , int py);

// ★ NPC 관련 함수
void map_load_npcs(Map* m);
NPC* map_get_npc_at(Map* m , int x , int y);
NPC* map_get_adjacent_npc(Map* m , int px , int py);

// ★ 문 관련 함수
void map_load_doors(Map* m);
Door* map_get_door_at(Map* m, int x, int y);


void map_draw_viewport(const Map* m , const Player* p , int startX , int startY ,
	int viewW , int viewH, int effectX, int effectY, float effectTimer);

#endif
