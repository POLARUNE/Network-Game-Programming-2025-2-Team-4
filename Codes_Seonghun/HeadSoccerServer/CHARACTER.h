#pragma once

#define CHAR_SIZE	50
#define HEADING		10
#define SHOOT		10
#define POWERSHOOT	20

#include <Windows.h>

class Character {
protected:
	// 좌표
	int xPos = 0;
	int yPos = 630;

	// 능력치
	int jump = 5;
	int speed = 5;
	int power = 5;

	// 점수
	int score = 0;

	// body 변경
	int kick = 0;

	// 플레이어 번호 저장
	int playernum = 0;

public:
	Character(int playerNum);
	~Character();

	RECT CharPos() const;
	int CharScore() const;

	void Move(int dir, int playerNum);
	void Jump(int dir);
	void Kick(int);
	void Goal();
	void ResetPos(int);
};