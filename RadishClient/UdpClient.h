#pragma once
#include <boost/asio.hpp>
#include <string>

using namespace boost::asio;

namespace Radish {
namespace Network {

//////////////////////////////////////////////////////////////////////////
// class UdpClient
// 客户端通信类
// 负责与服务器进行连接与通信
//////////////////////////////////////////////////////////////////////////
class UdpClient
{
	typedef UdpClient			this_type;
	typedef ip::udp::endpoint   endpoint_type;
	typedef ip::udp::socket     socket_type;

public:
	UdpClient() : m_sock(m_io) {}
	~UdpClient();
	void ReadMessage(const std::string& msg, endpoint_type& ep);
	void WriteMessage(endpoint_type& sender_ep);

private:
	io_context m_io;
	socket_type m_sock;
	enum { MAX_LENGTH = 1024 };
	char m_data[MAX_LENGTH];
};

} // namespace Network
} // namespace Radish

