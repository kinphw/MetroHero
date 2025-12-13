#pragma once
#ifndef CHEST_H
#define CHEST_H

typedef struct {
	int x , y;
	int isOpened;          // 0 = 닫힘, 1 = 이미 열린 상자
	char tile;             // ★ 맵 상의 타일 문자 ('0'~'9')

	const char* itemType;  // "weapon", "armor", "item"
	const char* itemName;  // 획득할 아이템 이름
} Chest;

void chest_init(Chest* c , int x , int y , char tile, const char* itemType , const char* itemName);

#endif
