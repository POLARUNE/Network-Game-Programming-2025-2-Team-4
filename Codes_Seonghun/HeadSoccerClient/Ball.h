#pragma once

#define PI  3.141519

#define GRAVITY 0.5
#define FRICTION 0.8

#include "Character.h"
#include <mmsystem.h>

class Ball {
	// 좌표
	double xPos;
	double yPos;

	// 속도
	double xVel;
	double yVel;

	// 반지름
	int radius;

	// 공 이미지
	CImage ballSkin;
public:
	Ball();
	~Ball();

	double BallxPos() const;
	double BallyPos() const;

	void Draw(HDC hdc);
	// void Physics(Character* p1, Character* p2);
	void Reset();
};