#include "CHARACTER.h"

extern RECT WinSize;
extern BOOL CrashCheck;

Character::Character()
{
	xPos = 0;
	yPos = 0;
	PwGauge = 0;

	PowerGaugeL.Load(_T("sprite\\PowerGauge_L.png"));
	PowerGaugeFrameL.Load(_T("sprite\\PowerGaugeFrame_L.png"));

	PowerGaugeR.Load(_T("sprite\\PowerGauge_R.png"));
	PowerGaugeFrameR.Load(_T("sprite\\PowerGaugeFrame_R.png"));
}

RECT Character::CharPos() const
{
	RECT CharPos = { xPos, yPos, xPos + CHAR_SIZE, yPos + CHAR_SIZE + 7 };
	return CharPos;
}

int Character::CharScore() const
{
	return score;
}

void Character::PwGaugeFull()
{
	PwGauge += power;

	if (PwGauge >= 100) {
		PwGauge = 100;
	}
}

void Character::Kick(int playerNum)
{
	kick = playerNum - kick;
}

void Character::Goal()
{
	score++;
}

void Character::Goaled()
{
	PwGauge += 20;
	if (PwGauge >= 100) {
		PwGauge = 100;
	}
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

int Character::PowerGauge() const
{
	return PwGauge;
}

Character::~Character()
{
	PowerGaugeL.Destroy();
	PowerGaugeR.Destroy();
	PowerGaugeFrameL.Destroy();
	PowerGaugeFrameR.Destroy();
}