#include "common.h"

MainDropTarget::MainDropTarget(HWND _hWnd)
{
	ulRefs = 0;
	hWnd = _hWnd;
}

MainDropTarget::~MainDropTarget()
{
}

HRESULT MainDropTarget::QueryInterface(REFIID iid, void **ppvObject)
{
	if(iid == IID_IUnknown || iid == IID_IDropTarget)
	{
		AddRef();
		*ppvObject = this;
		return S_OK;
	}
	return E_NOINTERFACE;	
}

ULONG MainDropTarget::AddRef()
{
	ulRefs++;	
	return ulRefs;
}
ULONG MainDropTarget::Release()
{
	if(--ulRefs)
		delete this;
	return ulRefs;
}

HRESULT MainDropTarget::DragEnter(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	*pdwEffect = DROPEFFECT_NONE;
	HRESULT hRet = E_INVALIDARG;
	IEnumFORMATETC *ienum = 0;
	if(SUCCEEDED(pDataObject->EnumFormatEtc(DATADIR_GET, &ienum)))
	{
		FORMATETC fmtc;
		while(1)
		{
			RtlZeroMemory(&fmtc, sizeof(FORMATETC));
			if(ienum->Next(1, &fmtc, NULL) != S_OK)
				break;

			if(fmtc.ptd)
				CoTaskMemFree(fmtc.ptd);

			if(fmtc.cfFormat == 0x0)
				break;

			if(fmtc.cfFormat == CF_HDROP)
			{
				*pdwEffect = DROPEFFECT_COPY;
				hRet = S_OK;
				break;
			}
		}
		ienum->Release();
	}
	return hRet;
	
}

HRESULT MainDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	*pdwEffect = DROPEFFECT_COPY;
	return S_OK;
}

HRESULT MainDropTarget::DragLeave(void)
{
	return S_OK;	
}

HRESULT pascal MainDropTarget::Drop(IDataObject *pDataObject, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	*pdwEffect = DROPEFFECT_NONE;
	HRESULT hRet = E_INVALIDARG;
	IEnumFORMATETC *ienum = 0;
	if(SUCCEEDED(pDataObject->EnumFormatEtc(DATADIR_GET, &ienum)))
	{
		FORMATETC fmtc;
		while(1)
		{
			RtlZeroMemory(&fmtc, sizeof(FORMATETC));
			if(ienum->Next(1, &fmtc, NULL) != S_OK)
				break;

			if(fmtc.ptd)
				CoTaskMemFree(fmtc.ptd);

			if(fmtc.cfFormat == 0x0)
				break;

			if(fmtc.cfFormat == CF_HDROP)
			{
				*pdwEffect = DROPEFFECT_COPY;
				hRet = S_OK;
				STGMEDIUM medium;
				RtlZeroMemory(&medium, sizeof(STGMEDIUM));
				fmtc.tymed = TYMED_HGLOBAL;
				if(SUCCEEDED(pDataObject->GetData(&fmtc, &medium)))
				{
					HDROP hDrop = (HDROP)medium.hGlobal;
					UINT iFiles = DragQueryFileW(hDrop, 0xffffffff, NULL, 0);
					if(iFiles > 0)
					{
						for(UINT i = 0; i < iFiles; i++)
						{
							UINT strLen = DragQueryFileW(hDrop, i, NULL, 0) + 1;
							TCHAR *strFile = new TCHAR [strLen * sizeof(TCHAR)];
							if(strFile)
							{
								RtlZeroMemory(strFile, strLen * sizeof(TCHAR));
								UINT cnt = DragQueryFileW(hDrop, i, strFile, strLen);
								if(cnt)
								{
									SendMessageW(hWnd, WM_DROP_FILE, 0, (LPARAM)strFile);
									i = iFiles;
								}
								delete [] strFile;
							}
						}
					}
					ReleaseStgMedium(&medium);
					
				}
			}
		}
		ienum->Release();
	}
	return hRet;
}
