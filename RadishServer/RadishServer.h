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

	// 会话处理
	void Start();

private:

	// 读取消息
	void AsyncReadMessage();
	
	// 写入消息
	void AsyncWriteMessage(const ChatMessage& msg);
	void AsyncWriteMessage(sock_ptr sock, const ChatMessage& msg);

private:

	void DispatchMessage(const std::string& data);      // 分发消息

	void OnLoginMessage(const ChatMessage& msg);        // 登录消息
	void OnSignupMessage(const ChatMessage& msg);       // 注册消息
	void OnAddFriendMessage(const ChatMessage& msg);    // 添加好友消息
	void OnVRAddFriendMessage(const ChatMessage& msg);  // 添加好友验证结果消息
	void OnGetFriendsMessage(const ChatMessage& msg);   // 获取好友列表消息
	void OnGetUserInfoMessage(const ChatMessage& msg);  // 获取用户信息消息
	void OnChatsMessage(const ChatMessage& msg);        // 聊天消息
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


