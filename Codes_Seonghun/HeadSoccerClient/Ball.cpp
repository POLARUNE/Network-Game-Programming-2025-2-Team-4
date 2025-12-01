#include "Ball.h"

extern RECT Winsize;

Ball::Ball()
{
	xPos = 500;
	yPos = 300;

	xVel = yVel = 0;

	radius = 15;

	ballSkin.Load(_T("Sprites\\Ball.png"));
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
	ballSkin.TransparentBlt(hdc, (int)xPos, (int)yPos, radius * 2, radius * 2, RGB(255, 0, 0));
}

void Ball::Reset()
{
	xPos = 500;
	yPos = 300;
	xVel = yVel = 0;
	Sleep(1300);
}

Ball::~Ball()
{
	ballSkin.Destroy();
}