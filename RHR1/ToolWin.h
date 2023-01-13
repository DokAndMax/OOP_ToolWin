#pragma once
#include "framework.h"
#include <commctrl.h>
#include <list>
#include <vector>

class MyToolWinClass
{
public:
	class MyButton
	{
		friend class MyToolWinClass;
	private:
		MyButton(MyToolWinClass*, int, int);
		MyButton(const MyButton&);
		MyButton& operator=(MyButton&);
		MyToolWinClass* mytb;

		TBBUTTON button;
		int groupId;
		LPCWSTR tooltipText;
	public:
		MyButton* SetImage(int, int);
		MyButton* SetTooltip(LPCWSTR);
		MyButton* Enable();
		MyButton* Disable();
		MyButton* Show();
		MyButton* Hide();
		MyButton* Delete();
	};

	~MyToolWinClass();

	BOOL Run(HWND, int, int, int);
	MyButton* AddButton(int);
	MyButton* AddCheck(int);
	void ChangeButtonState(int, int);
	void ChangeButtonBitmap(int, int);
	void RemoveButton(int);
	int GetId(MyButton*);
	int AddBitmap(HINSTANCE, LPCWSTR, int);

private:
	HWND hWndParent;
	HWND hWndWindow;
	HWND hWndToolbar;
	ATOM atomWindow;

	int maxRows;
	int maxColumns;

	std::list<MyButton*> buttons;
	std::vector<HIMAGELIST> imageLists;

	ATOM RegisterToolWinClass(HINSTANCE);
	void ResizeWindow();

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	void OnNotify(HWND, WPARAM, LPARAM);

	MyButton* AddButtonWithStyle(int, int);
};