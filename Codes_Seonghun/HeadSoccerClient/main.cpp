#include "..\Common.h"

#include "Character.h"
#include "Korea.h"
#include "Canada.h"
#include "Brazil.h"
#include "Ball.h"
#include "Background.h"

#include <atlimage.h>

#pragma comment (lib, "msimg32.lib")

#define SERVERIP		"127.0.0.1"
#define SERVERPORT		9000
#define BUFSIZE			128

#define SCENE_START		1001
#define SCENE_READY		1002
#define SCENE_PLAY		1003
#define SCENE_RESULT	1004

SOCKET sock;
char buf[BUFSIZE + 1];

HINSTANCE g_hInst;
TCHAR lpszClass[] = TEXT("HEAD SOCCER");

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

DWORD WINAPI ServerThread(LPVOID arg)
{
	int retval;

	// 소켓 생성
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	while (1)
	{

	}

	closesocket(sock);
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	if (!hPrevInstance) {
		WndClass.cbClsExtra = 0;
		WndClass.cbWndExtra = 0;
		WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		WndClass.hInstance = hInstance;
		WndClass.lpfnWndProc = (WNDPROC)WndProc;
		WndClass.lpszClassName = lpszClass;
		WndClass.lpszMenuName = NULL;
		WndClass.style = CS_HREDRAW | CS_VREDRAW;
		RegisterClass(&WndClass);
	}
	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, 0, 0, 1016, 779, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	// 메시지 루프
	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	// 윈속 종료
	WSACleanup();
	return Message.wParam;
}

RECT WinSize;

int SceneNum;

Scene BackGround;

Character* P1;
Character* P2;

Ball ball;

/*
BOOL CrashCheck = FALSE;

RECT P1Rect, P2Rect;

int P1Num, P2Num;
BOOL Kick1, Kick2;
BOOL Goal1, Goal2;

BOOL Pause = FALSE;
BOOL PlayerReady = FALSE;

CImage Char[2][10];
CImage CharP1;
CImage CharP2;

BOOL P1Power, P2Power;
BOOL P1Crash, P2Crash;
*/

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, memdc;
	PAINTSTRUCT ps;

	static HBITMAP hBitmap;

	static RECT ButtonPlay = { 120, 650, 360, 700 };
	static RECT ButtonExit = { 680, 655, 930, 700 };

	static POINT mouse;
	
	/*
	static BOOL KeyBuffer[256] = { FALSE };

	static int JmpCnt1, JmpCnt2;

	RECT CrashSize;

	static RECT CharSelRect[10];

	static int P1Score, P2Score;
	*/

	switch (iMessage) {
	case WM_CREATE:
		GetClientRect(hWnd, &WinSize);
		SceneNum = SCENE_START;

		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case 'W':
		case 'w':
			break;
		case 'A':
		case 'a':
			break;
		case 'S':
		case 's':
			break;
		case 'D':
		case 'd':
			break;
		case 'F':
		case 'f':
			break;
		}

		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_KEYUP:
		switch (wParam) {
		case 'W':
		case 'w':
			break;
		case 'A':
		case 'a':
			break;
		case 'S':
		case 's':
			break;
		case 'D':
		case 'd':
			break;
		case 'F':
		case 'f':
			break;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		hBitmap = CreateCompatibleBitmap(hdc, WinSize.right, WinSize.bottom);
		memdc = CreateCompatibleDC(hdc);

		(HBITMAP)SelectObject(memdc, hBitmap);

		switch (SceneNum) {
		case SCENE_START:
			BackGround.DrawStartBG(memdc);
			break;

		case SCENE_READY:
			BackGround.DrawReadyBG(memdc);
			break;

		case SCENE_PLAY:
			BackGround.DrawPlayBG(memdc, P1->CharScore(), P2->CharScore());
			
			P1->UI_Print(memdc, 1);
			P2->UI_Print(memdc, 2);

			ball.Draw(memdc);

			P1->Draw(memdc, 1);
			P2->Draw(memdc, 2);

			break;

		case SCENE_RESULT:
			BackGround.DrawResultBG(memdc, P1->CharScore(), P2->CharScore());
			break;
		}

		BitBlt(hdc, 0, 0, 1000, 800, memdc, 0, 0, SRCCOPY);
		DeleteDC(memdc);

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		DeleteObject(hBitmap);
		
		delete P1;
		delete P2;

		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}