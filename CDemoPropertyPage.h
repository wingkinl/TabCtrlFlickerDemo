#pragma once


// CDemoPropertyPage

class CDemoPropertyPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CDemoPropertyPage)

public:
	CDemoPropertyPage();
	virtual ~CDemoPropertyPage();

protected:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	DECLARE_MESSAGE_MAP()
};


