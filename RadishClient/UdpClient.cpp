#include "UdpClient.h"
#include <iostream>



namespace Radish {
namespace Network {



void UdpClient::ReadMessage(const std::string& msg, endpoint_type& ep)
{
	m_sock.send_to(buffer(msg, msg.size()), ep);
}

void UdpClient::WriteMessage(endpoint_type& sender_ep)
{
	memset(m_data, 0, MAX_LENGTH);
	size_t len = m_sock.receive_from(buffer(m_data, MAX_LENGTH), sender_ep);
	std::cout << "received: " << m_data << "\n";
}

} // namespace Network
} // namespace Radish

