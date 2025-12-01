#pragma once

#include <windows.h>
#include <atlimage.h>

#define CHAR_SIZE	50
#define HEADING		10
#define SHOOT		10
#define POWERSHOOT	20


class Character {
protected:
	// 캐릭터 위치 좌표
	int xPos = 0;
	int yPos = 0;

	// 파워게이지
	// int PwGauge = 0;

	// 캐릭터 능력치
	int jump = 0;  // 5 ~ 6
	int speed = 0; // 5 ~ 7
	int power = 0; // 3 ~ 5

	// 점수
	int score = 0;

public:
	Character();

	RECT CharPos() const;
	int CharScore() const;

	//void PwGaugeFull();
	//int PowerGauge() const;

	virtual void UI_Print(HDC hdc, int playerNum) const = 0;
	virtual void Draw(HDC hdc, int playerNum) const = 0;


	// virtual void PowerShoot(HDC hdc, int playerNum, double xpos, double ypos) = 0;

	void Goal();
	
	//void Goaled();
	//void Kick(int);

	void ResetPos(int);
	virtual ~Character();
};