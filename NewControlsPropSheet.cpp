// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "NewControls.h"
#include "NewControlsPropSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NewControlsPropSheet

IMPLEMENT_DYNAMIC(NewControlsPropSheet, CMFCPropertySheet)

NewControlsPropSheet::NewControlsPropSheet(CWnd* pParentWnd)
:CMFCPropertySheet(IDS_CAPTION, pParentWnd)
{
	BOOL b32BitIcons = TRUE;

	if (afxGlobalData.m_nBitsPerPixel < 16)
	{
		b32BitIcons = FALSE;
	}

	//SetLook(CMFCPropertySheet::PropSheetLook_OutlookBar);
	//SetIconsList(b32BitIcons ? IDB_ICONS32 : IDB_ICONS, 32);

	AddPage(&m_Page1);
	AddPage(&m_Page2);
	AddPage(&m_Page3);
	AddPage(&m_Page4);
	AddPage(&m_Page5);
	AddPage(&m_Page6);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_szClient = CSize(0, 0);
	m_bAutoRepositionOnSize = FALSE;
	m_bAutoRepositionOnSwitchPage = FALSE;
	m_bDelayRepositionOnSwitchPage = FALSE;
	m_bRepositionReuseSizingRoutine = FALSE;
	m_nTimerDelayReposition = 0;
	m_szPage = CSize(0, 0);
	m_pResizePage = nullptr;
}

NewControlsPropSheet::~NewControlsPropSheet()
{
}

#define WM_APP_RESIZE_TO_FIT_PAGE		(WM_APP+0x100)
#define TIMERID_DELAYED_SIZING			0x9999

BEGIN_MESSAGE_MAP(NewControlsPropSheet, CMFCPropertySheet)
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_COMMANDHELP, &NewControlsPropSheet::OnCommandResizeToFitCurrentPage)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_APP_RESIZE_TO_FIT_PAGE, &NewControlsPropSheet::OnResizeToFitCurrentPage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NewControlsPropSheet message handlers

BOOL NewControlsPropSheet::OnInitDialog()
{
	// force single line tab
	EnableStackedTabs(FALSE);

	BOOL bResult = CMFCPropertySheet::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bValidString;
		CString strAboutMenu;
		bValidString = strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE); // Set big icon
	SetIcon(m_hIcon, FALSE); // Set small icon

	SetDlgItemText(IDHELP, _T("Fit"));
	
	CRect rectClient;
	GetClientRect(rectClient);
	m_szClient = rectClient.Size();

	CRect rectTab;
	GetTabControl()->GetWindowRect(rectTab);
	ScreenToClient(rectTab);
	m_TabMargin.left = rectTab.left - rectClient.left;
	m_TabMargin.top = rectTab.top - rectClient.top;
	m_TabMargin.right = rectTab.right - rectClient.right;
	m_TabMargin.bottom = rectTab.bottom - rectClient.bottom;

	m_bAutoRepositionOnSize = TRUE;
	m_bAutoRepositionOnSwitchPage = TRUE;
	return bResult;
}

HCURSOR NewControlsPropSheet::OnQueryDragIcon()
{
	return(HCURSOR) m_hIcon;
}

void NewControlsPropSheet::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg aboutDlg(this);
		aboutDlg.DoModal();
	}
	else
	{
		CMFCPropertySheet::OnSysCommand(nID, lParam);
	}
}

int NewControlsPropSheet::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	// make it resizable
	ModifyStyle(DS_MODALFRAME, WS_THICKFRAME);
	return 0;
}

CSize NewControlsPropSheet::CalcPageCompactSize(CPropertyPage* pPage) const
{
	ASSERT_VALID(pPage);
	CRect rectClient;
	pPage->GetClientRect(rectClient);
	CRect rectBound; rectBound.SetRectEmpty();
	CWnd* pWndChild = pPage->GetWindow(GW_CHILD);
	while (pWndChild != NULL)
	{
		ASSERT_VALID(pWndChild);
		if (pWndChild->GetStyle() & WS_VISIBLE)
		{
			CRect rect;
			pWndChild->GetWindowRect(rect);
			pPage->ScreenToClient(rect);
			if (rectBound.IsRectEmpty())
			{
				rectBound = rect;
			}
			else
			{
				CRect rectTemp = rectBound;
				rectBound.UnionRect(&rectTemp, rect);
			}
		}
		pWndChild = pWndChild->GetNextWindow();
	}
	// use the top-left bound as the gap
	auto ptGap = rectBound.TopLeft();
	if (ptGap.x <= 0)
		ptGap.x = 5;
	if (ptGap.y <= 0)
		ptGap.y = 5;
	CSize szCompact(0, 0);

	szCompact.cx = rectBound.Width() + ptGap.x * 2;
	szCompact.cy = rectBound.Height() + ptGap.y * 2;
	return szCompact;
}

inline void TryDeferWindowPos(HDWP& hDWP, CWnd* pWnd, int x, int y, int cx, int cy, UINT nFlags)
{
	if (hDWP)
		hDWP = DeferWindowPos(hDWP, pWnd->GetSafeHwnd(), nullptr, x, y, cx, cy, nFlags);
	else
		pWnd->SetWindowPos(nullptr, x, y, cx, cy, nFlags);
}

inline void TryDeferWindowPos(HDWP& hDWP, CWnd* pWnd, const CRect& rect, UINT nFlags)
{
	TryDeferWindowPos(hDWP, pWnd, rect.left, rect.top, rect.Width(), rect.Height(), nFlags);
}

void TryDeferWindowPos(HDWP& hDWP, CWnd* pWnd, CWnd* pParent, CSize szDiff, BOOL bResize)
{
	CRect rect;
	pWnd->GetWindowRect(rect);
	pParent->ScreenToClient(rect);
	UINT nFlags = SWP_NOACTIVATE | SWP_NOZORDER;
	if (bResize)
	{
		rect.right += szDiff.cx;
		rect.bottom += szDiff.cy;
		nFlags |= SWP_NOMOVE;
	}
	else
	{
		rect.OffsetRect(szDiff);
		nFlags |= SWP_NOSIZE;
	}
	TryDeferWindowPos(hDWP, pWnd, rect, nFlags);
}

void NewControlsPropSheet::ResizeToFitPage(CPropertyPage* pPage)
{
	CSize szPage = CalcPageCompactSize(pPage);
	CTabCtrl* pTabCtrl = GetTabControl();
	CRect rectPage(CPoint(0, 0), szPage);
	CRect rectTab = rectPage;
	pTabCtrl->AdjustRect(TRUE, &rectTab);

	CRect rectDlg;
	rectDlg.left = rectTab.left - m_TabMargin.left;
	rectDlg.top = rectTab.top - m_TabMargin.top;
	rectDlg.right = rectTab.right - m_TabMargin.right;
	rectDlg.bottom = rectTab.bottom - m_TabMargin.bottom;

	UINT nFlags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE;
	if (m_bRepositionReuseSizingRoutine)
	{
		AdjustWindowRect(&rectDlg, GetStyle(), FALSE);
		m_szPage = szPage;
		m_pResizePage = pPage;
		SetWindowPos(nullptr, -1, -1, rectDlg.Width(), rectDlg.Height(), nFlags);
		m_pResizePage = nullptr;
		m_szPage = CSize(0, 0);
	}
	else
	{
		AdjustWindowRect(&rectDlg, GetStyle(), FALSE);

		CRect rectDlgCur;
		GetWindowRect(rectDlgCur);
		CSize szDiff = rectDlg.Size() - rectDlgCur.Size();
		RepositionControls(pPage, szDiff, &szPage);

		BOOL bOldAutoSize = m_bAutoRepositionOnSize;
		m_bAutoRepositionOnSize = FALSE;

		SetWindowPos(nullptr, -1, -1, rectDlg.Width(), rectDlg.Height(), nFlags);

		m_bAutoRepositionOnSize = bOldAutoSize;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	m_szClient = rectClient.Size();
}

void NewControlsPropSheet::OnActivatePage(CPropertyPage* pPage)
{
	CMFCPropertySheet::OnActivatePage(pPage);
	if (m_bAutoRepositionOnSwitchPage)
	{
		if (m_bDelayRepositionOnSwitchPage)
		{
			// use low priority WM_TIMER
			m_nTimerDelayReposition = SetTimer(TIMERID_DELAYED_SIZING, 10, nullptr);
			//PostMessage(WM_APP_RESIZE_TO_FIT_PAGE);
		}
		else
		{
			ResizeToFitPage(pPage);
		}
	}
}

void NewControlsPropSheet::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nTimerDelayReposition)
	{
		KillTimer(m_nTimerDelayReposition);
		m_nTimerDelayReposition = 0;
		ResizeToFitPage(GetActivePage());
	}
	CMFCPropertySheet::OnTimer(nIDEvent);
}

LRESULT NewControlsPropSheet::OnCommandResizeToFitCurrentPage(WPARAM, LPARAM)
{
	CPoint ptCursor;
	GetCursorPos(&ptCursor);
	CRect rectButton;
	CWnd* pButton = GetDlgItem(IDHELP);
	pButton->GetWindowRect(rectButton);
	CSize ptOffset = ptCursor - rectButton.TopLeft();

	ResizeToFitPage(GetActivePage());

	pButton->GetWindowRect(rectButton);
	ptCursor = rectButton.TopLeft() + ptOffset;
	SetCursorPos(ptCursor.x, ptCursor.y);

	return 0;
}

LRESULT NewControlsPropSheet::OnResizeToFitCurrentPage(WPARAM, LPARAM)
{
	ResizeToFitPage(GetActivePage());
	return 0;
}

void NewControlsPropSheet::RepositionControls(CPropertyPage* pPage, CSize szDiff, CSize* pszPage)
{
	HDWP hDWP = BeginDeferWindowPos(5);

	TryDeferWindowPos(hDWP, GetTabControl(), this, szDiff, TRUE);
	if (pszPage)
		TryDeferWindowPos(hDWP, pPage, -1,-1, pszPage->cx, pszPage->cy, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE);
	else
		TryDeferWindowPos(hDWP, pPage, this, szDiff, TRUE);
	TryDeferWindowPos(hDWP, GetDlgItem(IDOK), this, szDiff, FALSE);
	TryDeferWindowPos(hDWP, GetDlgItem(IDCANCEL), this, szDiff, FALSE);
	TryDeferWindowPos(hDWP, GetDlgItem(ID_APPLY_NOW), this, szDiff, FALSE);
	TryDeferWindowPos(hDWP, GetDlgItem(IDHELP), this, szDiff, FALSE);

	if (hDWP)
		EndDeferWindowPos(hDWP);
}

void NewControlsPropSheet::OnSize(UINT nType, int cx, int cy)
{
	CMFCPropertySheet::OnSize(nType, cx, cy);
	if (!m_bAutoRepositionOnSize)
		return;
	CSize szDiff = CSize(cx, cy) - m_szClient;
	
	CSize* pszPage = m_szPage.cx != 0 ? &m_szPage : nullptr;
	auto pPage = m_pResizePage ? m_pResizePage : GetActivePage();
	RepositionControls(pPage, szDiff, pszPage);

	CRect rectClient;
	GetClientRect(rectClient);
	m_szClient = rectClient.Size();
}

