#include "Background.h"

extern RECT WinSize;

Scene::Scene()
{
	BackGround[0].Load(_T("Sprites\\BG_Start.png"));
	BackGround[1].Load(_T("Sprites\\BG_Ready.png"));
	BackGround[2].Load(_T("Sprites\\BG_Play.png"));
	BackGround[3].Load(_T("Sprites\\BG_Result.png"));
	GoalPostL.Load(_T("Sprites\\GoalPost_L.png"));
	GoalPostR.Load(_T("Sprites\\GoalPost_R.png"));
}

void Scene::DrawStartBG(HDC hdc)
{
	BackGround[0].BitBlt(hdc, 0, 0, 1000, 740, 0, 0, SRCCOPY);
}

void Scene::DrawReadyBG(HDC hdc)
{
	BackGround[1].StretchBlt(hdc, WinSize, SRCCOPY);
}

void Scene::DrawPlayBG(HDC hdc, int P1Score, int P2Score)
{
	SIZE size;

	BackGround[2].BitBlt(hdc, 0, 0, 1000, 740, 0, 0, SRCCOPY);
	GoalPostR.TransparentBlt(hdc, 930, 570, 62, 109, 0, 0, 62, 109, RGB(255, 255, 255));
	GoalPostL.TransparentBlt(hdc, 30, 570, 62, 109, 0, 0, 62, 109, RGB(255, 255, 255));

	HFONT hFont, oldFont;

	hFont = CreateFont(50, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("굴림"));
	oldFont = (HFONT)SelectObject(hdc, hFont);

	GetTextExtentPoint32(hdc, L"TIME", lstrlen(L"TIME"), &size);

	HBRUSH hBrush, oldBrush;

	hBrush = CreateSolidBrush(RGB(0, 0, 0));
	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	RoundRect(hdc, WinSize.right / 2 - 100, 0, WinSize.right / 2 - 0 + size.cx, size.cy, 5, 5);

	SelectObject(hdc, oldBrush);
	DeleteObject(hBrush);

	SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(255, 255, 255));

	TextOut(hdc, WinSize.right / 2 - 50, 0, L"TIME", lstrlen(L"TIME"));

	SelectObject(hdc, oldFont);
	DeleteObject(hFont);

	hFont = CreateFont(40, 0, 0, 0, FW_ULTRABOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("밝은고딕"));
	oldFont = (HFONT)SelectObject(hdc, hFont);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 70, 200));

	wsprintf(Timer, L"%d : %02d", Timer_M, Timer_S);
	TextOut(hdc, WinSize.right / 2 - 40, size.cy + 25, Timer, lstrlen(Timer));

	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
}

void Scene::DrawResultBG(HDC hdc, int P1Score, int P2Score)
{
	BackGround[3].StretchBlt(hdc, WinSize, SRCCOPY);

	TCHAR result[2][10];

	HFONT hFont, oldFont;
	SIZE size;

	hFont = CreateFont(100, 0, 0, 0, FW_ULTRABOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("굴림"));
	oldFont = (HFONT)SelectObject(hdc, hFont);

	SetBkColor(hdc, RGB(0, 117, 175));

	if (P1Score > P2Score) {
		SetTextColor(hdc, RGB(0, 255, 0));
		wsprintf(result[0], L"WIN");
		TextOut(hdc, 50, 50, result[0], lstrlen(result[0]));

		SetTextColor(hdc, RGB(110, 0, 130));
		wsprintf(result[1], L"LOSE");
		GetTextExtentPoint32(hdc, result[1], lstrlen(result[1]), &size);
		TextOut(hdc, WinSize.right - 50 - size.cx, 50, result[1], lstrlen(result[1]));
	}

	else if (P1Score == P2Score) {
		SetTextColor(hdc, RGB(255, 255, 0));
		wsprintf(result[0], L"DRAW");
		TextOut(hdc, 50, 50, result[0], lstrlen(result[0]));

		wsprintf(result[1], L"DRAW");
		GetTextExtentPoint32(hdc, result[1], lstrlen(result[1]), &size);
		TextOut(hdc, WinSize.right - 50 - size.cx, 50, result[1], lstrlen(result[1]));
	}

	else {
		SetTextColor(hdc, RGB(110, 0, 130));
		wsprintf(result[0], L"LOSE");
		TextOut(hdc, 50, 50, result[0], lstrlen(result[0]));

		SetTextColor(hdc, RGB(0, 255, 0));
		wsprintf(result[1], L"WIN");
		GetTextExtentPoint32(hdc, result[1], lstrlen(result[1]), &size);
		TextOut(hdc, WinSize.right - 50 - size.cx, 50, result[1], lstrlen(result[1]));
	}

	TCHAR SCORE[2][10];

	wsprintf(SCORE[0], L"%d", P1Score);
	wsprintf(SCORE[1], L"%d", P2Score);

	GetTextExtentPoint32(hdc, SCORE[1], lstrlen(SCORE[1]), &size);

	SetTextColor(hdc, RGB(255, 255, 0));

	TextOut(hdc, 150, 200, SCORE[0], lstrlen(SCORE[0]));
	TextOut(hdc, WinSize.right - 150 - size.cx, 200, SCORE[1], lstrlen(SCORE[1]));

	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
}

void Scene::SetMinute(int m)
{
	Timer_M = m;
}

void Scene::SetSecond(int s)
{
	Timer_S = s;
}

Scene::~Scene()
{
	BackGround[0].Destroy();
	BackGround[1].Destroy();
	BackGround[2].Destroy();
	BackGround[3].Destroy();
	GoalPostL.Destroy();
	GoalPostR.Destroy();
}