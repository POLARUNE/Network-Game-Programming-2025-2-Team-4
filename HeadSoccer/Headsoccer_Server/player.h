#pragma once

#define MAX_PLAYER 3

#pragma pack(push, 1)
struct Player {
	char inputFlag;
	int x, y;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct BallPacket {
	float x, y;
};
#pragma pack(pop)