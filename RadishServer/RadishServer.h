#pragma once
#include <boost/asio.hpp>
#include "DataAccessFactory.h"
#include "ChatMessage.h"
#include "MessageHandler.h"


using namespace boost::asio;
using namespace Radish::SQL;
using namespace Radish::Chat;
using namespace Utils;


namespace Network {


using acceptor_type = ip::tcp::acceptor;
using endpoint_type = ip::tcp::endpoint;
using socket_type   = ip::tcp::socket;
using sock_ptr      = std::shared_ptr<socket_type>;


class ChatSession
	: public std::enable_shared_from_this<ChatSession>
{
	using this_type = ChatSession;
	using DBParamType = RadishChatDatabaseBase::DataType;
	using DBFactory = Singleton<DataAccessFactory>;
	using ClientsQueue = Singleton<ContentValues<std::string, sock_ptr>>;
	using MessageQueue = Singleton<ContentValues<std::string, ChatMessage>>;

public:
	ChatSession(sock_ptr sock);

	// �Ự����
	void Start();

private:

	// ��ȡ��Ϣ
	void AsyncReadMessage();
	
	// д����Ϣ
	void AsyncWriteMessage(const ChatMessage& msg);
	void AsyncWriteMessage(sock_ptr sock, const ChatMessage& msg);

private:

	void DispatchMessage(const std::string& data);      // �ַ���Ϣ

	void OnLoginMessage(const ChatMessage& msg);        // ��¼��Ϣ
	void OnSignupMessage(const ChatMessage& msg);       // ע����Ϣ
	void OnAddFriendMessage(const ChatMessage& msg);    // ��Ӻ�����Ϣ
	void OnVRAddFriendMessage(const ChatMessage& msg);  // ��Ӻ�����֤�����Ϣ
	void OnGetFriendsMessage(const ChatMessage& msg);   // ��ȡ�����б���Ϣ
	void OnGetUserInfoMessage(const ChatMessage& msg);  // ��ȡ�û���Ϣ��Ϣ
	void OnChatsMessage(const ChatMessage& msg);        // ������Ϣ
	void OnCloseMessage(const ChatMessage& msg);

private:
	enum {max_length = 1024};
	char m_data[max_length];
	sock_ptr m_sock;
	MessageHandler m_MsgHandler;
};


class RadishServer
{
public:
	RadishServer(uint16_t port)
		: m_acceptor(m_io, endpoint_type(ip::tcp::v4(), port))
	{}

	void Accept();
	void Run();

private:
	io_context m_io;
	acceptor_type m_acceptor;
};

} // namespace Network


