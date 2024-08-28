#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>

using namespace boost::property_tree;


namespace Radish {
namespace Chat {


//////////////////////////////////////////////////////////////////////////
// class ChatMessage
// 聊天消息类(默认为xml格式)
//////////////////////////////////////////////////////////////////////////
class ChatMessage
{
public:

	ChatMessage();
	ChatMessage(const ptree& data);

	// 设置额外属性值
	template<class T>
	void SetProperty(const char* key, T value)
	{
		m_pt.put(key, value);
	}

	// 获取属性值
	template<typename ValueType = std::string>
	ValueType GetProperty(const char* key) const
	{
		return m_pt.get<ValueType>(key);
	}

	// 设置json原始数据
	void SetData(const std::string& data);

	// 获得json格式的原始数据
	const std::string GetData() const;

	// 文本消息
	void SetTextMessage(const std::string& toUserId, const std::string& fromUserId, const std::string& msgType, const std::string& content);

	// 图像消息
	void SetImageMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* picUrl,
		const char* mediaId);

	// 语音消息
	void SetVoiceMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* mediaId,
		const char* format, const char* recongnition);

	// 视频消息
	void SetVideoMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* mediaId,
		const char* thumbMediaId);

	// 小视频消息
	void SetShortVideoMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* mediaId,
		const char* thumbMediaId);

	// 位置消息
	void SetLocationMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* locationX,
		const char* locationY, const char* scale, const char* label);

	// 链接消息
	void SetLinkMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* title,
		const char* description, const char* url);

private:
	ptree m_pt;
};


} // namespace Chat
} // namespace Radish