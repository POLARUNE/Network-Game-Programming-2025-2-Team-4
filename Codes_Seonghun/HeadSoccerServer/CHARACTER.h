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
	int yPos = 0;

	// 능력치
	int jump = 0;
	int speed = 0;
	int power = 0;

	// 점수
	int score = 0;

	// body 변경
	int kick = 0;
public:
	Character();

	RECT CharPos() const;
	int CharScore() const;

	virtual void Move(int dir, int playerNum) = 0;
	virtual void Jump(int dir) = 0;
	void Kick(int);
	void Goal();
	void ResetPos(int);
	virtual ~Character();
};