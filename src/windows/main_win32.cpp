/***************************************************************************
 *   Copyright (C) 2008 by Deryabin Andrew   				               *
 *   andrew@it-optima.ru                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "common.h"
/*
#define CLASS_NAME TEXT("Ayfly class")
#define WINDOW_TEXT "Ayfly player :-) v." AYFLY_VERSION_TEXT

#define MENU_FILE_OPEN 2
#define MENU_FILE_EXIT 3
#define MENU_PLAYER_PLAY 5
#define MENU_PLAYER_REWIND 6
#define MENU_PLAYER_STOP 7
#define MENU_HELP_ABOUT 8

bool CreateGUI();
LRESULT pascal WndProcMain(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void OpenAyFile(TCHAR *strFile = 0);
bool fileOpened;
*/
HWND hWndMain = 0;
HINSTANCE hInst = 0;
HMENU hMainMenu = 0;
HACCEL hacc = 0;

/*
HWND hWndOpenBtn = 0;
HWND hWndPlayBtn = 0;
HWND hWndRewindBtn = 0;
HWND hWndStopBtn = 0;

int pascal WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    //WNDCLASSEXW wcex;	
    hInst = hInstance;
#ifndef _WIN32_WCE
    OleInitialize(0);
#endif

    CreateGUI();

    if (!hWndMain)
        return FALSE;

    fileOpened = false;

    SendMessageW(hWndMain, WM_SETTEXT, 0, (LPARAM) (TEXT(WINDOW_TEXT) TEXT(" - no file.")));

#ifndef _WIN32_WCE
    player = new DXAudio(44100);
#else
    DrawMenuBar(hWndMain);
    player = new WaveAudio(44100);
#endif
    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);


    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (hMainMenu)
        DestroyMenu(hMainMenu);
#ifndef _WIN32_WCE
    OleUninitialize();
#endif
    return (int) msg.wParam;

}

bool CreateGUI()
{


    
    
#ifndef _WIN32_WCE
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof (WNDCLASSEXW);
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hIcon = NULL;
    wcex.hIconSm = NULL;
    wcex.hInstance = hInst;
    wcex.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    wcex.lpfnWndProc = WndProcMain;
    wcex.lpszClassName = CLASS_NAME;
    wcex.lpszMenuName = NULL;
    wcex.style = CS_HREDRAW | CS_VREDRAW;

    ATOM aClass = RegisterClassExW(&wcex);
    hWndMain = CreateWindowW((LPWSTR) aClass, TEXT(WINDOW_TEXT), WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                             CW_USEDEFAULT, 0, 320, 240, NULL, NULL, hInst, 0);
    if (!hWndMain)
        return false;    
    hMainMenu = CreateMenu();
    if (hMainMenu)
    {
        HMENU hFileMenu = CreatePopupMenu();
        AppendMenuW(hFileMenu, MF_STRING, MENU_FILE_OPEN, TEXT("Open (&o)"));
        AppendMenuW(hFileMenu, MF_STRING, MENU_FILE_EXIT, TEXT("Exit"));

        HMENU hPlayerMenu = CreatePopupMenu();
        AppendMenuW(hPlayerMenu, MF_STRING, MENU_PLAYER_PLAY, TEXT("Play (&c)"));
        AppendMenuW(hPlayerMenu, MF_STRING, MENU_PLAYER_REWIND, TEXT("Rewind (&x)"));
        AppendMenuW(hPlayerMenu, MF_STRING, MENU_PLAYER_STOP, TEXT("Stop (&v)"));

        HMENU hHelpMenu = CreatePopupMenu();
        AppendMenuW(hHelpMenu, MF_STRING, MENU_HELP_ABOUT, TEXT("About"));

        AppendMenuW(hMainMenu, MF_STRING | MF_POPUP, (UINT) hFileMenu, TEXT("&File"));
        AppendMenuW(hMainMenu, MF_STRING | MF_POPUP, (UINT) hPlayerMenu, TEXT("&Player"));
        AppendMenuW(hMainMenu, MF_STRING | MF_POPUP, (UINT) hHelpMenu, TEXT("&Help"));
    }
    SetMenu(hWndMain, hMainMenu);
        
#else
    WNDCLASS wc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = NULL;
    wc.hInstance = hInst;
    wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    wc.lpfnWndProc = WndProcMain;
    wc.lpszClassName = CLASS_NAME;
    wc.lpszMenuName = NULL;
    wc.style = CS_HREDRAW | CS_VREDRAW;

    ATOM aClass = RegisterClass(&wc);
    hWndMain = CreateWindowW((LPWSTR) aClass, TEXT(WINDOW_TEXT), WS_OVERLAPPED | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                             CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInst, 0);
#endif

    hWndOpenBtn = CreateWindowW(TEXT("BUTTON"), TEXT("Open (&o)#"  xstr(MENU_FILE_OPEN)), WS_CHILDWINDOW | WS_VISIBLE | BS_FLAT | BS_TEXT | BS_DEFPUSHBUTTON, 5, 5, 70, 25, hWndMain, NULL, hInst, 0);
    hWndPlayBtn = CreateWindowW(TEXT("BUTTON"), TEXT("Play (&c)"), WS_CHILDWINDOW | WS_VISIBLE | BS_FLAT | BS_TEXT | BS_PUSHBUTTON, 80, 5, 70, 25, hWndMain, NULL, hInst, 0);
    hWndRewindBtn = CreateWindowW(TEXT("BUTTON"), TEXT("Rewind (&x)"), WS_CHILDWINDOW | WS_VISIBLE | BS_FLAT | BS_TEXT | BS_PUSHBUTTON, 155, 5, 70, 25, hWndMain, NULL, hInst, 0);
    hWndStopBtn = CreateWindowW(TEXT("BUTTON"), TEXT("Stop (&v)"), WS_CHILDWINDOW | WS_VISIBLE | BS_FLAT | BS_TEXT | BS_PUSHBUTTON, 230, 5, 70, 25, hWndMain, NULL, hInst, 0);
    

    return true;
}

#ifdef _WIN32_WCE

int main(int argc, char **argv)
{
    return WinMain((HINSTANCE) GetModuleHandle(0), (HINSTANCE) 0, (CHAR *) 0, SW_SHOW);
}
#endif

void OpenAyFile(TCHAR *strFile)
{
    TCHAR file [MAX_PATH];
    RtlZeroMemory(file, MAX_PATH * sizeof (TCHAR));
    if (strFile == 0)
    {
        OPENFILENAMEW ofn;
        RtlZeroMemory(&ofn, sizeof (OPENFILENAMEW));
        ofn.lStructSize = sizeof (OPENFILENAMEW);
        ofn.hwndOwner = hWndMain;
#ifndef _WIN32_WCE
        ofn.hInstance = hInst;
#endif
        ofn.lpstrFilter = L"AY-891x files (*.stc, *.stp, *.pt2, *.pt3, *.ay, *.psc)\0*.stc;*.stp;*.pt2;*.pt3;*.ay;*.psc\0\0";
        ofn.lpstrCustomFilter = 0;
        ofn.nMaxCustFilter = 0;
        ofn.nFilterIndex = 0;
        RtlZeroMemory(file, sizeof (MAX_PATH) * sizeof (TCHAR));
        ofn.lpstrFile = file;
        ofn.nMaxFile = MAX_PATH - 1;
        ofn.lpstrFileTitle = 0;
        ofn.Flags = 0;
        ofn.lpstrDefExt = 0;
        ofn.lCustData = 0;
        ofn.lpfnHook = 0;
        ofn.lpTemplateName = 0;
        ofn.lpstrTitle = TEXT("Open AY file..");
        if (GetOpenFileNameW(&ofn) != TRUE)
        {
            MessageBoxW(hWndMain, TEXT("Error opening file"), TEXT("Error!"), MB_ICONSTOP);
            return;
        }
    }
    else
#ifndef _MSC_VER
        wsprintf(file, TEXT("%s"), strFile);
#else
        StringCbPrintfW(file, MAX_PATH * sizeof (TCHAR), TEXT("%s"), strFile);
#endif



    fileOpened = false;

    bool started = player->Started();
    player->Stop();
    shutdownSpeccy();

    initSpeccy();
    if (readFile(file))
    {
        TCHAR windowText [8192];
        TCHAR fileName [MAX_PATH];
        RtlZeroMemory(windowText, 8192 * sizeof (TCHAR));
        RtlZeroMemory(fileName, MAX_PATH * sizeof (TCHAR));
        //_tsplitpath(file, 0, 0, fileName, 0);
        int len = wcslen(file);
        while (--len)
        {
            if (file [len] == L'\\')
            {
#ifndef _MSC_VER
                wsprintf(fileName, TEXT("%s"), &file [len + 1]);
#else
                StringCbPrintfW(fileName, MAX_PATH * sizeof (TCHAR), TEXT("%s"), &file [len + 1]);
#endif
                break;
            }
        }
#ifndef _MSC_VER
        wsprintf(windowText, (TEXT(WINDOW_TEXT) TEXT(" - %s")), fileName);
#else
        StringCbPrintfW(windowText, 8192 * sizeof (TCHAR), (TEXT(WINDOW_TEXT) TEXT(" - %s")), fileName);
#endif
        SendMessage(hWndMain, WM_SETTEXT, 0, (LPARAM) windowText);

        fileOpened = true;

        if (started)
            player->Start();
    }
}

LRESULT pascal WndProcMain(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    int wmId, wmEvent;
    wchar_t buffer [100];

    switch (message)
    {
    case WM_CREATE:
#ifndef _WIN32_WCE
        RegisterDragDrop(hWnd, new MainDropTarget(hWnd));
#endif
        return TRUE;
        break;
    case WM_KEYDOWN:
        //wsprintf(buffer, TEXT("0x%x"), wParam);
        //MessageBox(hWnd, buffer, TEXT("!!!!"), MB_OK);
        switch(wParam)
        {
        case 0x4f: //Open file
            SendMessage(hWnd, WM_COMMAND, MENU_FILE_OPEN, 0);
            break;
        case 0x43: //Play
            SendMessage(hWnd, WM_COMMAND, MENU_PLAYER_PLAY, 0);
            break;
        case 0x58: //Rewind
            SendMessage(hWnd, WM_COMMAND, MENU_PLAYER_REWIND, 0);
            break;
        case 0x56: //Stop
            SendMessage(hWnd, WM_COMMAND, MENU_PLAYER_STOP, 0);
            break;
        default:
            break;
        }
        break;
    case WM_COMMAND:
        wmId = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        wsprintf(buffer, TEXT("%d"), wParam);
        MessageBoxW(hWnd, buffer, TEXT("!!!!"), MB_OK);        
        switch (wmId)
        {
        case MENU_FILE_OPEN:
            OpenAyFile();
            break;
        case MENU_FILE_EXIT:
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case MENU_HELP_ABOUT:
            MessageBoxW(hWnd, TEXT("Ayfly player. \nAndrew Deryabin, 2008.\nAlexander Shatin, 2008."), TEXT("About.."), MB_OK);
            break;        
        case MENU_PLAYER_PLAY:
            if (fileOpened)
            {
                if (player->Started())
                {
                    player->Stop();
                    SetWindowTextW(hWndPlayBtn, TEXT("Play (&c)"));
                }
                else
                {
                    player->Start();
                    SetWindowTextW(hWndPlayBtn, TEXT("Pause (&c)"));
                }
            }
            break;
        case MENU_PLAYER_REWIND:
            if (fileOpened)
            {
                bool started = player->Started();
                if (started)
                {
                    player->Stop();
                    SetWindowTextW(hWndPlayBtn, TEXT("Play (&c)"));
                }

                resetSpeccy();
                if (started)
                {
                    player->Start();
                    SetWindowTextW(hWndPlayBtn, TEXT("Pause (&c)"));
                }
            }
            break;
        case MENU_PLAYER_STOP:
            if (fileOpened)
            {
                if (player->Started())
                {
                    player->Stop();
                    SetWindowTextW(hWndPlayBtn, TEXT("Play (&c)"));
                }
                resetSpeccy();
            }
            break;
        default:
            
            break;
        }
        break;
    case WM_DROP_FILE:
        OpenAyFile((TCHAR *) lParam);
        break;
    case WM_GETDLGCODE:
        return DLGC_BUTTON | DLGC_HASSETSEL | DLGC_UNDEFPUSHBUTTON | DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTTAB;
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        delete player;
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
#ifndef _WIN32_WCE
        RevokeDragDrop(hWnd);
#endif
        DestroyWindow(hWnd);
        break;
    default:
        break;
    }
    return DefWindowProcW(hWnd, message, wParam, lParam);
}
*/
