// About.cpp: файл реализации
//

#include "stdafx.h"
#include "ChatCpp.h"
#include "About.h"
#include "ChatCppDlg.h"
#include "afxdialogex.h"

extern bool access;

// диалоговое окно CAbout

IMPLEMENT_DYNAMIC(CAbout, CDialogEx)

CAbout::CAbout(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, password(_T(""))
{

}

CAbout::~CAbout()
{
}

void CAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, password);
	DDX_Text(pDX, IDC_EDIT1, password);
}


BEGIN_MESSAGE_MAP(CAbout, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CAbout::OnBnClickedButton1)
END_MESSAGE_MAP()


// обработчики сообщений CAbout


void CAbout::OnBnClickedButton1()
{
	UpdateData(true);
	CString pass = password;
	if (pass == "qwerty")
	{
		CDialog::OnOK();
		access = false;
	}
	else
	{
	}
}