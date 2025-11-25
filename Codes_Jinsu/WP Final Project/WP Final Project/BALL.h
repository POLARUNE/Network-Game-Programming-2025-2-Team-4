#pragma once

#define PI      3.141519

#define GRAVITY 0.5
#define FRICTION 0.8

#include <mmsystem.h>
#include "CHARACTER.h"

class Ball {
	// 좌표
	double xPos;
	double yPos;

	// 속도
	double xVel;
	double yVel;

	// 반지름
	int radius;

	// 공 스킨
	CImage ballSkin[3];

	// 랜덤 값
	int SkinRnd;
public:
	Ball();

	double BallxPos() const;
	double BallyPos() const;

	void Draw(HDC hdc);
	void Physics(Character* p1, Character* p2);
	void Reset();
	~Ball();
};