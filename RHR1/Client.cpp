#include "Client.h"
#include "resource.h"
#include <commdlg.h>

void Client::AddButton(HWND hWnd)
{
    buttons.push_back(toolWin.AddButton(counter + 10000));

    HWND lvhWnd = GetDlgItem(hWndDialog, IDC_LIST1);

    WCHAR buffer[64];
    wsprintf(buffer, L"Êíîïêà %d", counter);

    counter += 1;

    LVITEM lvI{};
    lvI.mask = LVIF_TEXT;
    lvI.iItem = ListView_GetItemCount(lvhWnd);
    lvI.pszText = (LPWSTR)buffer;

    if (ListView_InsertItem(lvhWnd, &lvI) == -1)
        return;
}

void Client::AddBitmap(HWND hWnd)
{
    OPENFILENAME ofn = { 0 };
    TCHAR szFile[260] = { 0 };
    // Initialize remaining fields of OPENFILENAME structure
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = _T("Bitmap image\0*.BMP\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;


    int bsize = ParseIntFromControl(hWndDialog, IDC_EDIT6);
    if (GetOpenFileName(&ofn) == TRUE && bsize)
    {
        toolWin.AddBitmap(NULL, szFile, bsize);
    }
}

void Client::ChangeButton(HWND hWnd)
{
    HWND lvhWnd = GetDlgItem(hWndDialog, IDC_LIST1);
    int iPos = ListView_GetNextItem(lvhWnd, -1, LVNI_SELECTED);

    WCHAR check[2];
    GetWindowText(GetDlgItem(hWnd, IDC_EDIT2), check, 2);
    BOOL isImageIdEmpty = (check[0] == 0);
    GetWindowText(GetDlgItem(hWnd, IDC_EDIT3), check, 2);
    BOOL isImageGroupIdEmpty = (check[0] == 0);

    if (iPos == -1)
        return;

    if (!isImageIdEmpty && !isImageGroupIdEmpty)
    {
        int imageId = ParseIntFromControl(hWndDialog, IDC_EDIT2);
        int imageGroupId = ParseIntFromControl(hWndDialog, IDC_EDIT3);
        buttons[iPos]->SetImage(imageId, imageGroupId);
    }

    WCHAR* tooltipText = new WCHAR[1024];
    GetWindowText(GetDlgItem(hWnd, IDC_EDIT1), tooltipText, 1024);
    if (tooltipText[0])
        buttons[iPos]->SetTooltip(tooltipText);
}

void Client::DeleteButton(HWND hWnd)
{
    HWND lvhWnd = GetDlgItem(hWndDialog, IDC_LIST1);
    int iPos = ListView_GetNextItem(lvhWnd, -1, LVNI_SELECTED);

    if (iPos == -1) return;

    ListView_DeleteItem(lvhWnd, iPos);
    buttons[iPos]->Delete();
    buttons.erase(buttons.begin() + iPos);
}

void Client::RunToolbar(HWND hWnd)
{
    int hx = ParseIntFromControl(hWndDialog, IDC_EDIT4);
    int vx = ParseIntFromControl(hWndDialog, IDC_EDIT5);
    int bsize = ParseIntFromControl(hWndDialog, IDC_EDIT6);
    if (!hx || !vx || !bsize)
        return;
	toolWin.Run(hWnd, hx, vx, bsize);
}

void Client::HideButton(HWND hWnd)
{
    HWND lvhWnd = GetDlgItem(hWndDialog, IDC_LIST1);
    int iPos = ListView_GetNextItem(lvhWnd, -1, LVNI_SELECTED);

    if (iPos == -1) return;

    buttons[iPos]->Hide();
}

void Client::ShowButton(HWND hWnd)
{
    HWND lvhWnd = GetDlgItem(hWndDialog, IDC_LIST1);
    int iPos = ListView_GetNextItem(lvhWnd, -1, LVNI_SELECTED);

    if (iPos == -1) return;

    buttons[iPos]->Show();
}

void Client::EnableButton(HWND hWnd)
{
    HWND lvhWnd = GetDlgItem(hWndDialog, IDC_LIST1);
    int iPos = ListView_GetNextItem(lvhWnd, -1, LVNI_SELECTED);

    if (iPos == -1) return;

    buttons[iPos]->Enable();
}

void Client::DisableButton(HWND hWnd)
{
    HWND lvhWnd = GetDlgItem(hWndDialog, IDC_LIST1);
    int iPos = ListView_GetNextItem(lvhWnd, -1, LVNI_SELECTED);

    if (iPos == -1) return;

    buttons[iPos]->Disable();
}

void Client::OnInitDialog(HWND hWnd)
{
    hWndDialog = hWnd;

    HWND lvhWnd = GetDlgItem(hWndDialog, IDC_LIST1);
    ListView_SetExtendedListViewStyle(lvhWnd, LVS_EX_AUTOSIZECOLUMNS | LVS_EX_FULLROWSELECT);
}

int Client::ParseIntFromControl(HWND hWnd, int nIDDlgItem)
{
    WCHAR num[1024];
    GetWindowText(GetDlgItem(hWnd, nIDDlgItem), num, 1024);
    return wcstol(num, NULL, 10);
}