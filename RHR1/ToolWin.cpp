#include "ToolWin.h"
#include <commctrl.h>
#include <algorithm>

MyToolWinClass::~MyToolWinClass()
{
    for (const auto& imageList : imageLists)
        ImageList_Destroy(imageList);

    for (const auto& button : buttons)
        delete button;

    if (IsWindow(hWndWindow))
        DestroyWindow(hWndWindow);


    if (IsWindow(hWndWindow) && atomWindow)
    {
        HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWndParent, GWLP_HINSTANCE);
        UnregisterClass(MAKEINTATOM(atomWindow), hInst);
    }
}

BOOL MyToolWinClass::Run(HWND hWnd, int hx, int vx, int bsize)
{
    if (IsWindow(hWndWindow))
    {
        SetActiveWindow(hWndWindow);
        return TRUE;
    }

    if (!hx || !vx)
        return FALSE;

    maxRows = hx;
    maxColumns = vx;

    hWndParent = hWnd;
    HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWndParent, GWLP_HINSTANCE);

    if (!(atomWindow || RegisterToolWinClass(hInst)))
        return FALSE;

    hWndWindow = CreateWindowEx(WS_EX_TOOLWINDOW,
        MAKEINTATOM(atomWindow), L"Засоби",
        WS_SYSMENU | WS_CAPTION,
        CW_USEDEFAULT, 0, 0, 0,
        hWndParent, NULL, hInst, this);

    hWndToolbar = CreateWindowEx(0,
        TOOLBARCLASSNAME, NULL,
        TBSTYLE_TRANSPARENT | TBSTYLE_WRAPABLE | TBSTYLE_TOOLTIPS
        | WS_CHILD,
        0, 0, 0, 0,
        hWndWindow, NULL, hInst, NULL);

    if (!(hWndWindow && hWndToolbar))
        return FALSE;

    SendMessage(hWndToolbar, CCM_SETVERSION, (WPARAM)5, 0);
    SendMessage(hWndToolbar, TB_SETBITMAPSIZE, 0, MAKELPARAM(bsize, bsize));
    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(hWndToolbar, TB_SETROWS, MAKEWPARAM(5, FALSE), NULL);

    for (int i = 0; i < imageLists.size(); i++)
        SendMessage(hWndToolbar, TB_SETIMAGELIST,
            (WPARAM)i,
            (LPARAM)imageLists[i]);

    std::vector<TBBUTTON> tempButtons;
    for (const auto& button : buttons)
        tempButtons.push_back(button->button);

    SendMessage(hWndToolbar, TB_ADDBUTTONS, tempButtons.size(), (LPARAM)tempButtons.data());

    ResizeWindow();

    ShowWindow(hWndWindow, SW_SHOW);
    ShowWindow(hWndToolbar, SW_SHOW);
    UpdateWindow(hWndWindow);
    UpdateWindow(hWndToolbar);
    return TRUE;
}

MyToolWinClass::MyButton* MyToolWinClass::AddButtonWithStyle(int commandId, int style)
{
    MyButton* button = new MyButton(this, commandId, style);
    buttons.push_back(button);

    ResizeWindow();

    return button;
}

MyToolWinClass::MyButton* MyToolWinClass::AddButton(int commandId)
{
    return AddButtonWithStyle(commandId, BTNS_BUTTON);
}

MyToolWinClass::MyButton* MyToolWinClass::AddCheck(int commandId)
{
    return AddButtonWithStyle(commandId, BTNS_CHECK);
}

void MyToolWinClass::ChangeButtonState(int id, int value)
{
    TBBUTTONINFO info{};
    info.cbSize = sizeof(TBBUTTONINFO);
    info.dwMask = TBIF_STATE | TBIF_BYINDEX;
    info.fsState = value;

    SendMessage(hWndToolbar, TB_SETBUTTONINFO, id, (LPARAM)&info);
}

void MyToolWinClass::ChangeButtonBitmap(int id, int value)
{
    TBBUTTONINFO info{};
    info.cbSize = sizeof(TBBUTTONINFO);
    info.dwMask = TBIF_IMAGE | TBIF_BYINDEX;
    info.iImage = value;

    SendMessage(hWndToolbar, TB_SETBUTTONINFO, id, (LPARAM)&info);
}

void MyToolWinClass::RemoveButton(int id)
{
    SendMessage(hWndToolbar, TB_DELETEBUTTON, id, 0);
    auto It = buttons.begin();
    std::advance(It, id);
    delete (*It);
    buttons.erase(It);

    ResizeWindow();
}

int MyToolWinClass::GetId(MyButton* button)
{
    auto elementIt = std::find(buttons.begin(), buttons.end(), button);
    int index = (int)std::distance(buttons.begin(), elementIt);
    return index;
}


int MyToolWinClass::AddBitmap(HINSTANCE resource, LPCWSTR resourceId, int size)
{
    int fuLoad = IS_INTRESOURCE(resourceId) ? NULL : LR_LOADFROMFILE;
    HIMAGELIST hImageList = ImageList_Create(size, size,
        ILC_COLOR32 | ILC_MASK, 0, 0);
    HBITMAP myBitmap = (HBITMAP)LoadImage(resource, resourceId,
        IMAGE_BITMAP, 0, 0, fuLoad);
    ImageList_Add(hImageList, myBitmap, NULL);
    DeleteObject(myBitmap);
    int imageListId = (int)imageLists.size();

    SendMessage(hWndToolbar, TB_SETIMAGELIST,
        (WPARAM)imageListId,
        (LPARAM)hImageList);

    imageLists.push_back(hImageList);
    return imageListId;
}

void MyToolWinClass::ResizeWindow()
{
    if (!IsWindow(hWndToolbar))
        return;
    int size = (int)SendMessage(hWndToolbar, TB_GETBUTTONSIZE, 0, 0);
    int width = LOWORD(size);
    int height = HIWORD(size);

    int columns = (int)buttons.size();
    int rows = (int)ceil((double)buttons.size() / maxColumns);

    if ((double)columns / maxColumns > 1)
        columns = maxColumns;

    if (rows > maxRows)
        rows = maxRows;

    RECT tb{ 0,0, width * columns, height * rows };
    AdjustWindowRect(&tb, WS_SYSMENU | WS_CAPTION, FALSE);
    SetWindowPos(hWndWindow, 0, 0, 0, tb.right - tb.left, tb.bottom - tb.top + 2,
        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
}

MyToolWinClass::MyButton::MyButton(MyToolWinClass* tb, int cId, int style)
{
    TBBUTTON button{};
    button.idCommand = cId;
    button.fsState = TBSTATE_ENABLED;
    button.fsStyle = style;
    button.iBitmap = MAKELONG(0, -1);

    int buttonsCount = (int)SendMessage(tb->hWndToolbar, TB_BUTTONCOUNT, 0, 0);
    SendMessage(tb->hWndToolbar, TB_INSERTBUTTON, (WPARAM)buttonsCount, (LPARAM)&button);

    this->mytb = tb;
    this->button = button;
    this->groupId = 0;
    this->tooltipText = nullptr;
}

MyToolWinClass::MyButton* MyToolWinClass::MyButton::SetImage(int id, int imggroupId)
{
    button.iBitmap = MAKELONG(id, imggroupId);
    mytb->ChangeButtonBitmap(mytb->GetId(this), button.iBitmap);

    return this;
}

MyToolWinClass::MyButton* MyToolWinClass::MyButton::SetTooltip(LPCWSTR text)
{
    tooltipText = text;

    return this;
}

MyToolWinClass::MyButton* MyToolWinClass::MyButton::Enable()
{
    button.fsState |= TBSTATE_ENABLED;
    mytb->ChangeButtonState(mytb->GetId(this), button.fsState);

    return this;
}

MyToolWinClass::MyButton* MyToolWinClass::MyButton::Disable()
{
    button.fsState &= ~TBSTATE_ENABLED;
    mytb->ChangeButtonState(mytb->GetId(this), button.fsState);

    return this;
}

MyToolWinClass::MyButton* MyToolWinClass::MyButton::Show()
{
    button.fsState &= ~TBSTATE_HIDDEN;
    mytb->ChangeButtonState(mytb->GetId(this), button.fsState);

    return this;
}

MyToolWinClass::MyButton* MyToolWinClass::MyButton::Hide()
{
    button.fsState |= TBSTATE_HIDDEN;
    mytb->ChangeButtonState(mytb->GetId(this), button.fsState);

    return this;
}

MyToolWinClass::MyButton* MyToolWinClass::MyButton::Delete()
{
    mytb->RemoveButton(mytb->GetId(this));
    return nullptr;
}

ATOM MyToolWinClass::RegisterToolWinClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"ToolWin";
    wcex.hIconSm = NULL;

    atomWindow = RegisterClassEx(&wcex);
    return atomWindow;
}

void MyToolWinClass::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh->code == TTN_NEEDTEXT)
    {
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
        for (const auto& button : buttons)
        {
            if (lpttt->hdr.idFrom == button->button.idCommand
                && button->tooltipText != nullptr)
            {
                lstrcpy(lpttt->szText, button->tooltipText);
            }
        }
        /*lstrcpy(lpttt->szText, L"Щось невідоме");*/
    }
}


LRESULT MyToolWinClass::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static MyToolWinClass* toolWin = nullptr;
    switch (message)
    {
    case WM_CREATE:
    {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        toolWin = (MyToolWinClass*)cs->lpCreateParams;
        break;
    }
    case WM_NOTIFY:
    {
        toolWin->OnNotify(hWnd, wParam, lParam);
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        default:
            SendMessage(toolWin->hWndParent, message, wParam, lParam);
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_DESTROY:
        DestroyWindow(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}