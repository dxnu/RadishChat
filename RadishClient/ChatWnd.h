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

	// ���ݵ������ڵĲ���
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
	void OnAddFriendMessage(const ChatMessage& msg);		 // ��Ӻ�����Ϣ
	void OnVRAddFriendMessage(const ChatMessage& msg);       // ��Ӻ�����֤���ؽ����Ϣ
	void OnVRGetFriendsMessage(const ChatMessage& msg);      // ��ȡ�����б��ؽ����Ϣ
	void OnVRGetUserInfoMessage(const ChatMessage& msg);     // ��ȡ�û���Ϣ���ؽ����Ϣ
	void OnChatsMessage(const ChatMessage& msg);			 // ������Ϣ
	
private:
	
	void OnClickAddFriend(TNotifyUI* pObj);      // ��Ӻ��ѵ���¼�
	void OnClickAcceptRequest(TNotifyUI* pObj);  // ���ܺ����������¼�
	void OnClickIgnoreRequest(TNotifyUI* pObj);  // ���Ժ����������¼�
	void OnClickSendMessage(TNotifyUI* pObj);    // ������Ϣ����¼�
	void OnSelectListFriends(TNotifyUI* pObj);   // �����б�ѡ���¼�
	void OnClickChats(TNotifyUI* pObj);          // ����
	void OnClickContacts(TNotifyUI* pObj);       // ��ϵ��
	void OnClickSetting(TNotifyUI* pObj);        // ����
	void OnClickSends(TNotifyUI* pObj);          // ����
	void OnSelectListSessions(TNotifyUI* pObj);  // �Ự�б�ѡ���¼�
	void OnClickLogout(TNotifyUI* pObj);         // �˳�


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

	// ���ܺ�������
	CButtonUI* m_btnAccept;

	// �û���Ϣ�ؼ�
	CVerticalLayoutUI* m_vlUserInfo;
	CLabelUI* m_labUserInfoName;
	CLabelUI* m_labUserInfoWhatsUp;
	CLabelUI* m_labUserInfoRadishId;
	CButtonUI* m_btnSendMessage;

	// ������
	CTabLayoutUI* m_pTabNavigation;

	// �Ự�б�
	CListUI* m_pListSessions;

	// ��������
	CTabLayoutUI* m_pTabChats;

	// ��๦������
	CButtonUI* m_btnChats;
	CButtonUI* m_btnContacts;
	CButtonUI* m_btnSetting;

	// ��ǰ�Ự����ID������
	CLabelUI* m_labCurSessionId;
	CLabelUI* m_labCurSessionName;
};

