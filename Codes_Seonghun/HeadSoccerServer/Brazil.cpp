#include "Brazil.h"

extern RECT P1Rect;
extern RECT P2Rect;
extern BOOL CrashCheck;

Brazil::Brazil(int playerNum) :Character()
{
	switch (playerNum) {
	case 1:
		xPos = 100;
		break;

	case 2:
		xPos = 875;
		break;
	}

	yPos = 630;

	jump = 5;
	speed = 5;
	power = 5;
}

void Brazil::Move(int dir, int playerNum)
{
	switch (dir) {
	case 1: // 좌
		xPos -= speed;

		if (xPos <= 75) {
			xPos = 75;
		}

		if (CrashCheck) {
			switch (playerNum)
			{
			case 1:
				xPos = P2Rect.left - CHAR_SIZE - 1;
				break;

			case 2:
				xPos = P1Rect.right + 1;
				break;
			}
			CrashCheck = FALSE;
		}

		break;

	case 2: // 우
		xPos += speed;

		if (xPos >= 900) {
			xPos = 900;
		}

		if (CrashCheck) {
			switch (playerNum)
			{
			case 1:
				xPos = P2Rect.left - CHAR_SIZE - 1;
				break;

			case 2:
				xPos = P1Rect.right + 1;
				break;
			}
			CrashCheck = FALSE;
		}

		break;
	}
}

void Brazil::Jump(int dir)
{
	if (dir) {
		yPos -= jump;
	}

	else {
		yPos += jump;
	}
}

Brazil::~Brazil()
{

}