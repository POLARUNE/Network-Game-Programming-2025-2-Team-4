#pragma once

#include <atlimage.h>

class Scene
{
	CImage BackGround;
	CImage GoalPostR, GoalPostL;

	RECT P1Rect, P2Rect;
	int P1Num, P2Num;

	BOOL Kick1, Kick2;
	BOOL Goal1, Goal2;

	int SceneNum = 0;

	CImage Char[2][10];
	CImage CharP1;
	CImage CharP2;

	CImage ResBG;

public:
	Scene();
	~Scene();

	void DrawBG(HDC hdc);
	void DrawSelectBG(HDC hdc, int P1Score, int P2Score);
	void ResultBG(HDC);
	void DeleteSelBG();
	void DeleteResBG();
};