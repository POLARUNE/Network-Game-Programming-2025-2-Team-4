#include "CHARACTER.h"

extern BOOL CrashCheck;

Character::Character()
{
	xPos = 0;
	yPos = 0;
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

Character::~Character()
{

}