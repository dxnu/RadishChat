#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>

using namespace boost::property_tree;


namespace Radish {
namespace Chat {


//////////////////////////////////////////////////////////////////////////
// class ChatMessage
// ������Ϣ��(Ĭ��Ϊxml��ʽ)
//////////////////////////////////////////////////////////////////////////
class ChatMessage
{
public:

	ChatMessage();
	ChatMessage(const ptree& data);

	// ���ö�������ֵ
	template<class T>
	void SetProperty(const char* key, T value)
	{
		m_pt.put(key, value);
	}

	// ��ȡ����ֵ
	template<typename ValueType = std::string>
	ValueType GetProperty(const char* key) const
	{
		return m_pt.get<ValueType>(key);
	}

	// ����jsonԭʼ����
	void SetData(const std::string& data);

	// ���json��ʽ��ԭʼ����
	const std::string GetData() const;

	// �ı���Ϣ
	void SetTextMessage(const std::string& toUserId, const std::string& fromUserId, const std::string& msgType, const std::string& content);

	// ͼ����Ϣ
	void SetImageMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* picUrl,
		const char* mediaId);

	// ������Ϣ
	void SetVoiceMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* mediaId,
		const char* format, const char* recongnition);

	// ��Ƶ��Ϣ
	void SetVideoMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* mediaId,
		const char* thumbMediaId);

	// С��Ƶ��Ϣ
	void SetShortVideoMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* mediaId,
		const char* thumbMediaId);

	// λ����Ϣ
	void SetLocationMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* locationX,
		const char* locationY, const char* scale, const char* label);

	// ������Ϣ
	void SetLinkMessage(const char* toUserId, const char* fromUserId, const char* msgType, const char* title,
		const char* description, const char* url);

private:
	ptree m_pt;
};


} // namespace Chat
} // namespace Radish