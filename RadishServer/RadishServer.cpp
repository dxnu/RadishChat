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

			// 会话处理
			std::make_shared<ChatSession>(sock)->Start();
		}

		// 接受下一个请求
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
	// 开始接收消息
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
			// 分发处理该条消息
			this->DispatchMessage(m_data);
			
		}
	});
}

void ChatSession::AsyncWriteMessage(const ChatMessage& msg)
{
	std::string data = msg.GetData();

	// 发送消息
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
	// 设置消息
	ChatMessage msg;
	msg.SetData(data);

	// 根据消息类型，调用相关的处理函数
	std::string msgType = msg.GetProperty("MsgType");
	m_MsgHandler.Dispatch(msgType, msg);
}

// 登录消息
void ChatSession::OnLoginMessage(const ChatMessage& msg)
{
	ChatMessage sendMsg;

	try {
		auto table = DBFactory::Instance().Create("Accounts");
		table->Open("radish_chat.db");
		table->Create();

		// 获取传递过来的用户名与密码
		std::string fromUserId = msg.GetProperty("FromUserId");
		std::string userPsw = msg.GetProperty("Content");

		// 从数据库获取用户名与密码
		DBParamType param;
		param.Put("account_id", fromUserId);
		table->Select(param);

		if (!param.Empty())
		{
			if (param.Get("password_hash") == userPsw)  // 比对密码
			{
				// 保存当前登录的客户端ID及socket
				ClientsQueue::Instance().Put(fromUserId, m_sock);

				// 密码正确，返回验证成功
				sendMsg.SetProperty("ErrCode", 0);
				sendMsg.SetProperty("MsgType", "vrlogin");
				sendMsg.SetProperty("FromUserId", fromUserId);
				sendMsg.SetProperty("FromUserName", param.Get("account_name"));
				AsyncWriteMessage(sendMsg);

				// 处理待处理消息
				if (!MessageQueue::Instance().Empty())
				{
					// 目标上线，处理未处理的消息
					ChatMessage uhMsg = MessageQueue::Instance().Get(fromUserId);
					if (MessageQueue::Instance().LastError != GET_VALUE_FAILED)
					{
						AsyncWriteMessage(uhMsg);
					}
				}
			}
			else
			{
				// 密码错误
				sendMsg.SetProperty("ErrCode", 1);
				sendMsg.SetProperty("MsgType", "vrlogin");
				sendMsg.SetProperty("ErrMsg", "Failed to log in, password is invalid.");
				AsyncWriteMessage(sendMsg);
			}
		}
		else
		{
			// 不存在该用户
			sendMsg.SetProperty("ErrCode", 1);
			sendMsg.SetProperty("MsgType", "vrlogin");
			sendMsg.SetProperty("ErrMsg", "Failed to log in, username is invalid");
			AsyncWriteMessage(sendMsg);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "OnLogin" << e.what() << std::endl;
		// 登录失败，其它错误
		sendMsg.SetProperty("ErrCode", 1);
		sendMsg.SetProperty("MsgType", "vrlogin");
		sendMsg.SetProperty("ErrMsg", e.what());
		AsyncWriteMessage(sendMsg);
	}
}

// 注册消息
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

		// 添加用户
		table->Insert(param);

		// 注册成功
		sendMsg.SetProperty("ErrCode", 0);
		sendMsg.SetProperty("MsgType", "vrsignup");
		AsyncWriteMessage(sendMsg);
	}
	catch (const std::exception& e)
	{
		std::cerr << "OnSignup" << e.what() << std::endl;

		// 注册失败
		sendMsg.SetProperty("MsgType", "vrsignup");
		sendMsg.SetProperty("ErrCode", 1);
		sendMsg.SetProperty("ErrMsg", "Failed to sign up, the accout was exists.");
		AsyncWriteMessage(sendMsg);
	}
}

// 添加好友消息
void ChatSession::OnAddFriendMessage(const ChatMessage& msg)
{
	try {
		// 获取传递过来的参数
		std::string toUserId = msg.GetProperty("ToUserId");
		std::string fromUserId = msg.GetProperty("FromUserId");

		// 被添加的好友当前在线
		sock_ptr sock = ClientsQueue::Instance().Get(toUserId);
		if (ClientsQueue::Instance().LastError != GET_VALUE_FAILED)
		{
			AsyncWriteMessage(sock, msg);
		}
		else
		{
			// 被添加的好友当前不在线，加入到未处理消息队列中
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
	// 获取传递过来的参数
	std::string toUserId = msg.GetProperty("ToUserId");
	std::string toUserName = msg.GetProperty("ToUserName");
	std::string fromUserId = msg.GetProperty("FromUserId");

	ChatMessage sendMsg;
	try {
		auto table = DBFactory::Instance().Create("Friends");
		table->Open("radish_chat.db");
		table->Create();

		// 写入参数
		DBParamType param;
		param.Put("account_id", fromUserId);
		param.Put("friend_id", toUserId);
		// 添加好友
		table->Insert(param);

		// 互加
		param.Clear();
		param.Put("account_id", toUserId);
		param.Put("friend_id", fromUserId);
		table->Insert(param);

		// 发送验证结果给客户端
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

		// 查询好友信息
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
