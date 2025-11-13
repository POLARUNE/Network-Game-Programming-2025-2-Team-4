#include "Korea.h"

extern RECT WinSize;

Korea::Korea(int playerNum) : Character()
{
	imgHead[0].Load(_T("Sprites\\Korea_Face_L.png"));
	imgHead[1].Load(_T("Sprites\\Korea_Face_L.png"));
	
	imgBody[0].Load(_T("Sprites\\Body_Normal.png"));
	imgBody[1].Load(_T("Sprites\\Body_Kick_L.png"));
	imgBody[2].Load(_T("Sprites\\Body_Kick_R.png"));
	
	imgFlag.Load(_T("Sprites\\Korea_Flag.png"));

	switch (playerNum) {
	case 1:
		xPos = 100;
		// Power.Load(_T("sprite\\koreaPower_L.png"));
		break;

	case 2:
		xPos = 875;
		// Power.Load(_T("sprite\\koreaPower_R.png"));
		break;
	}

	yPos = 630;

	jump = 5;
	speed = 5;
	power = 5;
}

void Korea::UI_Print(HDC hdc, int playerNum) const
{
	HFONT hFont, oldFont;
	TCHAR str[10];

	SetTextColor(hdc, RGB(255, 255, 0));
	_itow(score, str, 10);

	hFont = CreateFont(40, 0, 0, 0, FW_ULTRABOLD, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("밝은고딕"));
	oldFont = (HFONT)SelectObject(hdc, hFont);

	switch (playerNum) {
	case 1:
		TextOut(hdc, WinSize.right / 4 + 60, 140, str, lstrlen(str));

		imgFlag.StretchBlt(hdc, 285, 57, 100, 66, SRCCOPY);

		// PowerGaugeL.TransparentBlt(hdc, 269 - (248 * PwGauge / 100), 12, 248 * PwGauge / 100, 98, 100 - PwGauge, 0, PwGauge, 30, RGB(255, 255, 255));
		// PowerGaugeFrameL.TransparentBlt(hdc, 20, 10, 250, 100, RGB(255, 255, 255));

		break;

	case 2:
		TextOut(hdc, WinSize.right / 4 * 3 - 80, 140, str, lstrlen(str));

		imgFlag.StretchBlt(hdc, 615, 57, 100, 66, SRCCOPY);

		// PowerGaugeR.TransparentBlt(hdc, WinSize.right - 269, 12, 248 * PwGauge / 100, 98, 0, 0, PwGauge, 30, RGB(255, 255, 255));
		// PowerGaugeFrameR.TransparentBlt(hdc, WinSize.right - 270, 10, 250, 100, RGB(255, 255, 255));

		break;
	}

	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
}

void Korea::Draw(HDC hdc, int playerNum) const
{
	switch (playerNum) {
	case 1:
		// Body[kick].TransparentBlt(hdc, xPos, yPos, CHAR_SIZE, CHAR_SIZE, RGB(255, 255, 255));
		imgHead[playerNum - 1].TransparentBlt(hdc, xPos, yPos - 7, CHAR_SIZE, CHAR_SIZE, RGB(255, 0, 0));

		break;

	case 2:
		// Body[kick].TransparentBlt(hdc, xPos, yPos, CHAR_SIZE, CHAR_SIZE, RGB(255, 255, 255));
		imgHead[playerNum - 1].TransparentBlt(hdc, xPos, yPos - 7, CHAR_SIZE, CHAR_SIZE, RGB(255, 0, 0));

		break;
	}
}

//void Korea::PowerShoot(HDC hdc, int playerNum, double xpos, double ypos)
//{
//	switch (playerNum) {
//	case 1:
//		Power.TransparentBlt(hdc, xpos - 80, ypos - 20, 130, 70, RGB(255, 0, 0));
//		break;
//
//	case 2:
//		Power.TransparentBlt(hdc, xpos - 20, ypos - 20, 130, 70, RGB(255, 0, 0));
//		break;
//	}
//
//	PwGauge = 0;
//}

Korea::~Korea()
{
	imgHead[0].Destroy();
	imgHead[1].Destroy();
	imgBody[0].Destroy();
	imgBody[1].Destroy();
	imgBody[2].Destroy();
	// Power.Destroy();
	imgFlag.Destroy();
}