#include "RadishClient.h"
#include <iostream>
#include "EncryptUtils.h"
#include <boost/lexical_cast.hpp>
#include <functional>


using namespace Radish::Utils;


namespace Radish {
namespace Network {


RadishClient::RadishClient()
	: m_sock(m_io)
{
}

RadishClient::~RadishClient()
{
	m_sock.close();
}

void RadishClient::Connect(const char *addr, unsigned short port)
{
	// ������ַ
	ip::tcp::resolver r(m_io);
	ip::tcp::resolver::query q(addr, boost::lexical_cast<std::string>(port));
	auto iter = r.resolve(q);

	// ���ӷ�����
	m_sock.async_connect(*iter,
		[this](const boost::system::error_code& ec) {
		if (!ec)
		{
			AsyncReadMessage();
		}
	});
}

void RadishClient::AddMessageHandler(const std::string& msgType, LPFN_HANDLER callback)
{
	m_MsgHandler.AddHandler(msgType, callback);
}

void RadishClient::AsyncReadMessage()
{
	memset(m_data, 0, max_length);

	m_sock.async_read_some(buffer(m_data),
		[this](boost::system::error_code ec, std::size_t)
	{
		if (!ec)
		{
			// ���������Ϣ
			this->DispatchMessage(m_data);

			// ��������������Ϣ
			AsyncReadMessage();
		}
	});
}

void RadishClient::AsyncWriteMessage(const ChatMessage& chatMessage)
{
	std::string data = chatMessage.GetData();
	m_sock.async_write_some(buffer(data.data(), data.length()),
		[this](const boost::system::error_code& ec, std::size_t) {
		if (!ec)
		{
			// ���ͳɹ��������ڴ˴�������
		}
	});
}

void RadishClient::Run()
{
	m_io.run();
}

void RadishClient::DispatchMessage(const std::string& data)
{
	// ������Ϣ
	ChatMessage msg;
	msg.SetData(data);

	// ������Ϣ���ͣ�������صĴ�����
	std::string msgType = msg.GetProperty<std::string>("MsgType");
	m_MsgHandler.Dispatch(msgType, msg);
}

} // namespace Chat
} // namespace Radish