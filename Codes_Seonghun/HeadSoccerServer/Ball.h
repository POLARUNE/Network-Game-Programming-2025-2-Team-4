#pragma once

#define PI 3.141519

#define GRAVITY 0.5
#define FRICTION 0.8

#include "Character.h"
// #include <mmsystem.h>

class Ball {
	// 좌표
	double xPos;
	double yPos;

	// 속도
	double xVel;
	double yVel;

	// 반지름
	int radius;
public:
	Ball();
	~Ball();

	double BallxPos() const;
	double BallyPos() const;

	void Reset();
	void Physics(Character* p1, Character* p2);
};