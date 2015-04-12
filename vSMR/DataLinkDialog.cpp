// DataLinkDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DataLinkDialog.hpp"
#include "afxdialogex.h"


// CDataLinkDialog dialog

IMPLEMENT_DYNAMIC(CDataLinkDialog, CDialogEx)

CDataLinkDialog::CDataLinkDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDataLinkDialog::IDD, pParent)
	, m_Callsign(_T(""))
	, m_Aircraft(_T(""))
	, m_From(_T(""))
	, m_Dest(_T(""))
	, m_CTOT(_T(""))
	, m_TSAT(_T(""))
	, m_Rwy(_T(""))
	, m_Departure(_T(""))
	, m_SSR(_T(""))
	, m_Freq(_T("121.800"))
	, m_Message(_T(""))
	, m_Req(_T(""))
	, m_Climb(_T(""))
{

}

CDataLinkDialog::~CDataLinkDialog()
{
}

void CDataLinkDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CALLSIGN, m_Callsign);
	DDX_Text(pDX, IDC_ACT, m_Aircraft);
	DDX_Text(pDX, IDC_FROM, m_From);
	DDX_Text(pDX, IDC_DEST, m_Dest);
	DDX_Text(pDX, IDC_CTOT, m_CTOT);
	DDX_Text(pDX, IDC_TSAT, m_TSAT);
	DDX_Text(pDX, IDC_RWY, m_Rwy);
	DDX_Text(pDX, IDC_SID, m_Departure);
	DDX_Text(pDX, IDC_SSR, m_SSR);
	DDX_Text(pDX, IDC_FREQ, m_Freq);
	DDX_Text(pDX, IDC_MESSAGE, m_Message);
	DDX_Text(pDX, IDC_ORIG, m_Req);
	DDX_Text(pDX, IDC_CLB, m_Climb);
}


BEGIN_MESSAGE_MAP(CDataLinkDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDataLinkDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CDataLinkDialog message handlers


void CDataLinkDialog::OnBnClickedOk()
{
	if (m_CTOT.IsEmpty()) 
	{
		m_CTOT = "no";
	}
	if (m_TSAT.IsEmpty()) 
	{
		m_TSAT = "no";
	}
	if (m_Message.IsEmpty())
	{
		m_Message = "no";
	}
	if (m_Freq.IsEmpty())
	{
		m_Freq = "no";
	}

	CDialogEx::OnOK();
}
