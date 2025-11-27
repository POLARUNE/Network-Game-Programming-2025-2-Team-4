#pragma once

#include "CHARACTER.h"
#include <atlimage.h>
#include <Windows.h>


class Poland : public Character
{
	CImage Head[2];
	CImage Body[3];

	CImage Power;

	// ±ê¹ß
	CImage flag;

public:
	Poland(int playerNum);

	virtual void UI_Print(HDC hdc, int playerNum) const override;
	virtual void PowerShoot(HDC hdc, int playerNum, double xpos, double ypos) override;
	virtual void Draw(HDC hdc, int playerNum) const override;
	virtual void Move(int dir, int playerNum) override;
	virtual void Jump(int dir) override;
	~Poland();
};