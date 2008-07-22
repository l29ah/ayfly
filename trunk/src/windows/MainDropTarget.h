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
