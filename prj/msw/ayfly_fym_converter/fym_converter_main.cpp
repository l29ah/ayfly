/***************************************************************************
*   Copyright (C) 2009 by Deryabin Andrew                                 *
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
#include <stdio.h>
#include <fcntl.h>
#include <vector>
#include <zlib.h>
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>

bool end;
char *out_dir;
FILE *fxml;
bool is_reg13 [2];

HWND editsrc;
HWND editdst;
HWND msglist;
HWND convertbtn;
#define ID_EDITSRC 1000
#define ID_EDITDST 1001
#define ID_MSGLIST 1002
#define ID_GETSRC 1003
#define ID_GETDST 1004
#define ID_CONVERT 1005

char srcdir [MAX_PATH * 2];
char dstdir [MAX_PATH * 2];

bool elapsed_callback(void *)
{
	end = true;
	return end;
}

void usage()
{
	fprintf(stderr, "\tusage: ayfly_fym_converter <input_dir> <output_dir>\n" );

}

void trimr(char *str)
{
	long i = strlen(str);
	if(i == 0)
		return;
	i -= 1;
	while (str [i] == ' ')
	{
		i--;
	}
	str [i + 1] = 0;
}

void replace_for_xml(char *str)
{
	trimr(str);
	size_t len = strlen(str);
	size_t i = 0;
	while (i < len)
	{
		if(str [i] == '<' || str [i] == '<' || str [i] == '&')
			str [i] = ' ';
		else if(str [i] == '!')
			str [i] = 'i';
		else if(str [i] == '"')
			str [i] = '_';
		else if(str [i] == '\'')
			str [i] = '_';
		else if(str [i] == '@')
			str [i] = 'e';
		else if(str [i] == '+')
			str [i] = 't';
		else if(((unsigned char)str [i]) < 32)
		{
			len = strlen(&str [i]);
			memmove(&str [i], &str [i + 1], len);
			i--;
			len = strlen(str);
		}
		else
			str [i] = tolower(str [i]);
		i++;
	}
}

void replace_for_xml_lite(char *str)
{
	trimr(str);
	size_t len = strlen(str);
	size_t i = 0;
	while (i < len)
	{
		if(str [i] == '<')
		{
			str [i] = '(';
		}
		else if(str [i] == '>')
		{
			str [i] = ')';
		}		
		else if(str [i] == '"')
		{
			memmove(&str [i + 2], &str [i + 1], len - i);
			memcpy(&str [i], "''", 2);
			len = strlen(str);
			i += 1;
		}
		else if(((unsigned char)str [i]) == '#')
		{
			len = strlen(&str [i]);
			memmove(&str [i], &str [i + 1], len);
			i--;
			len = strlen(str);
		}
		i++;
	}
}

void write_z(unsigned char **dst, const void *src, unsigned long len, unsigned long *dstlen, unsigned long *ptr)
{
	unsigned long newlen = *ptr + len;
	if(newlen > *dstlen)
	{
		newlen = newlen > 1024 ? newlen : 1024;
		*dst = (unsigned char *)realloc(*dst, newlen);
		*dstlen = newlen;		
	}
	memcpy(*dst + *ptr, src, len);
	*ptr += len;
}

void aywrite_callback(void *info, unsigned long chip_nr, unsigned char reg, unsigned char val)
{
	if(reg == 13)
		is_reg13 [chip_nr] = true;
}

void ProcessDir(char *dir, char *short_dir)
{
	if(!strcmp(out_dir, dir))
		return;
	WIN32_FIND_DATA dt;
	char mask [(MAX_PATH + 4) * sizeof(char)];
	sprintf(mask, "%s\\*.*", dir);
	std::vector<unsigned char> regs [28];
	void *song;
	char xml_entry [4096];
	bool first = true;	
	HANDLE hf = FindFirstFile(mask, &dt);
	if(hf != INVALID_HANDLE_VALUE)
	{
		if(strcmp(short_dir, "") && first)
		{
			sprintf(xml_entry, "\t<fym name=\"%s\" />\r\n", short_dir);
			fwrite(xml_entry, 1, strlen(xml_entry), fxml);
			first = false;
		}
		bool ret = TRUE;
		while(ret == TRUE)
		{
			if(strcmp(dt.cFileName, ".") && strcmp(dt.cFileName, ".."))
			{
				if((dt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					char next_dir [MAX_PATH + 1];
					sprintf(next_dir, "%s\\%s", dir, dt.cFileName);
					ProcessDir(next_dir, dt.cFileName);
				}
				else
				{
					char fullpath [MAX_PATH];
					sprintf(fullpath, "%s\\%s", dir, dt.cFileName);
					char fname [MAX_PATH];
					memset(fname, 0, sizeof(fname));
					_splitpath(dt.cFileName, 0, 0, fname, 0);
					char res_path [MAX_PATH];
					replace_for_xml(fname);
					sprintf(res_path, "%s\\%s.fym", out_dir, fname);
					song = ay_initsong(fullpath, 44100, 0);
					if(song)
					{
						SendMessage(msglist, LB_ADDSTRING, 0, (LPARAM)dt.cFileName);
						SendMessage(msglist, LB_SETCURSEL, SendMessage(msglist, LB_GETCOUNT, 0, 0) - 1, 0);
						//printf("%s\n", res_path);
						for(unsigned char i = 0; i < 14; i++)
						{
							regs [i].clear();
							regs [i + 14].clear();
						}
						unsigned long ptr = 0;
						unsigned char *temp_buffer = (unsigned char *)malloc (1024);
						unsigned long dstlen = 1024;						
						FILE *f = fopen(res_path, "wb");
						ay_setelapsedcallback(song, elapsed_callback, 0);
						ay_seeksong(song, ay_getelapsedtime(song) + 1);
						is_reg13 [0] = is_reg13 [1] = true;
						ay_setaywritecallback(song, aywrite_callback);
						const char *songname = ay_getsongname(song);
						const char *songauthor = ay_getsongauthor(song);
						unsigned long head_len = strlen(songname) + strlen(songauthor) + 2 + 5 * 4;
						while(ay_getsonglength(song) > ay_getelapsedtime(song))
						{
							unsigned long chip_nr = 0;
							while(chip_nr < 2)
							{
								const unsigned char *regs_raw = ay_getregs(song, chip_nr);
								for(unsigned char i = 0; i < 14; i++)
								{
									if((i == 13) && !is_reg13 [chip_nr])
										regs [i + chip_nr * 14].push_back(255);
									else
									{
										regs [i + chip_nr * 14].push_back(regs_raw [i]);
										if(i == 13)
											is_reg13 [chip_nr] = false;
									}
								}
								if(!ay_ists(song))
									break;
								chip_nr++;
							}
							ay_seeksong(song, ay_getelapsedtime(song) + 1);
						}
						unsigned long chip_nr = 0;

						unsigned long framefreq = ay_getintfreq(song);
						while(chip_nr < 2)
						{
							unsigned long len = regs [chip_nr * 14].size();
							write_z(&temp_buffer, &head_len, 4, &dstlen, &ptr);
							write_z(&temp_buffer, &len, 4, &dstlen, &ptr);
							unsigned long loop = ay_getsongloop(song);
							write_z(&temp_buffer, &loop, 4, &dstlen, &ptr);
							unsigned long chipfreq = ay_getayfreq(song);
							write_z(&temp_buffer, &chipfreq, 4, &dstlen, &ptr);							
							write_z(&temp_buffer, &framefreq, 4, &dstlen, &ptr);
							write_z(&temp_buffer, songname, strlen(songname) + 1, &dstlen, &ptr);
							write_z(&temp_buffer, songauthor, strlen(songauthor) + 1, &dstlen, &ptr);
							for(unsigned long i = 0; i < 14; i++)
							{
								std::vector<unsigned char>::pointer ptr_reg = &regs [i + chip_nr * 14] [0];
								write_z(&temp_buffer, ptr_reg, len, &dstlen, &ptr);
							}
							

							if(!ay_ists(song))
								break;
							chip_nr++;
						}

						unsigned char *comp_buffer = (unsigned char *)malloc(ptr * 2);
						dstlen = ptr * 2;
						compress2(comp_buffer, &dstlen, temp_buffer, ptr, 9);
						fwrite(comp_buffer, 1, dstlen, f);
						free(comp_buffer);
						fclose(f);
						

						unsigned long time = ay_getsonglength(song);
						float seconds_f = time / framefreq;
						unsigned long seconds = seconds_f;
						if((float)seconds != seconds_f)
							seconds++;
						unsigned long minutes = seconds / 60;
						seconds = seconds % 60;

						
						if(ay_ists(song))
							sprintf((char *)temp_buffer, "[ts] %s", songname);
						else
							sprintf((char *)temp_buffer, "%s", songname);
						replace_for_xml_lite((char *)temp_buffer);
						sprintf(xml_entry, "\t\t<fym url=\"%s.fym\" name=\"%s\" time=\"%u:%.2u\" size=\"%.2f kb\" />\r\n", fname, temp_buffer, minutes, seconds, (float)dstlen / 1024);
						fwrite(xml_entry, 1, strlen(xml_entry), fxml);

						ay_closesong(&song);
						free(temp_buffer);
						

					}
				}
			}
			ret = FindNextFile(hf, &dt);
		}

	}
}

void convert(char *srcdir, char *dstdir)
{

	out_dir = dstdir;

	if(!CreateDirectory(out_dir, 0))
	{
		if(GetLastError() != ERROR_ALREADY_EXISTS)
		{
			fprintf(stderr, "Can't create output directory!!\n");
			return;
		}
	}

	char tmp_buf [MAX_PATH];
	sprintf(tmp_buf, "%s\\list.xml", out_dir);

	fxml = fopen(tmp_buf, "wbc");
	char *str = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n";
	fwrite(str, 1, strlen(str), fxml);
	str = "<fyms>\r\n";
	fwrite(str, 1, strlen(str), fxml);

	ProcessDir(srcdir, "");

	str = "</fyms>\r\n";
	fwrite(str, 1, strlen(str), fxml);
	fclose(fxml);
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	convert(srcdir, dstdir);
	SendMessage(msglist, LB_ADDSTRING, 0, (LPARAM)"-->Conversion ended...");
	SendMessage(msglist, LB_SETCURSEL, SendMessage(msglist, LB_GETCOUNT, 0, 0) - 1, 0);
	EnableWindow(convertbtn, TRUE);

	return 0;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CREATE:
		{
			/*CreateWindow("STATIC",
				"Source Path:",
				WS_VISIBLE | WS_CHILD,
				5, 7, 90, 20,
				hwnd, 
				0, 
				(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE), 
				NULL);       // pointer not needed 
				*/

			editsrc = CreateWindow("EDIT",      // predefined class 
				NULL,        // no window title 
				WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER, 
				100, 5, 260, 20,  // set size in WM_SIZE message 
				hwnd,        // parent window 
				(HMENU) ID_EDITSRC,   // edit control ID 
				(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE), 
				NULL);       // pointer not needed 

			CreateWindow("BUTTON",
				"...",
				WS_CHILD | WS_VISIBLE,
				365, 5, 25, 20,
				hwnd, 
				(HMENU) ID_GETSRC, 
				(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE), 
				NULL);       // pointer not needed 

			/*CreateWindow("STATIC",
				"Dst Path:",
				WS_CHILD | WS_VISIBLE,
				5, 37, 90, 20,
				hwnd, 
				0, 
				(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE), 
				NULL);       // pointer not needed 
				*/

			editdst = CreateWindow("EDIT",      // predefined class 
				NULL,        // no window title 
				WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER, 
				100, 35, 260, 20,  // set size in WM_SIZE message 
				hwnd,        // parent window 
				(HMENU) ID_EDITDST,   // edit control ID 
				(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE), 
				NULL);       // pointer not needed 

			CreateWindow("BUTTON",
				"...",
				WS_CHILD | WS_VISIBLE,
				365, 35, 25, 20,
				hwnd, 
				(HMENU) ID_GETDST, 
				(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE), 
				NULL);       // pointer not needed 

			msglist = CreateWindow("LISTBOX",      // predefined class 
				NULL,        // no window title 
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL, 
				5, 65, 385, 160,  // set size in WM_SIZE message 
				hwnd,        // parent window 
				(HMENU) ID_MSGLIST,   // edit control ID 
				(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE), 
				NULL);       // pointer not needed

			convertbtn = CreateWindow("BUTTON",
				"Convert !",
				WS_CHILD | WS_VISIBLE,
				320, 220, 70, 20,
				hwnd, 
				(HMENU) ID_CONVERT, 
				(HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE), 
				NULL);       // pointer not needed

		}
		break;
	case WM_DESTROY:
		exit(0);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);
			RECT rt = {5, 7, 90, 25};
			SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, "Source path:", 12, &rt, 0);
			rt.left = 5;
			rt.top = 37;
			rt.right = 90;
			rt.bottom = 25 + 37;
			DrawText(hdc, "Dst path:", 9, &rt, 0);
			EndPaint(hwnd, &ps);
		}
		break;
	case WM_COMMAND:
		switch(HIWORD(wParam))
		{
		case BN_CLICKED:
			if(LOWORD(wParam) == ID_GETSRC || LOWORD(wParam) == ID_GETDST)
			{
				BROWSEINFO info;
				memset(&info, 0, sizeof(info));
				info.hwndOwner = hwnd;
				info.iImage = 0;
				info.pidlRoot = 0;
				info.lpszTitle = "Select folder...";
				info.ulFlags = BIF_NEWDIALOGSTYLE;
				info.lpfn = 0;
				info.lParam = 0;
				PIDLIST_ABSOLUTE pidl = SHBrowseForFolder(&info);
				if(pidl)
				{
					char tmp_dir [MAX_PATH * 2];
					char buffer [MAX_PATH * 2];
					SHGetPathFromIDList(pidl, buffer);
					if(LOWORD(wParam) == ID_GETSRC)
						SetWindowText(editsrc, buffer);
						memset(tmp_dir, 0, sizeof(tmp_dir));
						GetWindowText(editdst, tmp_dir, sizeof(srcdir));
						if(strlen(tmp_dir) == 0)
						{
							strcat(buffer, "\\fym");
							SetWindowText(editdst, buffer);

						}
					else
						SetWindowText(editdst, buffer);
				}

			}
			else if(LOWORD(wParam) == ID_CONVERT)
			{				
				GetWindowText(editsrc, srcdir, sizeof(srcdir));
				GetWindowText(editdst, dstdir, sizeof(dstdir));
				SendMessage(msglist, LB_RESETCONTENT, 0, 0);
				SendMessage(msglist, LB_ADDSTRING, 0, (LPARAM)"-->Starting conversion...");
				EnableWindow(convertbtn, FALSE);
				CreateThread(0, 0, ThreadProc, 0, 0, 0);
			}
			break;
		default:
			break;
		}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}


int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	CoInitialize(0);
	INITCOMMONCONTROLSEX ccex;
	ccex.dwSize = sizeof(ccex);
	ccex.dwICC = ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&ccex);
	ATOM a;
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
	wcex.hIconSm = wcex.hIcon;
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = WndProc;
	wcex.lpszClassName = "Ayfly class";
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	a = RegisterClassEx(&wcex);
	HWND hWnd = CreateWindow((LPCSTR)a, "Ayfly to FYM converter v." AYFLY_VERSION_TEXT, 
		WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_SYSMENU, 
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 280, 0, 0, 
		hInstance, 0);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg;
	while(GetMessage(&msg, hWnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;


}