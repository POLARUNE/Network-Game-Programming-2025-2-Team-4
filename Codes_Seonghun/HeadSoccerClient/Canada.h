#pragma once

#include "Character.h"

class Canada : public Character
{
	CImage imgHead[2]; // 왼쪽, 오른쪽
	CImage imgBody[3]; // 기본, 왼발, 오른발

	// CImage imgPower;

	// 국기
	CImage imgFlag;

public:
	Canada(int playerNum);
	~Canada();

	virtual void UI_Print(HDC hdc, int playerNum) const override;
	virtual void Draw(HDC hdc, int playerNum) const override;

	// virtual void PowerShoot(HDC hdc, int playerNum, double xpos, double ypos) override;
};