#pragma once
#include "Common.h"
#include "RadishClient.h"
#include "EncryptUtils.h"
#include "ChatWnd.h"
#include "UIEventHandler.h"



using namespace Radish::Chat;
using namespace Radish::Network;


class LoginWnd : public WindowImplBase
{
public:
	LoginWnd();
	~LoginWnd();

	virtual void InitWindow() override;
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	// 连接服务器
	void ConnectServer(const char* addr, uint16_t port);

protected:
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;

private:
	void SlidePages();
	void SetCurrentPageStatus(int n);

private:
	void OnClickLogin(TNotifyUI* pObj);
	void OnClickSignup(TNotifyUI* pObj);
	void OnClickToSignup(TNotifyUI* pObj);
	void OnClickRetrievePsw(TNotifyUI* pObj);

private:
	void OnVRLoginMessage(const ChatMessage& msg);
	void OnVRSignupMessage(const ChatMessage& msg);

private:
	CTabLayoutUI* m_tabLoginSignup;

	CVerticalLayoutUI* m_vlShowPages;
	CButtonUI* m_btnPage1;
	CButtonUI* m_btnPage2;
	CButtonUI* m_btnPage3;
	CButtonUI* m_btnPage4;

	CButtonUI* m_btnLogin;
	CButtonUI* m_btnSignup;
	CButtonUI* m_btnToSignup;
	CButtonUI* m_btnRetrievePsw;
	
	CEditUI* m_etLoginAccount;
	CEditUI* m_etLoginPassword;
	
	std::shared_ptr<RadishClient> m_client;        // tcp通信

	EventHandler m_ClickHandler;  // 点击事件处理委托

	bool m_bFlag;
};

