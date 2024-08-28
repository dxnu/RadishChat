#include "RadishServer.h"
#include <iostream>
#include <memory>


namespace Network {

void RadishServer::Accept()
{
	sock_ptr sock(new socket_type(m_io));
	m_acceptor.async_accept(*sock, 
		[this, sock](const boost::system::error_code& ec) 
	{
		if (!ec)
		{
			std::cout << "connected client: " << sock->remote_endpoint().address() << '\n';

			// �Ự����
			std::make_shared<ChatSession>(sock)->Start();
		}

		// ������һ������
		Accept();
	});
}

void RadishServer::Run()
{
	m_io.run();
}

ChatSession::ChatSession(sock_ptr sock)
	: m_sock(sock)
{
	m_MsgHandler.AddHandler("login", std::bind(&this_type::OnLoginMessage, this, std::placeholders::_1));
	m_MsgHandler.AddHandler("signup", std::bind(&this_type::OnSignupMessage, this, std::placeholders::_1));
	m_MsgHandler.AddHandler("addfriend", std::bind(&this_type::OnAddFriendMessage, this, std::placeholders::_1));
	m_MsgHandler.AddHandler("vraddfriend", std::bind(&this_type::OnVRAddFriendMessage, this, std::placeholders::_1));
	m_MsgHandler.AddHandler("getfriends", std::bind(&this_type::OnGetFriendsMessage, this, std::placeholders::_1));
	m_MsgHandler.AddHandler("getuserinfo", std::bind(&this_type::OnGetUserInfoMessage, this, std::placeholders::_1));
	m_MsgHandler.AddHandler("chats", std::bind(&this_type::OnChatsMessage, this, std::placeholders::_1));
	m_MsgHandler.AddHandler("close", std::bind(&this_type::OnCloseMessage, this, std::placeholders::_1));
}

void ChatSession::Start()
{
	// ��ʼ������Ϣ
	AsyncReadMessage();
}

void ChatSession::AsyncReadMessage()
{
	memset(m_data, 0, max_length);

	auto self(shared_from_this());
	m_sock->async_read_some(buffer(m_data),
		[this, self](boost::system::error_code ec, std::size_t)
	{
		if (!ec)
		{
			// �ַ����������Ϣ
			this->DispatchMessage(m_data);
			
		}
	});
}

void ChatSession::AsyncWriteMessage(const ChatMessage& msg)
{
	std::string data = msg.GetData();

	// ������Ϣ
	auto self(shared_from_this());
	m_sock->async_write_some(buffer(data.data(), data.length()),
		[this, self](boost::system::error_code ec, std::size_t)
	{
		if (!ec)
		{
			this->AsyncReadMessage();
		}
	});
}

void ChatSession::AsyncWriteMessage(sock_ptr sock, const ChatMessage& msg)
{
	auto self(shared_from_this());
	sock->async_write_some(buffer(msg.GetData().data(), msg.GetData().length()),
		[this, self](boost::system::error_code ec, std::size_t)
	{
		if (!ec)
		{
			this->AsyncReadMessage();
		}
	});
}

void ChatSession::DispatchMessage(const std::string& data)
{
	// ������Ϣ
	ChatMessage msg;
	msg.SetData(data);

	// ������Ϣ���ͣ�������صĴ�����
	std::string msgType = msg.GetProperty("MsgType");
	m_MsgHandler.Dispatch(msgType, msg);
}

// ��¼��Ϣ
void ChatSession::OnLoginMessage(const ChatMessage& msg)
{
	ChatMessage sendMsg;

	try {
		auto table = DBFactory::Instance().Create("Accounts");
		table->Open("radish_chat.db");
		table->Create();

		// ��ȡ���ݹ������û���������
		std::string fromUserId = msg.GetProperty("FromUserId");
		std::string userPsw = msg.GetProperty("Content");

		// �����ݿ��ȡ�û���������
		DBParamType param;
		param.Put("account_id", fromUserId);
		table->Select(param);

		if (!param.Empty())
		{
			if (param.Get("password_hash") == userPsw)  // �ȶ�����
			{
				// ���浱ǰ��¼�Ŀͻ���ID��socket
				ClientsQueue::Instance().Put(fromUserId, m_sock);

				// ������ȷ��������֤�ɹ�
				sendMsg.SetProperty("ErrCode", 0);
				sendMsg.SetProperty("MsgType", "vrlogin");
				sendMsg.SetProperty("FromUserId", fromUserId);
				sendMsg.SetProperty("FromUserName", param.Get("account_name"));
				AsyncWriteMessage(sendMsg);

				// �����������Ϣ
				if (!MessageQueue::Instance().Empty())
				{
					// Ŀ�����ߣ�����δ�������Ϣ
					ChatMessage uhMsg = MessageQueue::Instance().Get(fromUserId);
					if (MessageQueue::Instance().LastError != GET_VALUE_FAILED)
					{
						AsyncWriteMessage(uhMsg);
					}
				}
			}
			else
			{
				// �������
				sendMsg.SetProperty("ErrCode", 1);
				sendMsg.SetProperty("MsgType", "vrlogin");
				sendMsg.SetProperty("ErrMsg", "Failed to log in, password is invalid.");
				AsyncWriteMessage(sendMsg);
			}
		}
		else
		{
			// �����ڸ��û�
			sendMsg.SetProperty("ErrCode", 1);
			sendMsg.SetProperty("MsgType", "vrlogin");
			sendMsg.SetProperty("ErrMsg", "Failed to log in, username is invalid");
			AsyncWriteMessage(sendMsg);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "OnLogin" << e.what() << std::endl;
		// ��¼ʧ�ܣ���������
		sendMsg.SetProperty("ErrCode", 1);
		sendMsg.SetProperty("MsgType", "vrlogin");
		sendMsg.SetProperty("ErrMsg", e.what());
		AsyncWriteMessage(sendMsg);
	}
}

// ע����Ϣ
void ChatSession::OnSignupMessage(const ChatMessage& msg)
{
	ChatMessage sendMsg;
	try {
		auto table = DBFactory::Instance().Create("Accounts");
		table->Open("radish_chat.db");
		table->Create();

		DBParamType param;
		param.Put("account_id", msg.GetProperty("FromUserId"));
		param.Put("account_name", msg.GetProperty("FromUserName"));
		param.Put("password_hash", msg.GetProperty("Content"));
		param.Put("salt", "@4712423");
		param.Put("email", msg.GetProperty("Email"));
		param.Put("whats_up", msg.GetProperty("Signature"));

		// ����û�
		table->Insert(param);

		// ע��ɹ�
		sendMsg.SetProperty("ErrCode", 0);
		sendMsg.SetProperty("MsgType", "vrsignup");
		AsyncWriteMessage(sendMsg);
	}
	catch (const std::exception& e)
	{
		std::cerr << "OnSignup" << e.what() << std::endl;

		// ע��ʧ��
		sendMsg.SetProperty("MsgType", "vrsignup");
		sendMsg.SetProperty("ErrCode", 1);
		sendMsg.SetProperty("ErrMsg", "Failed to sign up, the accout was exists.");
		AsyncWriteMessage(sendMsg);
	}
}

// ��Ӻ�����Ϣ
void ChatSession::OnAddFriendMessage(const ChatMessage& msg)
{
	try {
		// ��ȡ���ݹ����Ĳ���
		std::string toUserId = msg.GetProperty("ToUserId");
		std::string fromUserId = msg.GetProperty("FromUserId");

		// ����ӵĺ��ѵ�ǰ����
		sock_ptr sock = ClientsQueue::Instance().Get(toUserId);
		if (ClientsQueue::Instance().LastError != GET_VALUE_FAILED)
		{
			AsyncWriteMessage(sock, msg);
		}
		else
		{
			// ����ӵĺ��ѵ�ǰ�����ߣ����뵽δ������Ϣ������
			MessageQueue::Instance().Put(toUserId, std::move(msg));
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "OnAddFriend:" << e.what() << std::endl;
		ChatMessage sendMsg;
		sendMsg.SetProperty("ErrCode", 1);
		sendMsg.SetTextMessage("", "", "vraddfriend", e.what());
		AsyncWriteMessage(sendMsg);
	}
}

void ChatSession::OnVRAddFriendMessage(const ChatMessage& msg)
{
	// ��ȡ���ݹ����Ĳ���
	std::string toUserId = msg.GetProperty("ToUserId");
	std::string toUserName = msg.GetProperty("ToUserName");
	std::string fromUserId = msg.GetProperty("FromUserId");

	ChatMessage sendMsg;
	try {
		auto table = DBFactory::Instance().Create("Friends");
		table->Open("radish_chat.db");
		table->Create();

		// д�����
		DBParamType param;
		param.Put("account_id", fromUserId);
		param.Put("friend_id", toUserId);
		// ��Ӻ���
		table->Insert(param);

		// ����
		param.Clear();
		param.Put("account_id", toUserId);
		param.Put("friend_id", fromUserId);
		table->Insert(param);

		// ������֤������ͻ���
		sock_ptr sock = ClientsQueue::Instance().Get(toUserId);
		sendMsg.SetProperty("ErrCode", 0);
		sendMsg.SetProperty("FromUserName", toUserName);
		sendMsg.SetTextMessage(fromUserId, toUserId, "vraddfriend", "");
		AsyncWriteMessage(sendMsg);
		AsyncWriteMessage(sock, msg);
	}
	catch (const std::exception& e)
	{
		std::cerr << "OnVRAddFriend:" << e.what() << std::endl;
		sendMsg.SetProperty("ErrCode", 1);
		sendMsg.SetProperty("ErrMsg", e.what());
		AsyncWriteMessage(sendMsg);
	}
}

void ChatSession::OnGetFriendsMessage(const ChatMessage& msg)
{
	std::string fromUserId = msg.GetProperty("FromUserId");

	ChatMessage sendMsg;
	try {
		auto tableFriends = DBFactory::Instance().Create("Friends");
		tableFriends->Open("radish_chat.db");
		tableFriends->Create();

		DBParamType param;
		param.Put("account_id", fromUserId);
		tableFriends->Select(param);

		std::vector<std::string> friendsList;
		for (size_t i = 1; i < param.Size(); i += 2)
		{
			friendsList.push_back(param[i].second);
		}

		auto tableAccounts = DBFactory::Instance().Create("Accounts");
		tableAccounts->Open("radish_chat.db");

		// ��ѯ������Ϣ
		std::string content;
		for (auto& elem : friendsList)
		{
			param.Clear();
			param.Put("account_id", elem);
			tableAccounts->Select(param);
			
			content.append("[" + elem + "-" + param.Get("account_name") + "]");
		}

		sendMsg.SetProperty("MsgType", "vrgetfriends");
		sendMsg.SetProperty("Content", content);
		AsyncWriteMessage(sendMsg);
	}
	catch (const std::exception& e)
	{
		std::cerr << "OnGetFriendsMessage:" << e.what() << std::endl;
	}
}

void ChatSession::OnGetUserInfoMessage(const ChatMessage& msg)
{
	std::string toUserId = msg.GetProperty("ToUserId");

	ChatMessage sendMsg;
	try {
		auto table = DBFactory::Instance().Create("Accounts");
		table->Open("radish_chat.db");
		table->Create();

		DBParamType param;
		param.Put("account_id", toUserId);
		table->Select(param);

		sendMsg.SetProperty("AccountId", param.Get("account_id"));
		sendMsg.SetProperty("AccountName", param.Get("account_name"));
		sendMsg.SetProperty("Email", param.Get("email"));
		sendMsg.SetProperty("WhatsUp", param.Get("whats_up"));
		sendMsg.SetProperty("MsgType", "vrgetuserinfo");
		AsyncWriteMessage(sendMsg);
	}
	catch(const std::exception& e)
	{
		std::cerr << "OnGetUserInfoMessage:" << e.what() << std::endl;
	}
}

void ChatSession::OnChatsMessage(const ChatMessage& msg)
{	
	std::string toUserId = msg.GetProperty("ToUserId");
	ChatMessage sendMsg(msg);
	
	sock_ptr sock = ClientsQueue::Instance().Get(toUserId);
	if (ClientsQueue::Instance().LastError != GET_VALUE_FAILED)
	{
		AsyncWriteMessage(sock, sendMsg);
	}
	else
	{
		MessageQueue::Instance().Put(toUserId, std::move(sendMsg));
	}
}

void ChatSession::OnCloseMessage(const ChatMessage& msg)
{
	std::string fromUserId = msg.GetProperty("FromUserId");
	sock_ptr sock = ClientsQueue::Instance().Get(fromUserId);
	if (ClientsQueue::Instance().LastError != GET_VALUE_FAILED)
	{
		sock->close();
		ClientsQueue::Instance().Remove(fromUserId);
	}
}

}
