#include "stdafx.h"
#include "ChatCppDlg.h"
#include "Sock.h"




CSock::CSock()
: m_pParent(NULL)
{
	
}


CSock::~CSock()
{
}

// Событие подключения на стороне клиентского приложения.
void CSock::OnConnect(int nErrorCode)
{
	CChatCppDlg* pDlg = (CChatCppDlg*)m_pParent;


	nErrorCode == 0 ? pDlg->OnConnect(FALSE) : pDlg->OnConnect(TRUE);
	

	CAsyncSocket::OnConnect(nErrorCode);
}

// Событие отключения от сети
void CSock::OnClose(int nErrorCode)
{
}

// Событие возможности получения сообщений.
void CSock::OnReceive(int nErrorCode)
{

	CChatCppDlg* pDlg = (CChatCppDlg*)m_pParent;
	pDlg->OnReceive();
	
	CAsyncSocket::OnReceive(nErrorCode);
}

