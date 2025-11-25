#include <Windows.h>
#include <atlimage.h>
#include <random>
#include <time.h>
#include "BALL.h"

extern RECT WinSize;
extern BOOL Kick1, Kick2;
extern BOOL Goal1, Goal2;

extern BOOL P1Power, P2Power;
extern BOOL P1Crash, P2Crash;

Ball::Ball()
{
	xPos = 500;
	yPos = 300;

	xVel = yVel = 0;

	radius = 15;

	ballSkin[0].Load(_T("sprite\\Normal_Ball.png"));
	ballSkin[1].Load(_T("sprite\\Pink_Ball.png"));
	ballSkin[2].Load(_T("sprite\\Golden_Ball.png"));

	srand((unsigned int)time(NULL));

	SkinRnd = rand() % 3;
}

double Ball::BallxPos() const
{
	return xPos;
}

double Ball::BallyPos() const
{
	return yPos;
}

void Ball::Draw(HDC hdc)
{
	ballSkin[SkinRnd].TransparentBlt(hdc, (int)xPos, (int)yPos, radius * 2, radius * 2, RGB(255, 0, 0));
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
	if ((yPos + (radius * 2)) >= p1ext.top && (yPos + (radius * 2)) <= p1ext.top + 20 )
	{
		if ((((xPos >= p1ext.left) && (xPos <= p1ext.right))) || (((xPos + (radius * 2) >= p1ext.left) && (xPos + (radius * 2) <= p1ext.right))))
		{
			if (P1Power) {
				yVel -= GRAVITY;
				xVel = POWERSHOOT;
				P1Crash = TRUE;
			}

			else {
				yVel = -(yVel + HEADING);
			}
		}
	}
	
	else if ((yPos + (radius * 2)) >= p2ext.top && (yPos + (radius * 2)) <= p2ext.top + 20)
	{
		if ((((xPos >= p2ext.left) && (xPos <= p2ext.right))) || (((xPos + (radius * 2) >= p2ext.left) && (xPos + (radius * 2) <= p2ext.right))))
		{
			if (P2Power) {
				yVel -= GRAVITY;
				xVel = -POWERSHOOT;
				P2Crash = TRUE;
			}

			else {
				yVel = -(yVel + HEADING);
			}
		}
	}

	if (yPos + (radius * 2) >= p1ext.top + 20)
	{
		if (xPos <= p1ext.right && xPos >= p1ext.left)
		{
			if (Kick1 && !P1Power)
			{
				yVel = -(yVel + HEADING);
				
			}

			if (P1Power) {
				yVel -= GRAVITY;
				xVel = POWERSHOOT;
				P1Crash = TRUE;
			}

			else {
				xVel = SHOOT;
			}

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
			if (Kick2 && !P2Power)
			{
				yVel = -(yVel + HEADING);

			}

			if (P2Power) {
				yVel -= GRAVITY;
				xVel = -POWERSHOOT;
				P2Crash = TRUE;
			}

			else {
				xVel = -SHOOT;
			}
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
			PlaySound(L"sound\\loud.wav", NULL, SND_ASYNC);
			Goal1 = TRUE;
		}
		else if (xPos >= 930)
		{
			PlaySound(L"sound\\loud.wav", NULL, SND_ASYNC);
			Goal2 = TRUE;
		}
	}
}

Ball::~Ball()
{
	ballSkin[0].Destroy();
	ballSkin[1].Destroy();
	ballSkin[2].Destroy();
}