/***************************************************************************
 *   Copyright (C) 2008 by Deryabin Andrew                                 *
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

#include "ayfly.h"
#include <windows.h>

#define WINDOW_TEXT "Ayfly AY891x player v." AYFLY_VERSION_TEXT

LRESULT CALLBACK AyflyWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

ATOM AyflyClass;
HWND AyflyhWnd;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmdLine, int nShow)
{
    WNDCLASSEX clex;
    clex.cbClsExtra = 0;
    clex.cbSize = sizeof(WNDCLASSEX);
    clex.hCursor = LoadCursor(IDC_ARROW);
    clex.hIcon = 0;
    clex.hIconSm = 0;
    clex.hInstance = hInstance;
    clex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    clex.lpfnWndProc = AyflyWindowProc;
    clex.lpszClassName = TXT("ayfly class");
    clex.lpszMenuName = 0;    
    clex.style = CS_HREDRAW | CS_VREDRAW;
    ATOM AyflyClass = RegisterClassEx(&clex);
    if(AyflyClass)
    {
        AyflyhWnd = CreateWindow(AyflyClass, TXT(WINDOW_TEXT), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 560, 400, 0, 0, hInstance, 0);      
    }

}

LRESULT CALLBACK AyflyWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_CREATE:
            // Initialize the window. 
            return 0;

        case WM_PAINT:
            // Paint the window's client area. 
            return 0;

        case WM_SIZE:
            // Set the size and position of the window. 
            return 0;

        case WM_DESTROY:
            // Clean up window-specific data objects. 
            return 0;

            // 
            // Process other messages. 
            // 

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;

}
