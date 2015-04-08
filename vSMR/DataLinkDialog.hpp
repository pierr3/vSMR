#pragma once


// CDataLinkDialog dialog

#include "resource.h"

class CDataLinkDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CDataLinkDialog)

public:
	CDataLinkDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDataLinkDialog();

	CString m_Callsign;
	CString m_Aircraft;
	CString m_From;
	CString m_Dest;
	CString m_CTOT;
	CString m_TSAT;
	CString m_Rwy;
	CString m_Departure;
	CString m_SSR;
	CString m_Freq;
	CString m_Message;
	CString m_Req;
	CString m_Climb;

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
