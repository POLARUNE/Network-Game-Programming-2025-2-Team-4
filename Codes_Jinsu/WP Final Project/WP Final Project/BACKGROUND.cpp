#include <Windows.h>
#include <atlimage.h>
#include "BACKGROUND.h"

extern CImage BackGround;
extern CImage GoalPostR, GoalPostL;

extern RECT WinSize;

extern int Timer_M;
extern int Timer_S;

extern TCHAR Timer[10];

extern int SceneNum;
extern int P1Num, P2Num;

extern CImage Char[2][10];
extern 	CImage CharP1;
extern CImage CharP2;

extern CImage ResBG;

void DrawBG(HDC hdc)
{
	SIZE size;

	BackGround.BitBlt(hdc, 0, 0, 1000, 740, 0, 0, SRCCOPY);
	GoalPostR.TransparentBlt(hdc, 930, 570, 62, 109, 0, 0, 62, 109, RGB(255, 255, 255));
	GoalPostL.TransparentBlt(hdc, 30, 570, 62, 109, 0, 0, 62, 109, RGB(255, 255, 255));

	HFONT hFont, oldFont;

	hFont = CreateFont(50, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("±º∏≤"));
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

	hFont = CreateFont(40, 0, 0, 0, FW_ULTRABOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("π‡¿∫∞ÌµÒ"));
	oldFont = (HFONT)SelectObject(hdc, hFont);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 70, 200));

	wsprintf(Timer, L"%d : %02d", Timer_M, Timer_S);
	TextOut(hdc, WinSize.right / 2 - 40, size.cy + 25, Timer, lstrlen(Timer));

	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
}

void DrawSelectBG(HDC hdc, int P1Score, int P2Score)
{
	Char[0][0].Load(_T("sprite\\Alien_L.png"));
	Char[0][1].Load(_T("sprite\\Asura_L.png"));
	Char[0][2].Load(_T("sprite\\Brazil_L.png"));
	Char[0][3].Load(_T("sprite\\Cameroon_L.png"));
	Char[0][4].Load(_T("sprite\\Canada_L.png"));
	Char[0][5].Load(_T("sprite\\Egypt_L.png"));
	Char[0][6].Load(_T("sprite\\Israel_L.png"));
	Char[0][7].Load(_T("sprite\\Italy_L.png"));
	Char[0][8].Load(_T("sprite\\Korea_L.png"));
	Char[0][9].Load(_T("sprite\\Poland_L.png"));

	Char[1][0].Load(_T("sprite\\Alien_R.png"));
	Char[1][1].Load(_T("sprite\\Asura_R.png"));
	Char[1][2].Load(_T("sprite\\Brazil_R.png"));
	Char[1][3].Load(_T("sprite\\Cameroon_R.png"));
	Char[1][4].Load(_T("sprite\\Canada_R.png"));
	Char[1][5].Load(_T("sprite\\Egypt_R.png"));
	Char[1][6].Load(_T("sprite\\Israel_R.png"));
	Char[1][7].Load(_T("sprite\\Italy_R.png"));
	Char[1][8].Load(_T("sprite\\Korea_R.png"));
	Char[1][9].Load(_T("sprite\\Poland_R.png"));

	if (SceneNum == 2) {
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < 5; ++j) {
				if ((j + (i * 5) == 1) || (j + (i * 5) == 4) || (j + (i * 5) == 5)) {
					Char[0][j + (i * 5)].TransparentBlt(hdc, 250 + (j * 100), 30 + (i * 120), 100, 100, RGB(0, 255, 0));
				}

				else {
					Char[0][j + (i * 5)].TransparentBlt(hdc, 250 + (j * 100), 30 + (i * 120), 100, 100, RGB(255, 0, 0));
				}
			}
		}

		if ((P1Num == 1) || (P1Num == 4) || (P1Num == 5)) {
			Char[0][P1Num].TransparentBlt(hdc, 50, 400, 300, 300, RGB(0, 255, 0));
		}

		else {
			Char[0][P1Num].TransparentBlt(hdc, 50, 400, 300, 300, RGB(255, 0, 0));
		}

		if ((P2Num == 1) || (P2Num == 4) || (P2Num == 5)) {
			Char[1][P2Num].TransparentBlt(hdc, WinSize.right - 350, 400, 300, 300, RGB(0, 255, 0));
		}

		else {
			Char[1][P2Num].TransparentBlt(hdc, WinSize.right - 350, 400, 300, 300, RGB(255, 0, 0));
		}
	}

	else if (SceneNum == 4) {
		ResBG.Load(_T("sprite\\ResultBG.png"));
		ResBG.StretchBlt(hdc, WinSize, SRCCOPY);

		TCHAR result[2][10];

		HFONT hFont, oldFont;
		SIZE size;

		hFont = CreateFont(100, 0, 0, 0, FW_ULTRABOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("±º∏≤"));
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

		if ((P1Num == 1) || (P1Num == 4) || (P1Num == 5)) {
			Char[0][P1Num].TransparentBlt(hdc, 50, 400, 300, 300, RGB(0, 255, 0));
		}

		else {
			Char[0][P1Num].TransparentBlt(hdc, 50, 400, 300, 300, RGB(255, 0, 0));
		}

		if ((P2Num == 1) || (P2Num == 4) || (P2Num == 5)) {
			Char[1][P2Num].TransparentBlt(hdc, WinSize.right - 350, 400, 300, 300, RGB(0, 255, 0));
		}

		else {
			Char[1][P2Num].TransparentBlt(hdc, WinSize.right - 350, 400, 300, 300, RGB(255, 0, 0));
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
}

void ResultBG(HDC hdc)
{
	switch (P1Num) {
	case 0:
		CharP1.Load(_T("sprite\\Alien_L.png"));
		break;

	case 1:
		CharP1.Load(_T("sprite\\Asura_L.png"));
		break;

	case 2:
		CharP1.Load(_T("sprite\\Brazil_L.png"));
		break;

	case 3:
		CharP1.Load(_T("sprite\\Cameroon_L.png"));
		break;

	case 4:
		CharP1.Load(_T("sprite\\Canada_L.png"));
		break;

	case 5:
		CharP1.Load(_T("sprite\\Egypt_L.png"));
		break;

	case 6:
		CharP1.Load(_T("sprite\\Israel_L.png"));
		break;

	case 7:
		CharP1.Load(_T("sprite\\Italy_L.png"));
		break;

	case 8:
		CharP1.Load(_T("sprite\\Korea_L.png"));
		break;

	case 9:
		CharP1.Load(_T("sprite\\Poland_L.png"));
		break;
	}

	switch (P2Num) {
	case 0:
		CharP2.Load(_T("sprite\\Alien_L.png"));
		break;

	case 1:
		CharP2.Load(_T("sprite\\Asura_L.png"));
		break;

	case 2:
		CharP1.Load(_T("sprite\\Brazil_L.png"));
		break;

	case 3:
		CharP1.Load(_T("sprite\\Cameroon_L.png"));
		break;

	case 4:
		CharP1.Load(_T("sprite\\Canada_L.png"));
		break;

	case 5:
		CharP1.Load(_T("sprite\\Egypt_L.png"));
		break;

	case 6:
		CharP1.Load(_T("sprite\\Israel_L.png"));
		break;

	case 7:
		CharP1.Load(_T("sprite\\Italy_L.png"));
		break;

	case 8:
		CharP1.Load(_T("sprite\\Korea_L.png"));
		break;

	case 9:
		CharP1.Load(_T("sprite\\Poland_L.png"));
		break;
	}
}

void DeleteSelBG()
{
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			Char[i][j].Destroy();
		}
	}
}

void DeleteResBG()
{
	ResBG.Destroy();
	CharP1.Destroy();
	CharP2.Destroy();
}