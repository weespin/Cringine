#include "Game.h"

LRESULT GameWndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	InputSystem::GetInstance().ProcessInput(hWnd, msg, wParam, lParam);
	return true;
}