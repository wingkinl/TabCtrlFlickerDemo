// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

#include "Page1.h"
#include "Page2.h"
#include "Page3.h"
#include "Page4.h"
#include "Page5.h"
#include "Page6.h"

/////////////////////////////////////////////////////////////////////////////
// NewControlsPropSheet

class NewControlsPropSheet : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(NewControlsPropSheet)

// Construction
public:
	NewControlsPropSheet(CWnd* pParentWnd = NULL);

// Attributes
public:
	CPage1 m_Page1;
	CPage2 m_Page2;
	CPage3 m_Page3;
	CPage4 m_Page4;
	CPage5 m_Page5;
	CPage6 m_Page6;

	HICON m_hIcon;

// Overrides
public:
	virtual BOOL OnInitDialog();

	void OnActivatePage(CPropertyPage* pPage) override;

	void RepositionControls(CSize szDiff);
	void ResizeToFitPage(CPropertyPage* pPage);
	CSize CalcPageCompactSize(CPropertyPage* pPage) const;

// Implementation
public:
	virtual ~NewControlsPropSheet();
protected:
	CSize m_szClient;
	CRect m_TabMargin;

	BOOL m_bAutoRepositionOnSize;
	BOOL m_bAutoRepositionOnSwitchPage;
	BOOL m_bRepositionReuseSizingRoutine;

protected:
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnCommandResizeToFitCurrentPage(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnResizeToFitCurrentPage(WPARAM wp, LPARAM lp);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

