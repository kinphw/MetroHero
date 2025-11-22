#ifndef PLAYER_H
#define PLAYER_H

struct Map; // Map 구조체의 전방 선언

typedef struct {
	int x, y;

	// ★ 스탯
	int hp;           // 현재 체력
	int maxHp;        // 최대 체력
	int attack;       // 공격력
	int defense;      // 방어력

	const char* weaponName;
	const char* armorName;
	const char* item1;
} Player;

void player_init(Player* p);
void player_move(Player* p, const struct Map* m, int cmd); // struct Map*로 명확히 선언

#endif
