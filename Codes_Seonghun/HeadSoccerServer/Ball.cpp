#include "Ball.h"

extern RECT WinSize;

Ball::Ball()
{
	xPos = 500;
	yPos = 300;

	xVel = yVel = 0;

	radius = 15;
}

double Ball::BallxPos() const
{
	return xPos;
}

double Ball::BallyPos() const
{
	return yPos;
}

void Ball::Reset()
{
	xPos = 500;
	yPos = 300;
	xVel = yVel = 0;
	Sleep(1300);
}

void Ball::Physics(Character* p1, Character* p2)
{
	yVel += GRAVITY;
	yPos += yVel;
	xPos += xVel;

	if (yPos + (radius * 2) >= 678)
	{
		yPos = 678 - (radius * 2);
		yVel = -(yVel * FRICTION);
	}

	if (yPos + (radius * 2) >= 570 && yPos + (radius * 2) <= 600)
	{
		if (xPos <= 92 || xPos + (radius * 2) >= 930)
		{
			yVel = -(yVel * FRICTION);
		}
	}

	if (yPos <= WinSize.top + 170)
	{
		yVel = 0;
	}

	RECT p1ext, p2ext;

	p1ext = p1->CharPos();
	p2ext = p2->CharPos();
	POINT center = { xPos + radius , yPos + radius };

	if ((yPos + (radius * 2)) >= p1ext.top && (yPos + (radius * 2)) <= p1ext.top + 20)
	{
		if ((((xPos >= p1ext.left) && (xPos <= p1ext.right))) || (((xPos + (radius * 2) >= p1ext.left) && (xPos + (radius * 2) <= p1ext.right))))
		{
			/*if (P1Power) {
				yVel -= GRAVITY;
				xVel = POWERSHOOT;
				P1Crash = TRUE;
			}
			else {
				yVel = -(yVel + HEADING);
			}*/

			yVel = -(yVel + HEADING);
		}
	}
	else if ((yPos + (radius * 2)) >= p2ext.top && (yPos + (radius * 2)) <= p2ext.top + 20)
	{
		if ((((xPos >= p2ext.left) && (xPos <= p2ext.right))) || (((xPos + (radius * 2) >= p2ext.left) && (xPos + (radius * 2) <= p2ext.right))))
		{
			/*if (P2Power) {
				yVel -= GRAVITY;
				xVel = -POWERSHOOT;
				P2Crash = TRUE;
			}

			else {
				yVel = -(yVel + HEADING);
			}*/

			yVel = -(yVel + HEADING);
		}
	}

	if (yPos + (radius * 2) >= p1ext.top + 20)
	{
		if (xPos <= p1ext.right && xPos >= p1ext.left)
		{
			/*if (Kick1 && !P1Power)
			{
				yVel = -(yVel + HEADING);

			}*/

			/*if (P1Power) {
				yVel -= GRAVITY;
				xVel = POWERSHOOT;
				P1Crash = TRUE;
			}
			else {
				xVel = SHOOT;
			}*/

			xVel = SHOOT;

		}
		else if (xPos + (radius * 2) >= p1ext.left && xPos + (radius * 2) <= p1ext.right)
		{
			xVel = -SHOOT;
		}
	}

	if (yPos + (radius * 2) >= p2ext.top + 20)
	{
		if (xPos <= p2ext.right && xPos >= p2ext.left)
		{
			xVel = SHOOT;
		}
		else if (xPos + (radius * 2) >= p2ext.left && xPos + (radius * 2) <= p2ext.right)
		{
			/*if (Kick2 && !P2Power)
			{
				yVel = -(yVel + HEADING);

			}*/

			/*if (P2Power) {
				yVel -= GRAVITY;
				xVel = -POWERSHOOT;
				P2Crash = TRUE;
			}
			else {
				xVel = -SHOOT;
			}*/

			xVel = -SHOOT;
		}
	}

	if (yPos <= 570)
	{
		if (xPos <= WinSize.left || xPos >= WinSize.right)
		{
			xVel = -(xVel * FRICTION);
		}
	}
	else if (yPos >= 600)
	{
		if (xPos <= 92)
		{
			p2->Goal();
		}
		else if (xPos >= 930)
		{
			p1->Goal();
		}
	}
}

Ball::~Ball()
{	
}