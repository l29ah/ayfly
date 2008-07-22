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
 
#ifndef MAINDROPTARGET_H_
#define MAINDROPTARGET_H_

class MainDropTarget : public IDropTarget
{
public:
	MainDropTarget(HWND _hWnd);
	virtual ~MainDropTarget();
	virtual HRESULT pascal QueryInterface(REFIID iid, void **ppvObject);
	virtual ULONG pascal AddRef();
	virtual ULONG pascal Release();
	virtual HRESULT pascal DragEnter(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
	virtual HRESULT pascal DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
	virtual HRESULT pascal DragLeave(void);
	virtual HRESULT pascal Drop(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
private:
	ULONG ulRefs;
	HWND hWnd;
	
};

#endif /*MAINDROPTARGET_H_*/
