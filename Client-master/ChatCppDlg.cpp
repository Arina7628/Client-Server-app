// ChatCppDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChatCppDlg.h"
#include ".\chatcppdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

bool access = true;

CChatCppDlg::CChatCppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatCppDlg::IDD, pParent)
	, password(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatCppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_CLIENT, m_ButtonStartClient);
	DDX_Control(pDX, IDC_EDIT_NAME, m_wndName);
	DDX_Control(pDX, IDC_EDIT_IP, m_wndIPAddress);
	DDX_Control(pDX, IDC_EDIT_PORT, m_wndPort);
	DDX_Control(pDX, IDC_EDIT_SEND, m_wndSend);
	DDX_Control(pDX, IDC_BUTTON_SEND, m_ButtonSend);
	DDX_Control(pDX, IDC_BUTTON_STOPCHAT, m_ButtonStopChat);
	DDX_Control(pDX, IDC_EDIT_CHAT, m_wndChat);
	DDX_Text(pDX, IDC_EDIT1, password);
}

BEGIN_MESSAGE_MAP(CChatCppDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_CLIENT, OnBnClickedRadioClient)
	ON_BN_CLICKED(IDC_BUTTON_STOPCHAT, OnBnClickedButtonStopchat)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnBnClickedButtonSend)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

int key = 0;

BOOL CChatCppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	
	//////////////////////////////////////////////////////////////////////////////
	// Необходимая инициализация
	SetWindowText("Нет сети!");

	m_wndName.SetLimitText(12); // Ограничение количества вводимых символов.
	
	m_wndSend.SetLimitText(200); // Ограничение количества вводимых символов.
	m_wndIPAddress.SetWindowText("127.0.0.1"); // Адрес по умолчанию для тестирования.
	m_wndPort.SetWindowText("8000");
	m_ButtonStopChat.SetWindowText(g_strExitFromChat);
	m_mainSocket.m_pParent = this;

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChatCppDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

CString encode(CString s)
{
	std::string s1((LPCSTR)s);
	std::string out;
	for (unsigned int i = 0;i < s1.length();i++)
		out += (char)(s[i] + 120) % 125 + 12;
	return CString(out.c_str());
}


// Запуск клиента
void CChatCppDlg::OnBnClickedRadioClient()
{
	UpdateData(true);
	CString S = "fds";
	CString N = "ssdf";
	if (encode(password) != S+N)
	{
		AfxMessageBox(_T("Error password"));
		return;
	}
	// Контроль несанкционированного запуска клиента:
	// Если кнопка не в состоянии нажатой,
	// если сокет в работе (т.е. только с нулевым сокетом можно начинать работать),
	// очень неудобно если в чате все будут под одинаковыми именами..
	if(m_ButtonStartClient.GetCheck() != BST_CHECKED) return;
	if(m_mainSocket.m_hSocket != INVALID_SOCKET) return;
	if(QueryName() == false)
	{
		AfxMessageBox("Введите свое имя для чата!");
		StopChat();
		return;
	}


	// Класс CAsyncSocket упрощает процедуру создания сокета,
	// вкладывая в функцию Create() непосредственное создание 
	// сокета и связывание его с одним из IP адресом доступном на компьютере.
	if(m_mainSocket.Create() == TRUE)
	{
		CString strAddress;
		m_wndIPAddress.GetWindowText(strAddress);
		CString strPort;
		m_wndPort.GetWindowText(strPort);
		if(m_mainSocket.Connect(strAddress, atoi(strPort)) == FALSE)
		{
			// В ассинхронном режиме код этой ошибки
			// считается как ожидание события подключения,
			// т.е. практически успешный возврат.
			if(GetLastError() == WSAEWOULDBLOCK)
			{
				DisabledControl(false);
			}
			else
			{
				// Если какая-либо ошибка возникла,
				// приводим приложение в первоначальное состояние,
				// готовым к следующей попытке создания соединения.
				StopChat();
			}
		}
	}
}


// Нажали кнопку "Выйти из чата".
void CChatCppDlg::OnBnClickedButtonStopchat()
{
	StopChat();
}

// Запрещает доступ к управлениям при работе 
// приложения в режиме сервера или клиента.
// Цель запрета - избежать исключения от 
// случайного нажатия "неправильных" кнопок.
void CChatCppDlg::DisabledControl(bool server)
{
	// Запреты.
	m_wndIPAddress.EnableWindow(FALSE);
	m_wndPort.EnableWindow(FALSE);
	m_ButtonSend.EnableWindow(FALSE);

	if(server == true)
	{
		m_ButtonStopChat.SetWindowText(g_strStopChat);
		m_ButtonStartClient.EnableWindow(TRUE);
	}
	else
	{
		m_ButtonStopChat.SetWindowText(g_strExitFromChat);
		//m_ButtonStartServer.EnableWindow(FALSE);
	}

	// Разрешения.
	// Разрешить возможность выхода из чата.
	m_ButtonStopChat.EnableWindow(TRUE);
}

// Разрешить доступ к управлениям после закрытия сокетов.
// Цель запрета - избежать исключения от 
// случайного нажатия "неправильных" кнопок.
void CChatCppDlg::EnabledControl(void)
{
	// Разрешения.
	m_wndIPAddress.EnableWindow(TRUE);
	m_wndPort.EnableWindow(TRUE);
	m_ButtonStartClient.EnableWindow(TRUE);
	//m_ButtonStartServer.EnableWindow(TRUE);
	

	// Запреты.
	m_ButtonStopChat.EnableWindow(FALSE);
	m_ButtonSend.EnableWindow(FALSE);
}


// Выход из чата,
// если это сработало на стороне сервера,
// то это полная остановка чата.
// Для более надежной работы чата, во всех
// приложениях должны быть запущеня дублирующие серверы...
void CChatCppDlg::StopChat(void)
{
	// Отсылаем сигнал об отключении от чата.
	SendDisconnect();

	m_mainSocket.Close();
	// Очистим вектор от ненужных элементов.
	m_vecSockets.clear();

	//m_ButtonStartServer.SetCheck(BST_UNCHECKED);
	m_ButtonStartClient.SetCheck(BST_UNCHECKED);

	// Разрешим доступ к управлению для
	// повторных попыток.
	EnabledControl();

	// В чате нет никого.
	SetWindowText("Нет сети!");
}


// Отправка подготовленного сообщения.
void CChatCppDlg::OnBnClickedButtonSend()
{
	CString strChat;
	m_wndSend.GetWindowText(strChat);
	SendChat(strChat);

}

// Извлечение сообщений из сети чата.
void CChatCppDlg::OnReceive(void)
{
	SENDBUFFER sb;
	if(m_ButtonStartClient.GetCheck() == BST_CHECKED)
	{
		m_mainSocket.Receive(&sb, sizeof(SENDBUFFER));
	}

	// Обработка принятого сообщения на основе
	// его типа.
	switch(sb.typemessage)
	{
	case m_TypeMessage::tmChat:
		{
			CString strChat;
			m_wndChat.GetWindowText(strChat);
			strChat += "    " + CString(sb.name) + ": " + CString(sb.buffer) + "\r\n";
			m_wndChat.SetWindowText(strChat);
			int number_line = m_wndChat.GetLineCount();
			m_wndChat.LineScroll(number_line);
		}
		break;
	case m_TypeMessage::tmDisconnect:
		{
			CString strChat;
			m_wndChat.GetWindowText(strChat);
			// Если принято сообщение об остановки чата(отключении сервера),
			// изменим содержимое сообщения.
			strChat += "    " + CString(sb.name) + " - покинул(а) чат!" + "\r\n";
			m_wndChat.SetWindowText(strChat);
			int number_line = m_wndChat.GetLineCount();
			m_wndChat.LineScroll(number_line);
		}
		break;
	}
}

// При закрытии приложения отправим в чат
// информацию об отключении чатующего.
void CChatCppDlg::OnClose()
{
	StopChat();
	CDialog::OnClose();
}
// Послать строку-сообщение в чат.
void CChatCppDlg::SendChat(CString strMessage)
{
	SENDBUFFER sb;
	int len = strMessage.GetLength();
	//strMessage = encod(std::string((LPCTSTR)strMessage), len);
	memcpy(sb.buffer, strMessage.GetBuffer(), sizeof(TCHAR)*len);
	m_wndName.GetWindowText(strMessage);
	len = strMessage.GetLength();
	memcpy(sb.name, strMessage.GetBuffer(), sizeof(TCHAR)*len);
	sb.typemessage = m_TypeMessage::tmChat;
	SendBuffer(sb, true);
}

// Послать буфер подготовленного сообщения в сеть.
void CChatCppDlg::SendBuffer(SENDBUFFER sb, bool toserver)
{
	// Если сокет не создан, нечего делать в этой функции.
	if(m_mainSocket.m_hSocket == INVALID_SOCKET) return;
	if(m_ButtonStartClient.GetCheck() == BST_CHECKED)
	{
		int send = m_mainSocket.Send(&sb, sizeof(SENDBUFFER));
		if(send == sizeof(SENDBUFFER))
			m_wndSend.SetWindowText("");
	}
}


// Формируем и отправляем сообщение об отключении от сети.
void CChatCppDlg::SendDisconnect(void)
{
	SENDBUFFER sb;
	CString s;
	m_wndName.GetWindowText(s);
	int len = s.GetLength();
	memcpy(sb.name, s.GetBuffer(), sizeof(TCHAR)*len);
	sb.typemessage = m_TypeMessage::tmDisconnect;
	SendBuffer(sb, true);

}

// Событие подключения, происходит на стороне клиента.
void CChatCppDlg::OnConnect(BOOL Error)
{
	if(Error == TRUE)
	{
		AfxMessageBox("Попытка подключения была отвергнута!\nВозможно сервер еще не создан!");
		StopChat();
	}
	else
	{
		m_ButtonSend.EnableWindow(TRUE);
		SetWindowText("Сеть работает!");
		TCHAR key_out[1];
		ZeroMemory(key_out, sizeof(TCHAR));
		m_mainSocket.Receive(&key_out, sizeof(key_out));
		CString str = key_out;
		key = atoi(str);
	}
}

// Запрос имени чатующего перед созданием сокета.
bool CChatCppDlg::QueryName(void)
{
	CString strName;
	m_wndName.GetWindowText(strName);
	if(strName == g_EmptyName || strName.IsEmpty() == true)
		return false;

	return true;
}
