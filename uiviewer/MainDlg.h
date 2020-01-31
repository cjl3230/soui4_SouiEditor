// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

class CMainDlg : public SHostWnd
{
public:
	CMainDlg(LPCTSTR pszLayoutId);
	~CMainDlg();

protected:
	void GetSwndIndex(SWindow *pWnd,SList<int> &lstIndex);

	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);

	SWindow * m_pHover;
protected:
	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_ID_COMMAND(IDCANCEL,OnClose)
		EVENT_ID_COMMAND(IDOK,OnClose)
	EVENT_MAP_END()
		
	LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseEvent)
		CHAIN_MSG_MAP(SHostWnd)
	END_MSG_MAP()
};
