#include "ayfly.h" //for usual dll linking use ayfly.h
#include <windows.h>
#include <iostream>
#include <stdio.h>

using namespace std;

//called at the end of song/start of loop
bool song_end(void *songinfo);
//simple end flag
bool end;

int _tmain(int argc, TCHAR **argv)
{
    if(argc < 2)
    {
        cout << "At least one filename must be given!" << endl;
        return 0;
    }
    

    for(int i = 1; i < argc; i++)
    {
        TCHAR *filename = argv [i];
        cout << "Loading song " << filename << ".." << endl;
        void *songinfo = ay_initsong(filename, 44100);
        if(songinfo == 0)
        {
            cout << "Error loading song. Skipping.." << endl;
            continue;
        }
        cout << "Playing song " << filename << ".." << endl;
        cout << "Song length = " << (ay_getsonglength(songinfo) / 50) << " seconds.." << endl;
        end = false;
        ay_setelapsedcallback(songinfo, song_end, songinfo);
        //important!! our window handle must be set BEFORE playback start!
        HWND hWnd = GetConsoleWindow();
        ay_sethwnd(songinfo, hWnd);
        ay_startsong(songinfo);
        bool ret = ay_songstarted(songinfo);
        while(!end)
        {
            Sleep(10);
            unsigned long elapsed = ay_getelapsedtime(songinfo) / 50;
            printf("Elapsed time: %lu seconds\r", elapsed);
        }        
        ay_stopsong(songinfo);
        cout << endl;
        cout << "Song " << filename << " ended." << endl;
    }
    //close library handle
    return 0;

}

bool song_end(void *songinfo)
{
    //just mark the end of song now..
    end = true;
	return end;
}