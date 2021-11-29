// CEFFrameDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "CEFFrame.h"
#include "CEFFrameDialog.h"
#include "afxdialogex.h"
#include "simple_app.h"
#include "simple_handler.h"


// CCEFFrameDialog 对话框

IMPLEMENT_DYNAMIC(CCEFFrameDialog, CDialogEx)

CCEFFrameDialog::CCEFFrameDialog(UINT Itemp, CWnd* pParent /*=NULL*/)
	: CDialogEx(Itemp, pParent)
{
	//m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

CCEFFrameDialog::~CCEFFrameDialog()
{
}

void CCEFFrameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCEFFrameDialog, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CCEFFrameDialog 消息处理程序


BOOL CCEFFrameDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	void* sandbox_info = NULL;
	CefMainArgs main_args(AfxGetApp()->m_hInstance);
	CefRefPtr<SimpleApp> app(new SimpleApp);
	CefSettings settings;
	settings.log_severity = LOGSEVERITY_DISABLE;
	settings.no_sandbox = true;							//不要沙箱
	settings.single_process = true;						//采用单进程模式
	settings.multi_threaded_message_loop = true;			//使用主消息循环
	CefInitialize(main_args, settings, app.get(), sandbox_info);
	app->m_pCCEFFrameDialog = this;
	CefWindowInfo window_info;
	CRect rt;
	GetWindowRect(&rt);									//获取对话框大小
	window_info.SetAsChild(m_hWnd, rt);	//设置cef浏览器窗口为对话框的子窗口
	CefRefPtr<SimpleHandler> handler(new SimpleHandler());
	m_simpleClient = handler;
	CefBrowserSettings browser_settings;
	m_simpleClient->m_pCCEFFrameDialog = this;
	CefBrowserHost::CreateBrowser(window_info, handler.get(), m_url, browser_settings, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CCEFFrameDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rt;
	GetWindowRect(&rt);
	if (m_simpleClient.get() != NULL) {
		CefRefPtr<CefBrowser> browser = m_simpleClient->getBrowser();
		if (browser) {
			CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
			::MoveWindow(hwnd, 0, 0, cx, cy, TRUE);
		}
	}
}

bool CCEFFrameDialog::Navigate(CString url)
{
	if (url.IsEmpty())
	{
		return false;
	}
	if (m_simpleClient.get() != NULL) {
		CefRefPtr<CefBrowser> browser = m_simpleClient->getBrowser();
		if (browser)
		{
			if (browser->GetMainFrame())
			{
				browser->GetMainFrame()->LoadURL(CefString(url));
			}
		}
	}
	return true;
}

bool CCEFFrameDialog::RegisterCppFunc(const std::string& function_name, my_cef::CppFunction function, bool global_function/* = false*/)
{
	if (m_simpleClient.get() != NULL) {
		CefRefPtr<CefBrowser> browser = m_simpleClient->getBrowser();
		if (browser)
		{
			return js_bridge_->RegisterCppFunc(function_name.c_str(), function, global_function ? nullptr : browser);
		}
	}

	return false;
}


