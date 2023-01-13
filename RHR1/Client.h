#pragma once
#include "framework.h"
#include "ToolWin.h"
#include <vector>

class Client
{
private:
	HWND hWndDialog;
	MyToolWinClass toolWin;

	int counter = 0;

	std::vector< MyToolWinClass::MyButton*> buttons;

	int ParseIntFromControl(HWND, int);
public:
	void AddButton(HWND);
	void AddBitmap(HWND);
	void ChangeButton(HWND);
	void DeleteButton(HWND);
	void RunToolbar(HWND);

	void HideButton(HWND);
	void ShowButton(HWND);
	void EnableButton(HWND);
	void DisableButton(HWND);

	void OnInitDialog(HWND hWnd);
};