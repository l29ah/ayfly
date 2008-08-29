#include "ayfly_dynamicdll.h" //for usual dll linking use ayfly.h
#include <windows.h>
#include <iostream>
#include <stdio.h>

using namespace std;

//called at the end of song/start of loop
void song_end(void *songinfo);
//simple end flag
bool end;

//imports that will be used.
ay_initsong pay_initsong;
ay_setcallback pay_setcallback;
ay_startsong pay_startsong;
ay_getelapsedtime pay_getelapsedtime;
ay_stopsong pay_stopsong;
ay_sethwnd pay_sethwnd;
ay_songstarted pay_songstarted;
ay_getsonglength pay_getsonglength;

int _tmain(int argc, TCHAR **argv)
{
    if(argc < 2)
    {
        cout << "At least one filename must be given!" << endl;
        return 0;
    }
    cout << "Loading ayfly.dll.." << endl;
    HMODULE hDll = LoadLibraryW(L"ayfly.dll");
    if(hDll == NULL)
    {
        cout << "Error loading ayfly.dll" << endl;
        return 1;
    }
    cout << "Dll successfully loaded." << endl;

    //get pointers to functions;
    pay_initsong = (ay_initsong)GetProcAddress(hDll, "ay_initsong");
    pay_setcallback = (ay_setcallback)GetProcAddress(hDll, "ay_setcallback");
    pay_startsong = (ay_startsong)GetProcAddress(hDll, "ay_startsong");
    pay_getelapsedtime = (ay_getelapsedtime)GetProcAddress(hDll, "ay_getelapsedtime");
    pay_stopsong = (ay_stopsong)GetProcAddress(hDll, "ay_stopsong");
    pay_sethwnd = (ay_sethwnd)GetProcAddress(hDll, "ay_sethwnd");
    pay_songstarted = (ay_songstarted)GetProcAddress(hDll, "ay_songstarted");
    pay_getsonglength = (ay_getsonglength)GetProcAddress(hDll, "ay_getsonglength");

    for(int i = 1; i < argc; i++)
    {
        TCHAR *filename = argv [i];
        cout << "Loading song " << filename << ".." << endl;
        void *songinfo = pay_initsong(filename, 44100);
        if(songinfo == 0)
        {
            cout << "Error loading song. Skipping.." << endl;
            continue;
        }
        cout << "Playing song " << filename << ".." << endl;
        cout << "Song length = " << (pay_getsonglength(songinfo) / 50) << " seconds.." << endl;
        end = false;
        pay_setcallback(songinfo, song_end, songinfo);
        //important!! our window handle must be set BEFORE playback start!
        HWND hWnd = GetConsoleWindow();
        pay_sethwnd(songinfo, hWnd);
        pay_startsong(songinfo);
        bool ret = pay_songstarted(songinfo);
        while(!end)
        {
            Sleep(10);
            unsigned long elapsed = pay_getelapsedtime(songinfo) / 50;
            printf("Elapsed time: %lu seconds\r", elapsed);
        }        
        pay_stopsong(songinfo);
        cout << endl;
        cout << "Song " << filename << " ended." << endl;
    }
    //close library handle
    FreeLibrary(hDll);
    return 0;

}

void song_end(void *songinfo)
{
    //just mark the end of song now..
    end = true;
}