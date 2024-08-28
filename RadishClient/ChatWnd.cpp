#include "ChatWnd.h"
#include "LoginWnd.h"
#include <functional>
#include <regex>
#include "LoginWnd.h"


using namespace Utils;

ChatWnd::ChatWnd(std::shared_ptr<RadishClient> client)
	: m_client(client)
{
	m_client->AddMessageHandler("addfriend", std::bind(&ChatWnd::OnAddFriendMessage, this, std::placeholders::_1));
	m_client->AddMessageHandler("vraddfriend", std::bind(&ChatWnd::OnVRAddFriendMessage, this, std::placeholders::_1));
	m_client->AddMessageHandler("vrgetfriends", std::bind(&ChatWnd::OnVRGetFriendsMessage, this, std::placeholders::_1));
	m_client->AddMessageHandler("vrgetuserinfo", std::bind(&ChatWnd::OnVRGetUserInfoMessage, this, std::placeholders::_1));
	m_client->AddMessageHandler("chats", std::bind(&ChatWnd::OnChatsMessage, this, std::placeholders::_1));

	m_client->AsyncReadMessage();
	
	std::thread([this]() { m_client->Run(); }).detach();
}


ChatWnd::~ChatWnd()
{
}

void ChatWnd::InitWindow()
{
	m_pListFriends = static_cast<CListUI*>(m_PaintManager.FindControl(L"list_friends"));
	m_ItemSelectEvent.AddHandler(m_pListFriends, std::bind(&ChatWnd::OnSelectListFriends, this, std::placeholders::_1));

	m_chats = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(L"list_chats"));

	// initialize controls
	m_labUserName = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"lab_user_name"));
	m_btnLogout = static_cast<CButtonUI*>(m_PaintManager.FindControl(L"btn_logout"));
	m_labUserName->SetText(LPCTSTR_CAST(m_fromUserName));

	// 添加好友
	m_btnAddFriend = static_cast<CButtonUI*>(m_PaintManager.FindControl(L"btn_add_friend"));
	m_ClickEvent.AddHandler(m_btnAddFriend, std::bind(&ChatWnd::OnClickAddFriend, this, std::placeholders::_1));

	// 添加请求列表
	m_pListAddRequests = static_cast<CListUI*>(m_PaintManager.FindControl(L"list_addrequests"));
	m_pListAddRequests->SetVisible(false);

	// 用户信息控件
	m_vlUserInfo = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(L"vl_userinfo"));
	m_labUserInfoName = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"lab_userinfo_name"));
	m_labUserInfoRadishId = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"lab_userinfo_radishid"));
	m_labUserInfoWhatsUp = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"lab_userinfo_whatsup"));
	
	m_btnSendMessage = static_cast<CButtonUI*>(m_PaintManager.FindControl(L"btn_messages"));
	m_ClickEvent.AddHandler(m_btnSendMessage, std::bind(&ChatWnd::OnClickSendMessage, this, std::placeholders::_1));

	// 导航列表
	m_pTabNavigation = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(L"tab_navigation"));
	m_pTabNavigation->SelectItem(1);
	

	// 会话列表
	m_pListSessions = static_cast<CListUI*>(m_PaintManager.FindControl(L"list_sessions"));
	m_ItemSelectEvent.AddHandler(m_pListSessions, std::bind(&ChatWnd::OnSelectListSessions, this, std::placeholders::_1));

	// 聊天列表
	m_pTabChats = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(L"tab_chats"));

	// 左侧功能列表
	m_btnChats = static_cast<CButtonUI*>(m_PaintManager.FindControl(L"btn_chats"));
	m_btnContacts = static_cast<CButtonUI*>(m_PaintManager.FindControl(L"btn_contacts"));
	m_btnSetting = static_cast<CButtonUI*>(m_PaintManager.FindControl(L"btn_setting"));
	m_ClickEvent.AddHandler(m_btnChats, std::bind(&ChatWnd::OnClickChats, this, std::placeholders::_1));
	m_ClickEvent.AddHandler(m_btnContacts, std::bind(&ChatWnd::OnClickContacts, this, std::placeholders::_1));
	m_ClickEvent.AddHandler(m_btnSetting, std::bind(&ChatWnd::OnClickSetting, this, std::placeholders::_1));

	m_labCurSessionId = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"lab_cur_id"));
	m_labCurSessionName = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"lab_cur_name"));

	// 退出按钮
	m_btnLogout = static_cast<CButtonUI*>(m_PaintManager.FindControl(L"btn_logout"));
	m_ClickEvent.AddHandler(m_btnLogout, std::bind(&ChatWnd::OnClickLogout, this, std::placeholders::_1));

	// 添加自己到联系人列表
	MakeContactsItem(m_fromUserId, m_fromUserName);

	// 获取好友列表
	GetFriendList();
}

void ChatWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == L"click")
	{
		if (msg.pSender->GetName() == L"btn_min")
		{
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		}
		else if (msg.pSender->GetName() == L"btn_close")
		{
			// 发送退出消息
			ChatMessage msg;
			msg.SetProperty("MsgType", "close");
			msg.SetProperty("FromUserId", m_fromUserId);
			m_client->AsyncWriteMessage(msg);

			this->Close();
		}
		else
		{
			m_ClickEvent.Dispatch(&msg);
		}
	}
	else if (msg.sType == L"itemselect")
	{
		m_ItemSelectEvent.Dispatch(&msg);
	}
}

LRESULT ChatWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCLBUTTONDBLCLK)
	{
		__super::HandleMessage(uMsg, wParam, lParam);

		// 随着好友申请列表长度的改变，改变好友列表的最大长度
		RECT r;
		::GetClientRect(m_hWnd, &r);
		int listFriendsMaxHeight = r.bottom - 120 - 20 - m_pListAddRequests->GetMaxHeight();
		m_pListFriends->SetAttribute(L"height", int_to_wstring(listFriendsMaxHeight).c_str());

		return 0;
	}
	
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void ChatWnd::SetParam(const std::string& fromUserId, const std::string& fromUserName)
{
	m_fromUserId = fromUserId;
	m_fromUserName = fromUserName;
}

DuiLib::CDuiString ChatWnd::GetSkinFile()
{
	return L"main_chat.xml";
}

LPCTSTR ChatWnd::GetWindowClassName(void) const
{
	return L"ChatWnd";
}

void ChatWnd::GetFriendList()
{
	ChatMessage sendMsg;
	sendMsg.SetTextMessage("", m_fromUserId, "getfriends", "");
	m_client->AsyncWriteMessage(sendMsg);
}

void ChatWnd::MakeContactsItem(const std::string& fromUserId, const std::string& fromUserName)
{
	CListContainerElementUI* pListContainerElement = new CListContainerElementUI;
	CButtonUI* btnAvator = new CButtonUI;
	CLabelUI* labFriendsName = new CLabelUI;
	CLabelUI* labFriendId = new CLabelUI;

	pListContainerElement->SetAttribute(L"height", L"50");
	pListContainerElement->Add(btnAvator);
	pListContainerElement->Add(labFriendsName);
	pListContainerElement->Add(labFriendId);

	// 头像
	btnAvator->SetAttribute(L"width", L"30");
	btnAvator->SetAttribute(L"height", L"30");
	btnAvator->SetAttribute(L"padding", L"5,5,5,5");
	btnAvator->SetBkImage(L"images/default-avatar.jpg");

	// 名称
	labFriendsName->SetName(L"lab_friend_name");
	labFriendsName->SetText(LPCTSTR_CAST(fromUserName));
	labFriendsName->SetAttribute(L"textsize", L"50");
	labFriendsName->SetTextColor(0xFFFFFFFF);
	labFriendsName->SetFont(0);
	labFriendsName->SetAttribute(L"padding", L"5,0,0,0");

	// ID
	labFriendId->SetName(L"lab_friend_id");
	labFriendId->SetText(LPCTSTR_CAST(fromUserId));
	labFriendId->SetVisible(false);

	m_pListFriends->Add(pListContainerElement);
}

void ChatWnd::MakeChats(LPCTSTR fromUserId, LPCTSTR fromUserName)
{
	CDialogBuilder builder;
	CVerticalLayoutUI* vLayout = static_cast<CVerticalLayoutUI*>(
		builder.Create(L"main/chats_item.xml", 0, 0, &m_PaintManager, 0));
	if (vLayout)
	{
		CListUI* pListChats = static_cast<CListUI*>(vLayout->FindSubControl(L"list_chats"));
		CButtonUI* btnSends = static_cast<CButtonUI*>(vLayout->FindSubControl(L"btn_sends"));
		m_ClickEvent.AddHandler(btnSends, std::bind(&ChatWnd::OnClickSends, this, std::placeholders::_1));
		/*pListChats->Add(MakeChatsItemRight("从失败中寻找原因，避开失败就是成功。活得精彩比活得成功更有意义"
			"有两种学习方式是有效的，第一种：发现什么有效，复制它；第二种发现什么无效，规避它。"
			"有两种学习方式是有效的，第一种：发现什么有效，复制它；第二种发现什么无效，规避它。"));
		pListChats->Add(MakeChatsItemRight("hello world!"));
		pListChats->Add(MakeChatsItemLeft("hello world!"));
		pListChats->Add(MakeChatsItemLeft("从失败中寻找原因，避开失败就是成功。活得精彩比活得成功更有意义"
			"有两种学习方式是有效的，第一种：发现什么有效，复制它；第二种发现什么无效，规避它。"
			"有两种学习方式是有效的，第一种：发现什么有效，复制它；第二种发现什么无效，规避它。"));*/

		m_pTabChats->Add(vLayout);
		m_pTabChats->SelectItem(vLayout);
		
		// 设置当前会话好友信息
		m_labCurSessionId->SetText(fromUserId);
		m_labCurSessionName->SetText(fromUserName);
	}
}

DuiLib::CListContainerElementUI* ChatWnd::MakeChatsItemLeft(LPCTSTR lpstrMsg)
{
	CDialogBuilder builder;
	CListContainerElementUI* pListContainerElement = static_cast<CListContainerElementUI*>(
		builder.Create(L"main/list_chats_litem.xml", 0, 0, &m_PaintManager, 0));

	if (pListContainerElement)
	{
		CRichEditUI* pRichEdit = (CRichEditUI*)pListContainerElement->FindSubControl(L"richet_lchats_message");
		CButtonUI* pBtnAvatar = (CButtonUI*)pListContainerElement->FindSubControl(L"btn_lchats_avatar");
		pRichEdit->SetText(lpstrMsg);

		// 获得当前使用字体宽高
		int n = pRichEdit->GetFont();
		TFontInfo* fontInfo = m_PaintManager.GetFontInfo(n);
		TEXTMETRIC tm = fontInfo->tm;
		double cxChar = tm.tmMaxCharWidth * 0.75;
		double cyChar = (tm.tmHeight + tm.tmExternalLeading) * 1.35;

		// 获取当前屏幕下，最大单行长度
		RECT r;
		::GetClientRect(m_hWnd, &r);
		double maxWidth = (r.right - 345) / 2;

		int strLength = wcslen(lpstrMsg);
		int lines = ceil(strLength * cxChar / maxWidth);
		int height = lines * cyChar;
		if (lines <= 1)
		{
			pRichEdit->SetAttribute(L"width", INT_TO_LPCTSTR(strLength * cxChar + 30));
			pRichEdit->SetAttribute(L"height", INT_TO_LPCTSTR(height + 20));
			pListContainerElement->SetAttribute(L"height", INT_TO_LPCTSTR(height + 23));
		}
		else
		{
			pRichEdit->SetAttribute(L"width", INT_TO_LPCTSTR(static_cast<int>(maxWidth + 30)));
			pRichEdit->SetAttribute(L"height", INT_TO_LPCTSTR(height + 20));
			pListContainerElement->SetAttribute(L"height", INT_TO_LPCTSTR(height + 23));
		}

		return pListContainerElement;
	}

	return nullptr;
}

DuiLib::CListContainerElementUI* ChatWnd::MakeChatsItemRight(LPCTSTR lpstrMsg)
{
	CDialogBuilder builder;
	CListContainerElementUI* pListContainerElement = static_cast<CListContainerElementUI*>(
		builder.Create(L"main/list_chats_ritem.xml", 0, 0, &m_PaintManager, 0));
	if (pListContainerElement)
	{
		CRichEditUI* pRichEdit = (CRichEditUI*)pListContainerElement->FindSubControl(L"richet_rchats_message");
		CButtonUI* pBtnAvatar = (CButtonUI*)pListContainerElement->FindSubControl(L"btn_rchats_avatar");
		CVerticalLayoutUI* pChatsBk = (CVerticalLayoutUI*)pListContainerElement->FindSubControl(L"vl_rchats_bk");
		pRichEdit->SetText(lpstrMsg);

		// 获得当前使用字体宽高
		int n = pRichEdit->GetFont();
		TFontInfo* fontInfo = m_PaintManager.GetFontInfo(n);
		TEXTMETRIC tm = fontInfo->tm;
		double cxChar = tm.tmMaxCharWidth * 0.75;
		double cyChar = (tm.tmHeight + tm.tmExternalLeading) * 1.35;
		
		// 获取当前屏幕下，最大单行长度
		RECT r;
		::GetClientRect(m_hWnd, &r);
		double maxWidth = (r.right - 345) / 2;

		int strLength = wcslen(lpstrMsg);
		int lines = ceil(strLength * cxChar / maxWidth);
		int height = lines * cyChar;
		if (lines <= 1)
		{
			pRichEdit->SetAttribute(L"width", INT_TO_LPCTSTR(strLength * cxChar));
			pRichEdit->SetAttribute(L"height", INT_TO_LPCTSTR(height));
			pChatsBk->SetAttribute(L"width", INT_TO_LPCTSTR(strLength * cxChar + 30));
			pChatsBk->SetAttribute(L"height", INT_TO_LPCTSTR(height + 20));
			pListContainerElement->SetAttribute(L"height", INT_TO_LPCTSTR(height + 23));
		}
		else
		{
			pRichEdit->SetAttribute(L"width", INT_TO_LPCTSTR(static_cast<int>(maxWidth)));
			pRichEdit->SetAttribute(L"height", INT_TO_LPCTSTR(height));
			pChatsBk->SetAttribute(L"width", INT_TO_LPCTSTR(static_cast<int>(maxWidth) + 30));
			pChatsBk->SetAttribute(L"height", INT_TO_LPCTSTR(height + 20));
			pListContainerElement->SetAttribute(L"height", INT_TO_LPCTSTR(height + 23));
		}

		return pListContainerElement;
	}

	return nullptr;
}

void ChatWnd::MakeFriendRequests(const std::string& fromUserId, const std::string& fromUserName)
{
	CListContainerElementUI* pListContainerElement = new CListContainerElementUI;
	CVerticalLayoutUI* vLayout = new CVerticalLayoutUI;
	CLabelUI* labFromUserId = new CLabelUI;
	CLabelUI* labFromUserName = new CLabelUI;
	CLabelUI* labTips = new CLabelUI;
	vLayout->Add(labFromUserId);
	vLayout->Add(labFromUserName);
	vLayout->Add(labTips);

	labFromUserId->SetName(L"lab_request_id");
	labFromUserId->SetText(LPCTSTR_CAST(fromUserId));
	labFromUserId->SetVisible(false);
	labFromUserName->SetName(L"lab_request_name");
	labFromUserName->SetAttribute(L"width", L"170");
	labFromUserName->SetText(LPCTSTR_CAST(fromUserName));
	labFromUserName->SetTextColor(0xFFFFFFFF);
	labFromUserName->SetAttribute(L"padding", L"5,0,0,0");

	labTips->SetText(L"请求添加你为好友");
	labTips->SetTextColor(0xFFFFFFFF);
	labTips->SetAttribute(L"padding", L"5,0,0,0");

	CHorizontalLayoutUI* hLayout = new CHorizontalLayoutUI;
	CButtonUI* btnAccept = new CButtonUI;
	CButtonUI* btnIgnore = new CButtonUI;
	hLayout->Add(vLayout);
	hLayout->Add(btnAccept);
	hLayout->Add(btnIgnore);

	btnAccept->SetText(L"Accept");
	btnAccept->SetTextColor(0xFF00FF00);
	btnAccept->SetAttribute(L"width", L"40");
	btnIgnore->SetText(L"x");
	btnIgnore->SetTextColor(0xFFFF0000);
	btnIgnore->SetAttribute(L"width", L"40");

	pListContainerElement->Add(hLayout);
	pListContainerElement->SetAttribute(L"height", L"40");

	// 添加点击事件
	m_ClickEvent.AddHandler(btnAccept, std::bind(&ChatWnd::OnClickAcceptRequest, this, std::placeholders::_1));
	m_ClickEvent.AddHandler(btnIgnore, std::bind(&ChatWnd::OnClickIgnoreRequest, this, std::placeholders::_1));

	m_pListAddRequests->Add(pListContainerElement);
	int height = m_pListAddRequests->GetMaxHeight();
	if (height <= 240) m_pListAddRequests->SetMaxHeight(height + 60);
	else m_pListAddRequests->SetAttribute(L"vscrollbar", L"true");

	if (!m_pListAddRequests->IsVisible())
	{
		m_pListAddRequests->SetVisible(true);
	}

	// 随着好友申请列表长度的改变，改变好友列表的最大长度
	RECT r;
	::GetClientRect(m_hWnd, &r);
	int listFriendsMaxHeight = r.bottom - 120 - 20 - m_pListAddRequests->GetMaxHeight();
	m_pListFriends->SetAttribute(L"height", INT_TO_LPCTSTR(listFriendsMaxHeight));
}

void ChatWnd::MakeSessions(LPCTSTR fromUserId, LPCTSTR fromUserName)
{
	CListContainerElementUI* pListContainerElement = new CListContainerElementUI;
	pListContainerElement->SetAttribute(L"height", L"60");

	CVerticalLayoutUI* vLayout = new CVerticalLayoutUI;
	CControlUI* ctrlSpring1 = new CControlUI;
	CControlUI* ctrlSpring2 = new CControlUI;

	CHorizontalLayoutUI* hLayout = new CHorizontalLayoutUI;
	hLayout->SetAttribute(L"height", L"40");

	CControlUI* ctrlAvatar = new CControlUI;
	ctrlAvatar->SetAttribute(L"width", L"35");
	ctrlAvatar->SetAttribute(L"height", L"35");
	ctrlAvatar->SetAttribute(L"padding", L"5,0,5,0");
	ctrlAvatar->SetBkImage(L"images/default-avatar.jpg");

	CVerticalLayoutUI* vLayoutInfo = new CVerticalLayoutUI;
	CLabelUI* labUserName = new CLabelUI;
	CLabelUI* labUserId = new CLabelUI;
	CLabelUI* labMsgSummary = new CLabelUI;
	labUserName->SetText(fromUserName);
	labUserName->SetTextColor(0xFFFFFFFF);
	labUserId->SetName(L"lab_user_id");
	labUserId->SetText(fromUserId);
	labUserId->SetVisible(false);
	labMsgSummary->SetText(L"riancy --- 123456");
	labMsgSummary->SetTextColor(0xFFFFFFFF);
	vLayoutInfo->Add(labUserName);
	vLayoutInfo->Add(labUserId);
	vLayoutInfo->Add(labMsgSummary);

	CLabelUI* labTime = new CLabelUI;
	labTime->SetText(L"08/23/19");
	labTime->SetTextColor(0xFFCCCCCC);
	labTime->SetAutoCalcWidth(true);
	labTime->SetAttribute(L"padding", L"0,0,5,0");

	hLayout->Add(ctrlAvatar);
	hLayout->Add(vLayoutInfo);
	hLayout->Add(labTime);

	vLayout->Add(ctrlSpring1);
	vLayout->Add(hLayout);
	vLayout->Add(ctrlSpring2);

	pListContainerElement->Add(vLayout);

	m_pListSessions->Add(pListContainerElement);
	m_pListSessions->SelectItem(m_pListSessions->GetCount() - 1);
	MakeChats(fromUserId, fromUserName);
}

// 添加好友
void ChatWnd::OnClickAddFriend(TNotifyUI* pObj)
{
	CEditUI* etFriendId = static_cast<CEditUI*>(m_PaintManager.FindControl(L"et_friend_id"));
	std::string friendId = STRING_CAST(etFriendId->GetText().GetData());

	if (!friendId.empty())
	{
		try {
			ChatMessage sendMsg;
			sendMsg.SetProperty("FromUserName", m_fromUserName);
			sendMsg.SetTextMessage(friendId, m_fromUserId, "addfriend", "");

			// 发送添加消息
			m_client->AsyncWriteMessage(sendMsg);
		}
		catch (const std::exception& e)
		{
			MessageBox(m_hWnd, LPCTSTR_CAST(e.what()), L"Error", MB_OK);
		}
	}

}

void ChatWnd::OnClickIgnoreRequest(TNotifyUI* pObj)
{
	CListContainerElementUI* container = (CListContainerElementUI*)pObj->pSender->GetParent()->GetParent();
	m_pListAddRequests->Remove(container);

	int height = m_pListAddRequests->GetCount() * 60 + 60;
	if (height <= 240) m_pListAddRequests->SetMaxHeight(height - 60);
	else m_pListAddRequests->SetAttribute(L"vscrollbar", L"true");

	// 随着好友申请列表长度的改变，改变好友列表的最大长度
	RECT r;
	::GetClientRect(m_hWnd, &r);
	int listFriendsMaxHeight = r.bottom - 120 - 20 - m_pListAddRequests->GetMaxHeight();
	m_pListFriends->SetAttribute(L"height", INT_TO_LPCTSTR(listFriendsMaxHeight));
}

void ChatWnd::OnClickSendMessage(TNotifyUI* pObj)
{
	CVerticalLayoutUI* vLayout = (CVerticalLayoutUI*)pObj->pSender->GetParent();
	CLabelUI* labUserInfoId = (CLabelUI*)vLayout->FindSubControl(L"lab_userinfo_radishid");
	CLabelUI* labUserInfoName = (CLabelUI*)vLayout->FindSubControl(L"lab_userinfo_name");

	//MessageBox(m_hWnd, labRadishId->GetText().GetData(), L"Tips", MB_OK);
	// 如果已有会话，则直接跳转到会话
	int count = m_pListSessions->GetCount();
	for (auto i = 0; i < count; ++i)
	{
		CListContainerElementUI* pListContainerElement = (CListContainerElementUI*)m_pListSessions->GetItemAt(i);
		CLabelUI* labUserId = (CLabelUI*)pListContainerElement->FindSubControl(L"lab_user_id");
		if (labUserId->GetText() == labUserInfoId->GetText())
		{
			m_pListSessions->SelectItem(i);
			m_pTabNavigation->SelectItem(0);
			return;
		}
	}
	
	// 没有会话，创建新会话
	MakeSessions(labUserInfoId->GetText(), labUserInfoName->GetText());
	m_pTabNavigation->SelectItem(0);
}

void ChatWnd::OnSelectListFriends(TNotifyUI* pObj)
{
	int n = m_pListFriends->GetCurSel();

	CListContainerElementUI* pListContainerElement = (CListContainerElementUI*)m_pListFriends->GetItemAt(n);
	CLabelUI* labFriendId = (CLabelUI*)pListContainerElement->FindSubControl(L"lab_friend_id");

	ChatMessage sendMsg;
	std::string toUserId = STRING_CAST(labFriendId->GetText().GetData());
	sendMsg.SetTextMessage(toUserId, "", "getuserinfo", "");
	m_client->AsyncWriteMessage(sendMsg);
}

void ChatWnd::OnClickChats(TNotifyUI* pObj)
{
	m_pTabNavigation->SelectItem(0);
}

void ChatWnd::OnClickContacts(TNotifyUI* pObj)
{
	m_pTabNavigation->SelectItem(1);
}

void ChatWnd::OnClickSetting(TNotifyUI* pObj)
{
	m_pTabNavigation->SelectItem(2);
}

void ChatWnd::OnClickSends(TNotifyUI* pObj)
{
	// 先在聊天窗口中显示消息
	CVerticalLayoutUI* pvlParent = (CVerticalLayoutUI*)pObj->pSender->GetParent()->GetParent();
	CRichEditUI* pRichEditContents = (CRichEditUI*)pvlParent->FindSubControl(L"richet_contents");
	pvlParent = (CVerticalLayoutUI*)pvlParent->GetParent();
	CListUI* pListChats = (CListUI*)pvlParent->FindSubControl(L"list_chats");

	if (!pRichEditContents->GetText().IsEmpty())
	{
		CListContainerElementUI *pListContainerElement = MakeChatsItemRight(pRichEditContents->GetText());
		pListChats->Add(pListContainerElement);

		// 发送消息
		std::string msg = STRING_CAST(pRichEditContents->GetText().GetData());
		ChatMessage sendMsg;
		std::string toUserName = STRING_CAST(m_labCurSessionId->GetText().GetData());
		std::string content = STRING_CAST(pRichEditContents->GetText().GetData());
		sendMsg.SetTextMessage(toUserName, m_fromUserId, "chats", content);
		sendMsg.SetProperty("FromUserName", m_fromUserName);
		m_client->AsyncWriteMessage(sendMsg);

		pRichEditContents->SetText(L"");  // 发送消息后，清空输入框中的内容
	}
}

void ChatWnd::OnSelectListSessions(TNotifyUI* pObj)
{
	int index = m_pListSessions->GetCurSel();
	m_pTabChats->SelectItem(index);
}

void ChatWnd::OnClickLogout(TNotifyUI* pObj)
{
	this->Close();
}

void ChatWnd::OnClickAcceptRequest(TNotifyUI* pObj)
{
	if (pObj->pSender->GetText() == L"Added") return;

	CHorizontalLayoutUI* hLayout = (CHorizontalLayoutUI*)pObj->pSender->GetParent();
	CLabelUI* toUserId = (CLabelUI*)hLayout->FindSubControl(L"lab_request_id");
	CLabelUI* toUserName = (CLabelUI*)hLayout->FindSubControl(L"lab_request_name");

	ChatMessage sendMsg;
	sendMsg.SetProperty("ErrCode", 0);
	sendMsg.SetProperty("ToUserName", STRING_CAST(toUserName->GetText().GetData()));
	sendMsg.SetProperty("FromUserName", m_fromUserName);
	sendMsg.SetTextMessage(STRING_CAST(toUserId->GetText().GetData()), m_fromUserId, "vraddfriend", "");
	m_client->AsyncWriteMessage(sendMsg);

	pObj->pSender->SetText(L"Added");  // 设为已添加
}

void ChatWnd::OnAddFriendMessage(const ChatMessage& msg)
{
	// 获取添加者ID
	std::string fromUserId = msg.GetProperty("FromUserId");
	std::string fromUserName = msg.GetProperty("FromUserName");

	MakeFriendRequests(fromUserId, fromUserName);
}

void ChatWnd::OnVRAddFriendMessage(const ChatMessage& msg)
{
	if (msg.GetProperty<int>("ErrCode") == 0)
	{
		// 添加成功
		MakeContactsItem(msg.GetProperty("FromUserId"), msg.GetProperty("FromUserName"));
	}
	else
	{
		// 添加失败
		std::string err = msg.GetProperty("ErrMsg");
		MessageBox(m_hWnd, LPCTSTR_CAST(err), L"Error", MB_OK | MB_ICONWARNING);
	}
}

void ChatWnd::OnVRGetFriendsMessage(const ChatMessage& msg)
{
	std::string friendsList = msg.GetProperty("Content");

	std::regex r("\\[(.*)-(.*)\\]");
	std::sregex_iterator pos(friendsList.begin(), friendsList.end(), r);
	decltype(pos) end;
	for (; pos != end; ++pos)
	{
		MakeContactsItem(pos->str(1), pos->str(2));
	}
}

void ChatWnd::OnVRGetUserInfoMessage(const ChatMessage& msg)
{
	std::string userId = msg.GetProperty("AccountId");
	std::string userName = msg.GetProperty("AccountName");
	std::string email = msg.GetProperty("Email");
	std::string signature = msg.GetProperty("WhatsUp");

	m_vlUserInfo->SetVisible(true);
	m_labUserInfoName->SetText(LPCTSTR_CAST(userName));
	m_labUserInfoWhatsUp->SetText(LPCTSTR_CAST(signature));
	m_labUserInfoRadishId->SetText(LPCTSTR_CAST(userId));
}

void ChatWnd::OnChatsMessage(const ChatMessage& msg)
{
	std::string fromUserId = msg.GetProperty("FromUserId");
	std::string fromUserName = msg.GetProperty("FromUserName");
	std::string content = msg.GetProperty("Content");

	// 先在当前会话列表中寻找是否与其存在会话
	int count = m_pListSessions->GetCount();
	CListContainerElementUI* pListContainerElement;
	CLabelUI* labUserId;
	for (auto i = 0; i < count; ++i)
	{
		pListContainerElement = (CListContainerElementUI*)m_pListSessions->GetItemAt(i);
		labUserId = (CLabelUI*)pListContainerElement->FindSubControl(L"lab_user_id");
		
		LPCTSTR userId = labUserId->GetText().GetData();
		if (STRING_CAST(userId) == fromUserId)
		{
			CVerticalLayoutUI* vLayout = (CVerticalLayoutUI*)m_pTabChats->GetItemAt(i);
			CListUI* pListChats = (CListUI*)vLayout->FindSubControl(L"list_chats");
			pListChats->Add(MakeChatsItemLeft(LPCTSTR_CAST(content)));
			return;
		}
	}


	// 未发现会话，创建新会话
	MakeSessions(LPCTSTR_CAST(fromUserId), LPCTSTR_CAST(fromUserName));
	int index = m_pListSessions->GetCurSel();
	CVerticalLayoutUI* vLayout = (CVerticalLayoutUI*)m_pTabChats->GetItemAt(index);
	CListUI* pListChats = (CListUI*)vLayout->FindSubControl(L"list_chats");
	pListChats->Add(MakeChatsItemLeft(LPCTSTR_CAST(content)));
	m_pTabNavigation->SelectItem(0);
}

