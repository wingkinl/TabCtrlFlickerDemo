// CDemoPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "NewControls.h"
#include "CDemoPropertyPage.h"


// CDemoPropertyPage

IMPLEMENT_DYNAMIC(CDemoPropertyPage, CMFCPropertyPage)

CDemoPropertyPage::CDemoPropertyPage()
{

}

CDemoPropertyPage::~CDemoPropertyPage()
{
}


BEGIN_MESSAGE_MAP(CDemoPropertyPage, CMFCPropertyPage)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



// CDemoPropertyPage message handlers

void CDemoPropertyPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CMFCPropertyPage::OnShowWindow(bShow, nStatus);
}


