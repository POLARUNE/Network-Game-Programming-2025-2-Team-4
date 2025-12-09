#include "CHARACTER.h"

Character::Character(int playerNum)
{
	switch (playerNum) {
	case 1:
		xPos = 100;
		break;

	case 2:
		xPos = 875;
		break;
	}
}

RECT Character::CharPos() const
{
	RECT CharPos = { xPos,yPos,xPos + CHAR_SIZE,yPos + CHAR_SIZE + 7 };
	return CharPos;
}

int Character::CharScore() const
{
	return score;
}

void Character::Kick(int playerNum)
{
	kick = playerNum - kick;
}

void Character::Goal()
{
	score++;
}

void Character::ResetPos(int playerNum)
{
	switch (playerNum)
	{
	case 1:
		xPos = 100;
		break;

	case 2:
		xPos = 875;
		break;
	}

	yPos = 630;
}

void Character::Move(int dir, int playerNum)
{
	switch (dir) {
	case 1: // 좌
		xPos -= speed;

		if (xPos <= 75) xPos = 75;
		break;

	case 2: // 우
		xPos += speed;

		if (xPos >= 900) xPos = 900;
		break;
	}
}

void Character::Jump(int dir)
{
	if (dir) yPos -= jump; // 올라가기
	else yPos += jump; // 내려가기
}

Character::~Character()
{

}