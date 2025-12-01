#pragma once

#include <atlimage.h>

class Scene
{
	CImage BackGround[4]; // 0: Start 1: Ready 2: Play 3: Result
	CImage GoalPostL, GoalPostR;

	int Timer_M = 1;
	int Timer_S = 0; 

	TCHAR Timer[10];

public:
	Scene();
	~Scene();

	void DrawStartBG(HDC hdc);
	void DrawPlayBG(HDC hdc, int P1Score, int P2Score);
	void DrawReadyBG(HDC hdc);
	void DrawResultBG(HDC hdc, int P1Score, int P2Score);

	void SetMinute(int m);
	void SetSecond(int s);
};