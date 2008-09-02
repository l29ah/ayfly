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

const unsigned long BitBufSiz = 16;
const unsigned long UCharMax = 255;
const unsigned long DicBit = 13;
const unsigned long DicSiz = 1 << DicBit;
const unsigned long MatchBit = 8;
const unsigned long MaxMatch = 1 << MatchBit;
const unsigned long ThResHold = 3;
const unsigned long PercFlag = 0x8000;
const unsigned long Nc = (UCharMax + MaxMatch + 2 - ThResHold);
const unsigned long CBit = 9;
const unsigned long CodeBit = 16;
const unsigned long Np = DicBit + 1;
const unsigned long Nt = CodeBit + 3;
const unsigned long PBit = 4;
const unsigned long TBit = 5;
const unsigned long Npt = Nt;
const unsigned long Nul = 0;
const unsigned long MaxHashVal = (3 * DicSiz + ((DicSiz << 9) + 1) * UCharMax);
const unsigned long WinBit = 14;
const unsigned long WindowSize = 1 << WinBit;
const unsigned long BufBit = 13;
const unsigned long BufSize = 1 << BufBit;

struct lha_params
{
    const unsigned char *file_data;
    const unsigned long file_len;
    unsigned long file_offset;
    unsigned long OrigSize, CompSize;
    unsigned char *OutPtr;
    unsigned short BitBuf;
    unsigned short SubBitBuf, BitCount;
    unsigned char Buffer[BufSize];
    unsigned short BufPtr;
    unsigned short BlockSize;
    unsigned short Left[2 * Nc], Right[2 * Nc];
    unsigned short PtTable[256];
    unsigned char PtLen[Npt];
    unsigned short CTable[4096];
    unsigned char CLen[Nc];
};

unsigned char GetC(lha_params &params)
{
    unsigned long l;
    if(BufPtr == 0)
    {
        unsigned long sz = (BufSize + params.file_offset) >= params.file_len ? (params.file_len - params.file_offset) : BufSize;
        memcpy(params.Buffer, params.file_data + params.file_offset, sz);
        params.file_offset += sz;
    }
    params.BufPtr = (params.BufPtr + 1) % (BufSize - 1);
    return params.Buffer[params.BufPtr];
}

void BWrite(lha_params &params, unsigned char *P, int N)
{
    int T;
    unsigned char *Scan;
    Scan = P;
    for(T = 1; T < N; T++)
    {
        *params.OutPtr = *Scan;
        params.Scan++;
        params.OutPtr++;
    }
}

void FillBuf(lha_params &params, int n)
{
    params.BitBuf = (params.BitBuf << n);
    while(n > params.BitCount)
    {
        n -= params.BitCount;
        params.BitBuf = params.BitBuf | (params.SubBitBuf << n);
        if(params.CompSize != 0)
        {
            Dec(CompSize);
            params.SubBitBuf = GetC(params);
        }
        else
            params.SubBitBuf = 0;
        params.BitCount = 8;
    }
    params.BitCount -= n;
    params.BitBuf = params.BitBuf | (params.SubBitBuf >> params.BitCount);
}

unsigned short GetBits(lha_params &params, int n)
{
    unsigned short GetBits = params.BitBuf >> (params.BitBufSiz - n);
    FillBuf(params, n);
    return GetBits;
}

void InitGetBits(lha_params &params)
{
  params.BitBuf = 0;
  params.SubBitBuf = 0;
  params.BitCount = 0;
  FillBuf (params.BitBufSiz);
}

bool MakeTable (lha_params &params, int nChar, unsigned char *BitLen, int TableBits, unsigned short *Table)
{
  unsigned short Count [16], Weight [16];
  unsigned short Start [17];
  unsigned short *p /* XWord */;
  int i, k, Len, Ch, JutBits, Avail, NextCode, Mask;
  for(i = 0; i < 16; i++)
      Count [i] = 0;
  for(i = 0; i < nChar; i++)
      Count [(*BitLen [i]) - 1]++;
  Start [0] = 0;
  for(i = 0; i < 16; i++)
    Start [i + 1] = Start [i] + (Count [i] << (15 - i));
  if(Start [16] != 0)
      return false;
  JutBits = 16 - TableBits;
  for(i = 0; i < TableBits; i++)
  {
    Start [i] = Start [i] >> JutBits;
    Weight [i] = 1 << (TableBits - 1 - i);
  }
  i = TableBits + 1;
  while(i <= 16)
  {
    Weight [i] = 1 << (16 - i);
    i++;
  }
  i = Start [TableBits] >> JutBits;
  if(i != 0)
  {
    k = 1 << TableBits;
    while(i != k)
    {
      *Table [i - 1] = 0;
      i++;
    }
  }
  Avail = nChar;
  Mask = 1 << (15 - TableBits);
  For Ch := 0 To Pred (nChar) Do
  Begin
    Len := BitLen^ [Ch];
    If Len = 0 Then Continue;
    k := Start [Len];
    NextCode := k + Weight [Len];
    If Len <= TableBits Then
    Begin
      For i := k To Pred (NextCode) Do Table^ [i] := Ch;
    End Else
    Begin
      p := Addr (Table^ [k Shr JutBits]);
      i := Len - TableBits;
      While i <> 0 Do
      Begin
        If p^ [0] = 0 Then
        Begin
          Right [Avail] := 0;
          Left [Avail] := 0;
          p^ [0] := Avail;
          Inc (Avail);
        End;
        If (k And Mask) <> 0 Then p := Addr (Right [p^ [0]])
          Else p := Addr (Left [p^ [0]]);
        k := k Shl 1;
        Dec (i);
      End;
      p^ [0] := Ch;
    End;
    Start [Len] := NextCode;
  End;
}
