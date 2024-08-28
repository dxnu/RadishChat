#include "LoginWnd.h"
#include <boost/asio.hpp>
#include <chrono>
#include <thread>
#include <boost/lexical_cast.hpp>
#include <memory>
#include "StringConvert.h"
#include "ChatMessage.h"

using namespace Utils;


LoginWnd::LoginWnd()
{
}


LoginWnd::~LoginWnd()
{
	m_bFlag = true;  // 安全退出线程
}

void LoginWnd::InitWindow()
{
	// 登录注册TabLayout
	m_tabLoginSignup = static_cast<CTabLayoutUI *>(m_PaintManager.FindControl(L"tab_login_signup"));

	// 展示页
	m_vlShowPages = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(L"vl_show_pages"));
	m_btnPage1 = static_cast<CButtonUI *>(m_PaintManager.FindControl(L"btn_page1"));
	m_btnPage2 = static_cast<CButtonUI *>(m_PaintManager.FindControl(L"btn_page2"));
	m_btnPage3 = static_cast<CButtonUI *>(m_PaintManager.FindControl(L"btn_page3"));
	m_btnPage4 = static_cast<CButtonUI *>(m_PaintManager.FindControl(L"btn_page4"));
	
	std::thread(&LoginWnd::SlidePages, this).detach();
	std::thread(&LoginWnd::ConnectServer, this, "127.0.0.1", 8016).detach();

	// 登录
	m_btnLogin = static_cast<CButtonUI *>(m_PaintManager.FindControl(L"btn_login"));
	m_ClickHandler.AddHandler(m_btnLogin, std::bind(&LoginWnd::OnClickLogin, this, std::placeholders::_1));

	// 登录的账号
	m_etLoginAccount = static_cast<CEditUI *>(m_PaintManager.FindControl(L"et_login_account"));

	// 登录的密码
	m_etLoginPassword = static_cast<CEditUI *>(m_PaintManager.FindControl(L"et_login_password"));

	// 找回密码
	m_btnRetrievePsw = static_cast<CButtonUI *>(m_PaintManager.FindControl(L"btn_retrieve_password"));
	m_ClickHandler.AddHandler(m_btnRetrievePsw, std::bind(&LoginWnd::OnClickRetrievePsw, this, std::placeholders::_1));

	// 注册账号
	m_btnToSignup = static_cast<CButtonUI *>(m_PaintManager.FindControl(L"btn_to_signup"));
	m_ClickHandler.AddHandler(m_btnToSignup, std::bind(&LoginWnd::OnClickToSignup, this, std::placeholders::_1));
	
	// 注册
	m_btnSignup = static_cast<CButtonUI *>(m_PaintManager.FindControl(L"btn_signup"));
	m_ClickHandler.AddHandler(m_btnSignup, std::bind(&LoginWnd::OnClickSignup, this, std::placeholders::_1));

}

void LoginWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == L"click")
	{
		if (msg.pSender->GetName() == L"btn_min")
		{
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		}
		else if (msg.pSender->GetName() == L"btn_close")
		{
			this->Close();
		}
		else
		{
			m_ClickHandler.Dispatch(&msg);
		}
	}
}

LRESULT LoginWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCLBUTTONDBLCLK)
		return 0;

	return __super::HandleMessage(uMsg, wParam, lParam);
}

void LoginWnd::ConnectServer(const char* addr, uint16_t port)
{
	// 连接服务器
	m_client = std::make_shared<RadishClient>();
	m_client->Connect(addr, port);

	// 添加消息处理回调函数
	m_client->AddMessageHandler("vrlogin", std::bind(&LoginWnd::OnVRLoginMessage, this, std::placeholders::_1));
	m_client->AddMessageHandler("vrsignup", std::bind(&LoginWnd::OnVRSignupMessage, this, std::placeholders::_1));

	m_client->Run();
}

DuiLib::CDuiString LoginWnd::GetSkinFile()
{
	return L"login/main_login.xml";
}

LPCTSTR LoginWnd::GetWindowClassName(void) const
{
	return L"LoginWnd";
}

// 循环显示展示页面
void LoginWnd::SlidePages()
{
	int i = 2;

	for (;;)
	{
		if (m_bFlag)
			break;

		boost::asio::io_context io_context;
		// 创建一个定时器
		boost::asio::steady_timer st(io_context, std::chrono::milliseconds(3000));

		st.async_wait([this, &i](const boost::system::error_code& ec) {
			CDuiString dstrImages;
			dstrImages.Format(L"images/show/show-page%d%s", i, L".jpg");
			m_vlShowPages->SetBkImage(dstrImages.GetData());

			// 设置当前页面状态
			SetCurrentPageStatus(i);

			i = i >= 4 ? 1 : i + 1;			
		});

		io_context.run();
	}
}

// 设置展示页按钮状态
void LoginWnd::SetCurrentPageStatus(int n)
{
	enum { gen_width = 10, sel_width = 15 };
	enum { gen_color = 0xFF00FF00, sel_color = 0xFFFF0000 };

	switch (n)
	{
	case 1:
		m_btnPage1->SetBkColor(sel_color);
		m_btnPage1->SetFixedWidth(sel_width);

		m_btnPage2->SetBkColor(gen_color);
		m_btnPage2->SetFixedWidth(gen_width);
		
		m_btnPage3->SetBkColor(gen_color);
		m_btnPage3->SetFixedWidth(gen_width);
		
		m_btnPage4->SetBkColor(gen_color);
		m_btnPage4->SetFixedWidth(gen_width);
		break;
	case 2:
		m_btnPage1->SetBkColor(gen_color);
		m_btnPage1->SetFixedWidth(gen_width);

		m_btnPage2->SetBkColor(sel_color);
		m_btnPage2->SetFixedWidth(sel_width);

		m_btnPage3->SetBkColor(gen_color);
		m_btnPage3->SetFixedWidth(gen_width);

		m_btnPage4->SetBkColor(gen_color);
		m_btnPage4->SetFixedWidth(gen_width);
		break;
	case 3:
		m_btnPage1->SetBkColor(gen_color);
		m_btnPage1->SetFixedWidth(gen_width);

		m_btnPage2->SetBkColor(gen_color);
		m_btnPage2->SetFixedWidth(gen_width);

		m_btnPage3->SetBkColor(sel_color);
		m_btnPage3->SetFixedWidth(sel_width);

		m_btnPage4->SetBkColor(gen_color);
		m_btnPage4->SetFixedWidth(gen_width);
		break;
	case 4:
		m_btnPage1->SetBkColor(gen_color);
		m_btnPage1->SetFixedWidth(gen_width);

		m_btnPage2->SetBkColor(gen_color);
		m_btnPage2->SetFixedWidth(gen_width);

		m_btnPage3->SetBkColor(gen_color);
		m_btnPage3->SetFixedWidth(gen_width);

		m_btnPage4->SetBkColor(sel_color);
		m_btnPage4->SetFixedWidth(sel_width);
		break;
	}
}

// 登录按钮事件
void LoginWnd::OnClickLogin(TNotifyUI* pObj)
{
	// 获取输入的用户名与密码
	std::string fromUserId = string_cast(m_etLoginAccount->GetText().GetData());
	std::string password = string_cast(m_etLoginPassword->GetText().GetData());

	try {
		// 设置消息
		ChatMessage sendMsg;
		sendMsg.SetTextMessage("", fromUserId, "login", password);

		// 发送登录信息
		m_client->AsyncWriteMessage(sendMsg);
	}
	catch (const std::exception& e)
	{
		MessageBox(m_hWnd, LPCTSTR_CAST(e.what()), L"Error", MB_OK | MB_ICONWARNING);
	}
}

// 注册按钮事件
void LoginWnd::OnClickSignup(TNotifyUI* pObj)
{
	CEditUI* etId = static_cast<CEditUI*>(m_PaintManager.FindControl(L"et_signup_id"));
	CEditUI* etPsw = static_cast<CEditUI*>(m_PaintManager.FindControl(L"et_signup_password"));
	CEditUI* etVerifyPsw = static_cast<CEditUI *>(m_PaintManager.FindControl(L"et_signup_repassword"));
	CEditUI* etEmail = static_cast<CEditUI*>(m_PaintManager.FindControl(L"et_signup_email"));
	CEditUI* etName = static_cast<CEditUI*>(m_PaintManager.FindControl(L"et_signup_name"));
	CEditUI* etSignature = static_cast<CEditUI*>(m_PaintManager.FindControl(L"et_signup_signature"));
	
	std::string fromUserId = STRING_CAST(etId->GetText().GetData());
	std::string fromUserName = STRING_CAST(etName->GetText().GetData());
	std::string psw = STRING_CAST(etPsw->GetText().GetData());
	std::string verifyPsw = STRING_CAST(etVerifyPsw->GetText().GetData());
	std::string email = STRING_CAST(etEmail->GetText().GetData());
	std::string signature = STRING_CAST(etSignature->GetText().GetData());

	try {
		if(psw != verifyPsw)
			throw std::runtime_error("两次输入的密码不一致");

		ChatMessage sendMsg;
		sendMsg.SetTextMessage("", fromUserId, "signup", psw);
		sendMsg.SetProperty("Email", email);
		sendMsg.SetProperty("FromUserName", fromUserName);
		sendMsg.SetProperty("Signature", signature);

		// 发送注册信息
		m_client->AsyncWriteMessage(sendMsg);
	}
	catch (const std::exception& e)
	{
		MessageBox(m_hWnd, LPCTSTR_CAST(e.what()), L"Error", MB_OK | MB_ICONWARNING);
	}
}

// 点击免费注册事件
void LoginWnd::OnClickToSignup(TNotifyUI* pObj)
{
	m_tabLoginSignup->SelectItem(1);
}

// 找回密码事件
void LoginWnd::OnClickRetrievePsw(TNotifyUI* pObj)
{
}

void LoginWnd::OnVRLoginMessage(const ChatMessage& msg)
{
	if (msg.GetProperty<int>("ErrCode") == 0) // 验证成功
	{
		//// 隐藏当前窗口
		this->ShowWindow(false);

		//// 显示聊天窗口
		std::string fromUserId = msg.GetProperty("FromUserId");
		std::string fromUserName = msg.GetProperty("FromUserName");

		// 创建聊天窗口
		auto chatWnd = std::make_unique<ChatWnd>(m_client);
		chatWnd->SetParam(fromUserId, fromUserName);
		chatWnd->Create(nullptr, L"ChatWnd", UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
		chatWnd->CenterWindow();
		chatWnd->ShowModal();
		this->Close();
	}
	else
	{
		// 验证失败
		MessageBox(m_hWnd, LPCTSTR_CAST(msg.GetProperty("ErrMsg")), L"Error", MB_OK | MB_ICONWARNING);
	}
}

void LoginWnd::OnVRSignupMessage(const ChatMessage& msg)
{
	// 接收验证信息
	if (msg.GetProperty<int>("ErrCode") == 0)
	{
		// 注册成功，跳转到登录界面
		m_tabLoginSignup->SelectItem(0);
	}
	else
	{
		// 注册失败
		MessageBox(m_hWnd, LPCTSTR_CAST(msg.GetProperty("ErrMsg")), L"Info", MB_OK);
	}

}

