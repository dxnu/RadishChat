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
// �ͻ���Tcpͨ����
// ���������������������ͨ��
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
	
	// ����
	void Connect(const char *addr, unsigned short port);

	// ����Ϣ�����¼�
	void AddMessageHandler(const std::string& msgType, LPFN_HANDLER);
	
	// ��ȡ��Ϣ
	void AsyncReadMessage();
	
	// д����Ϣ
	void AsyncWriteMessage(const ChatMessage& chatMessage);

	void Run();

private:

	// ���ݴ���
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