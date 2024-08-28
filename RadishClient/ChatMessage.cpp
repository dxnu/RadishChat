#include "ChatMessage.h"


namespace Radish {
namespace Chat {


ChatMessage::ChatMessage()
{

}

ChatMessage::ChatMessage(const ptree& data)
	: m_pt(data)
{
}

void ChatMessage::SetData(const std::string& data)
{
	std::istringstream is(data);
	read_xml(is, m_pt);
}

const std::string ChatMessage::GetData() const
{
	std::ostringstream os;
	write_xml(os, m_pt);
	return os.str();
}

void ChatMessage::SetTextMessage(const std::string& toUserId, const std::string& fromUserId, const std::string& msgType, const std::string& content)
{
	if (!toUserId.empty()) m_pt.put("ToUserId", toUserId);
	if (!fromUserId.empty()) m_pt.put("FromUserId", fromUserId);
	if (!msgType.empty()) m_pt.put("MsgType", msgType);
	if (!content.empty()) m_pt.put("Content", content);
}

void ChatMessage::SetImageMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* picUrl, const char* mediaId)
{
	/*m_json["ToUserId"] = toUserId;
	m_json["FromUserId"] = fromUserId;
	m_json["MsgType"] = msgType;
	m_json["PicUrl"] = picUrl;
	m_json["MediaId"] = mediaId;*/
}

void ChatMessage::SetVoiceMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* mediaId, const char* format, const char* recongnition)
{
	/*m_json["ToUserId"] = toUserId;
	m_json["FromUserId"] = fromUserId;
	m_json["MsgType"] = msgType;
	m_json["MediaId"] = mediaId;
	m_json["Format"] = format;
	m_json["Recongnition"] = recongnition;*/
}

void ChatMessage::SetVideoMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* mediaId, const char* thumbMediaId)
{
	/*m_json["ToUserId"] = toUserId;
	m_json["FromUserId"] = fromUserId;
	m_json["MsgType"] = msgType;
	m_json["MediaId"] = mediaId;
	m_json["ThumbMediaId"] = thumbMediaId;*/
}

void ChatMessage::SetShortVideoMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* mediaId, const char* thumbMediaId)
{
	/*m_json["ToUserId"] = toUserId;
	m_json["FromUserId"] = fromUserId;
	m_json["MsgType"] = msgType;
	m_json["MediaId"] = mediaId;
	m_json["ThumbMediaId"] = thumbMediaId;*/
}

void ChatMessage::SetLocationMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* locationX, const char* locationY, const char* scale, const char* label)
{
	/*m_json["ToUserId"] = toUserId;
	m_json["FromUserId"] = fromUserId;
	m_json["MsgType"] = msgType;
	m_json["LocationX"] = locationX;
	m_json["LocationY"] = locationY;
	m_json["Scale"] = scale;
	m_json["Label"] = label;*/
}

void ChatMessage::SetLinkMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* title, const char* description, const char* url)
{
	/*m_json["ToUserId"] = toUserId;
	m_json["FromUserId"] = fromUserId;
	m_json["MsgType"] = msgType;
	m_json["Title"] = title;
	m_json["Description"] = description;
	m_json["Url"] = url;*/
}

} // namespace Chat
} // namespace Radish