#ifndef PLAYER_H
#define PLAYER_H

struct Map;

typedef struct {
	int x , y;

	int hp;
	int maxHp;
	int attack;
	int defense;

	const char* weaponName;
	const char* armorName;
	const char* item1;
} Player;

void player_init(Player* p);
void player_move(Player* p , struct Map* m , int cmd);  // ★ const 제거

#endif