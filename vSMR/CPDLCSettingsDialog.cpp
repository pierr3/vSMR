// CPDLCSettingsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CPDLCSettingsDialog.hpp"
#include "afxdialogex.h"


// CCPDLCSettingsDialog dialog

IMPLEMENT_DYNAMIC(CCPDLCSettingsDialog, CDialogEx)

CCPDLCSettingsDialog::CCPDLCSettingsDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCPDLCSettingsDialog::IDD, pParent)
	, m_Logon(_T("EGKK"))
	, m_Password(_T("PASSWORD"))
	, m_Sound(1)
{

}

CCPDLCSettingsDialog::~CCPDLCSettingsDialog()
{
}

void CCPDLCSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LOGON, m_Logon);
	DDX_Text(pDX, IDC_PASSWORD, m_Password);
	DDX_Check(pDX, IDC_SOUND, m_Sound);
}


BEGIN_MESSAGE_MAP(CCPDLCSettingsDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCPDLCSettingsDialog::OnBnClickedOk)
END_MESSAGE_MAP()

void CCPDLCSettingsDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
