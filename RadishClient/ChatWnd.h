#pragma once
#include "Common.h"
#include "RadishClient.h"
#include "StringConvert.h"
#include <boost/any.hpp>
#include "UIEventHandler.h"


using namespace Radish::Network;


class ChatWnd : public WindowImplBase
{
public:
	ChatWnd(std::shared_ptr<RadishClient> client);
	~ChatWnd();

	virtual void InitWindow() override;
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	// 传递到本窗口的参数
	void SetParam(const std::string& fromUserId, const std::string& fromUserName);

protected:
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName(void) const override;

private:
	void GetFriendList();
	void MakeContactsItem(const std::string& fromUserId, const std::string& fromUserName);
	void MakeChats(LPCTSTR fromUserId, LPCTSTR fromUserName);
	CListContainerElementUI* MakeChatsItemLeft(LPCTSTR lpstrMsg);
	CListContainerElementUI* MakeChatsItemRight(LPCTSTR lpstrMsg);
	void MakeFriendRequests(const std::string& fromUserId, const std::string& fromUserName);
	void MakeSessions(LPCTSTR fromUserId, LPCTSTR fromUserName);

private:
	void OnAddFriendMessage(const ChatMessage& msg);		 // 添加好友消息
	void OnVRAddFriendMessage(const ChatMessage& msg);       // 添加好友验证返回结果消息
	void OnVRGetFriendsMessage(const ChatMessage& msg);      // 获取好友列表返回结果消息
	void OnVRGetUserInfoMessage(const ChatMessage& msg);     // 获取用户信息返回结果消息
	void OnChatsMessage(const ChatMessage& msg);			 // 聊天消息
	
private:
	
	void OnClickAddFriend(TNotifyUI* pObj);      // 添加好友点击事件
	void OnClickAcceptRequest(TNotifyUI* pObj);  // 接受好友请求点击事件
	void OnClickIgnoreRequest(TNotifyUI* pObj);  // 忽略好友请求点击事件
	void OnClickSendMessage(TNotifyUI* pObj);    // 发送消息点击事件
	void OnSelectListFriends(TNotifyUI* pObj);   // 好友列表选择事件
	void OnClickChats(TNotifyUI* pObj);          // 聊天
	void OnClickContacts(TNotifyUI* pObj);       // 联系人
	void OnClickSetting(TNotifyUI* pObj);        // 设置
	void OnClickSends(TNotifyUI* pObj);          // 发送
	void OnSelectListSessions(TNotifyUI* pObj);  // 会话列表选择事件
	void OnClickLogout(TNotifyUI* pObj);         // 退出


private:
	
	EventHandler m_ClickEvent;
	EventHandler m_ItemSelectEvent;

	CListUI* m_pListFriends;
	CListUI* m_pListAddRequests;
	CVerticalLayoutUI* m_chats;
	CLabelUI* m_labUserName;
	CButtonUI* m_btnLogout;
	CButtonUI* m_btnAddFriend;

	std::string m_fromUserId;
	std::string m_fromUserName;

	std::shared_ptr<RadishClient> m_client;

	// 接受好友请求
	CButtonUI* m_btnAccept;

	// 用户信息控件
	CVerticalLayoutUI* m_vlUserInfo;
	CLabelUI* m_labUserInfoName;
	CLabelUI* m_labUserInfoWhatsUp;
	CLabelUI* m_labUserInfoRadishId;
	CButtonUI* m_btnSendMessage;

	// 导航栏
	CTabLayoutUI* m_pTabNavigation;

	// 会话列表
	CListUI* m_pListSessions;

	// 聊天区域
	CTabLayoutUI* m_pTabChats;

	// 左侧功能区域
	CButtonUI* m_btnChats;
	CButtonUI* m_btnContacts;
	CButtonUI* m_btnSetting;

	// 当前会话好友ID与名称
	CLabelUI* m_labCurSessionId;
	CLabelUI* m_labCurSessionName;
};

