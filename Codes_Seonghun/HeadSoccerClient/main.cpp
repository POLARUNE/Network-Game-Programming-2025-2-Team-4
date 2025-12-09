#include "..\Common.h"

#include "Character.h"
#include "Korea.h"
#include "Canada.h"
#include "Brazil.h"
#include "Ball.h"
#include "Background.h"

#include <atlimage.h>

#pragma comment (lib, "msimg32.lib")
#pragma comment (lib, "winmm.lib")

char SERVERIP[64];

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

char PlayersReady = 0b00000000; // p1 p2 p3 self 순으로 2비트씩
char Readytemp;

char KeyBuf = 0b00000000; // 0 0 0 w a s d f 순서.

DWORD WINAPI ServerThread(LPVOID arg);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	strcpy(SERVERIP, lpCmdLine);

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
	if (!hWnd) {
		printf("CreateWindow failed: %d\n", GetLastError());
		return 0;
	}
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

Scene* BackGround;

Character* P1;
Character* P2;

Ball* ball;

RECT P1Rect, P2Rect;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, memdc;
	PAINTSTRUCT ps;

	static HBITMAP hBitmap;

	static RECT ButtonPlay = { 120, 650, 360, 700 };
	static RECT ButtonExit = { 680, 655, 930, 700 };

	static POINT mouse;

	switch (iMessage) {
	case WM_CREATE:
		GetClientRect(hWnd, &WinSize);
		SceneNum = SCENE_START;
		BackGround = new Scene();
		BackGround->SetMinute(1);
		BackGround->SetSecond(0);
		PlaySound(L"Sound\\bgm.wav", NULL, SND_ASYNC | SND_LOOP);
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case 'W':
		case 'w':

			KeyBuf |= 0b10000;
			break;

		case 'A':
		case 'a':

			KeyBuf |= 0b1000;
			break;

		case 'S':
		case 's':

			KeyBuf |= 0b100;
			break;

		case 'D':
		case 'd':

			KeyBuf |= 0b10;
			break;

		case 'F':
		case 'f':

			KeyBuf |= ~KeyBuf & (KeyBuf + 1);
			break;
		}

		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_KEYUP:
		switch (wParam) {
		case 'W':
		case 'w':

			KeyBuf &= ~(1<<4);
			break;

		case 'A':
		case 'a':

			KeyBuf &= ~(1 << 3);
			break;

		case 'S':
		case 's':

			KeyBuf &= ~(1 << 2);
			break;

		case 'D':
		case 'd':

			KeyBuf &= ~(1 << 1);
			break;

		case 'F':
		case 'f':

			KeyBuf &= KeyBuf - 1;
			break;
		}

		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_LBUTTONDOWN:
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);

		switch (SceneNum)
		{
		case SCENE_START:
			if (PtInRect(&ButtonPlay, mouse)) {
				SceneNum = SCENE_READY;
				CreateThread(NULL, 0, ServerThread, NULL, 0, NULL);
			}
			else if (PtInRect(&ButtonExit, mouse)) {
				PostQuitMessage(0);
			}

			break;

		case SCENE_READY:
			if (sqrt(pow(500 - mouse.x, 2) + pow(530 - mouse.y, 2)) <= 110) {
				PlayersReady ^= 0b10;

				if (PlayersReady == 0b10101010)
				{
					P1 = new Korea(1);
					ball = new Ball();
				}
			}

			break;

		case SCENE_RESULT:
			if (sqrt(pow(500 - mouse.x, 2) + pow(570 - mouse.y, 2)) <= 100) {
				delete P1;
				delete P2;
				delete BackGround;
				delete ball;

				PostQuitMessage(0);
			}
			break;
		}

		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		hBitmap = CreateCompatibleBitmap(hdc, WinSize.right, WinSize.bottom);
		memdc = CreateCompatibleDC(hdc);

		(HBITMAP)SelectObject(memdc, hBitmap);

		switch (SceneNum) {
		case SCENE_START:
			BackGround->DrawStartBG(memdc);
			break;

		case SCENE_READY:
			BackGround->DrawReadyBG(memdc);
			break;

		case SCENE_PLAY:
			BackGround->DrawPlayBG(memdc, P1->CharScore(), P2->CharScore());
			
			P1->UI_Print(memdc, 1);
			P2->UI_Print(memdc, 2);

			ball->Draw(memdc);

			P1->Draw(memdc, 1);
			P2->Draw(memdc, 2);

			break;

		case SCENE_RESULT:
			BackGround->DrawResultBG(memdc, P1->CharScore(), P2->CharScore());
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
		delete BackGround;
		delete ball;

		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

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
		// 씬 넘버 수신
		retval = recv(sock, (char*)&SceneNum, sizeof(int), 0);
		if (retval == SOCKET_ERROR) err_quit("recv() - SceneNum");

		switch (SceneNum)
		{
		case SCENE_READY:
			// 플레이어 준비 상태 송신
			retval = send(sock, (char*)&PlayersReady, sizeof(char), 0);
			if (retval == SOCKET_ERROR) err_quit("send() - PlayersReady");

			// 모든 플레이어 준비 상태 수신
			Readytemp = 0;
			retval = recv(sock, (char*)&Readytemp, sizeof(char), 0);
			if (retval == SOCKET_ERROR) err_quit("recv() - PlayersReady");
			else PlayersReady |= Readytemp;

			break;

		case SCENE_PLAY:
			// 키보드 입력 값 송신
			retval = send(sock, (char*)&KeyBuf, sizeof(char), 0);
			if (retval == SOCKET_ERROR) err_quit("send() - KeyBuf");
			break;

		case SCENE_RESULT:
			// 서버와 통신 종료
			closesocket(sock);
			return 0;
		}
	}

	closesocket(sock);
	return 0;
}