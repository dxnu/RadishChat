#pragma once
#include <boost/asio.hpp>
#include "ChatMessage.h"
#include "MessageHandler.h"

using namespace boost::asio;
using namespace Radish::Chat;
using namespace Utils;

namespace Radish {
namespace Network {



//////////////////////////////////////////////////////////////////////////
// class RadishClient
// 客户端Tcp通信类
// 负责与服务器进行连接与通信
//////////////////////////////////////////////////////////////////////////
class RadishClient
{
	using this_type      = RadishClient;
	using endpoint_type  = ip::tcp::endpoint;
	using socket_type    = ip::tcp::socket;
	using LPFN_HANDLER   = std::function<void(ChatMessage)>;

public:
	RadishClient();
	~RadishClient();
	
	// 连接
	void Connect(const char *addr, unsigned short port);

	// 绑定消息处理事件
	void AddMessageHandler(const std::string& msgType, LPFN_HANDLER);
	
	// 读取消息
	void AsyncReadMessage();
	
	// 写入消息
	void AsyncWriteMessage(const ChatMessage& chatMessage);

	void Run();

private:

	// 数据处理
	void DispatchMessage(const std::string& data);

private:
	io_context m_io;
	socket_type m_sock;
	ChatMessage m_chatMessage;
	enum { max_length = 1024 };
	char m_data[max_length];
	MessageHandler m_MsgHandler;
};

} // namespace Network
} // namespace Radish