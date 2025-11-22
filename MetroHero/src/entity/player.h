#ifndef PLAYER_H
#define PLAYER_H

struct Map; // Map 구조체의 전방 선언

typedef struct {
	int x, y;
	const char* weaponName;
	const char* armorName;
	const char* item1;
} Player;

void player_init(Player* p);
void player_move(Player* p, const struct Map* m, int cmd); // struct Map*로 명확히 선언

#endif
