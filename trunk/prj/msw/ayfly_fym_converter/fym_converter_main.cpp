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

bool end;
char *out_dir;
FILE *fxml;

bool elapsed_callback(void *)
{
	end = true;
	return end;
}

void usage()
{
	fprintf(stderr, "\tusage: ayfly_fym_converter <input_dir> <output_dir>\n" );

}

void replace_for_xml(char *str)
{
	size_t len = strlen(str);
	size_t i = 0;
	while (i < len)
	{
		if(str [i] == '<' || str [i] == '<' || str [i] == '&')
			str [i] = ' ';
		else if(str [i] == '!')
			str [i] = '1';
		else if(str [i] == '"')
			str [i] = '\'';
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

void ProcessDir(char *dir)
{
	WIN32_FIND_DATA dt;
	char mask [(MAX_PATH + 4) * sizeof(char)];
	sprintf(mask, "%s\\*.*", dir);
	std::vector<unsigned char> regs [14];
	unsigned char regs_old [14];
	void *song;

	HANDLE hf = FindFirstFile(mask, &dt);
	if(hf != INVALID_HANDLE_VALUE)
	{
		bool ret = TRUE;
		while(ret == TRUE)
		{
			if(strcmp(dt.cFileName, ".") && strcmp(dt.cFileName, ".."))
			{
				if((dt.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					char next_dir [MAX_PATH + 1];
					sprintf(next_dir, "%s\\%s", dir, dt.cFileName);
					ProcessDir(next_dir);
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
						printf("%s\n", res_path);
						for(unsigned char i = 0; i < 14; i++)
						{
							regs [i].clear();
						}
						memset(regs_old, 255, 14);

						unsigned long ptr = 0;
						unsigned char *temp_buffer = (unsigned char *)malloc (1024);
						unsigned long dstlen = 1024;						
						FILE *f = fopen(res_path, "wb");						
						ay_setelapsedcallback(song, elapsed_callback, 0);
						ay_seeksong(song, ay_getelapsedtime(song) + 1);
						const char *songname = ay_getsongname(song);
						const char *songauthor = ay_getsongauthor(song);
						unsigned long head_len = strlen(songname) + strlen(songauthor) + 2 + 5 * 4;
						while(ay_getsonglength(song) > ay_getelapsedtime(song))
						{
							const unsigned char *regs_raw = ay_getregs(song);
							for(unsigned char i = 0; i < 14; i++)
							{
								if((i == 13) && (regs_old [i] == regs_raw [i]))
									regs [i].push_back(255);
								else
								{
									regs [i].push_back(regs_raw [i]);
									regs_old [i] = regs_raw [i];										
								}
							}
							ay_seeksong(song, ay_getelapsedtime(song) + 1);
						}
						unsigned long len = regs [0].size();
						write_z(&temp_buffer, &head_len, 4, &dstlen, &ptr);
						write_z(&temp_buffer, &len, 4, &dstlen, &ptr);
						unsigned long loop = ay_getsongloop(song);
						write_z(&temp_buffer, &loop, 4, &dstlen, &ptr);
						unsigned long chipfreq = ay_getayfreq(song);
						write_z(&temp_buffer, &chipfreq, 4, &dstlen, &ptr);
						unsigned long framefreq = ay_getintfreq(song);
						write_z(&temp_buffer, &framefreq, 4, &dstlen, &ptr);
						write_z(&temp_buffer, songname, strlen(songname) + 1, &dstlen, &ptr);
						write_z(&temp_buffer, songauthor, strlen(songauthor) + 1, &dstlen, &ptr);
						for(unsigned long i = 0; i < 14; i++)
						{
							std::vector<unsigned char>::pointer ptr_reg = &regs [i] [0];
							write_z(&temp_buffer, ptr_reg, len, &dstlen, &ptr);
						}
						unsigned char *comp_buffer = (unsigned char *)malloc(ptr + 1);
						dstlen = ptr + 1;
						compress2(comp_buffer, &dstlen, temp_buffer, ptr, 9);
						fwrite(comp_buffer, 1, dstlen, f);
						fclose(f);

						char xml_entry [4096];
						unsigned long time = ay_getsonglength(song);
						float seconds_f = time / framefreq;
						unsigned long seconds = seconds_f;
						if((float)seconds != seconds_f)
							seconds++;
						unsigned long minutes = seconds / 60;
						seconds = seconds % 60;

						sprintf((char *)temp_buffer, "%s", songname);
						replace_for_xml((char *)temp_buffer);

						sprintf(xml_entry, "\t<fym url=\"%s.fym\" name=\"%s\" time=\"%.2u:%.2u\" size=\"%.2f kb\" />\r\n", fname, temp_buffer, minutes, seconds, (float)dstlen / 1024);
						fwrite(xml_entry, 1, strlen(xml_entry), fxml);

						ay_closesong(&song);
						free(temp_buffer);
						free(comp_buffer);

					}
				}
			}
			ret = FindNextFile(hf, &dt);
		}

	}
}

int main(int argc, char **argv)
{
	fprintf(stderr, "AY 891x file to fym converter v. " AYFLY_VERSION_TEXT ", ");
	fprintf(stderr, "Deryabin Andrew, 2009. GNU GPL v2 license.\n");
	if(argc < 3)
	{
		usage();
		exit(1);
	}

	out_dir = argv [2];

	if(!CreateDirectory(out_dir, 0))
	{
		if(GetLastError() != ERROR_ALREADY_EXISTS)
		{
			fprintf(stderr, "Can't create output directory!!\n");
			exit(1);
		}
	}
	
	char tmp_buf [MAX_PATH];
	sprintf(tmp_buf, "%s\\list.xml", out_dir);

	fxml = fopen(tmp_buf, "wb");
	char *str = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n";
	fwrite(str, 1, strlen(str), fxml);
	str = "<fyms>\r\n";
	fwrite(str, 1, strlen(str), fxml);

	ProcessDir(argv [1]);

	str = "</fyms>\r\n";
	fwrite(str, 1, strlen(str), fxml);

	return 0;
}
