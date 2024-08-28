#include "Delegate.h"
#include "ChatMessage.h"

namespace Radish {
namespace Chat {

	class MessageHandler
	{
		using delegate_type = Utils::Delegate<std::string, std::function<void(ChatMessage)>>;
	public:
		void AddHandler(const std::string& msgType, std::function<void(ChatMessage)> callback)
		{
			delegate_.Insert(msgType, callback);
		}

		void Dispatch(const std::string& msgType, const ChatMessage& msg)
		{
			delegate_type::AssocMap::const_iterator iter;
			bool ret = delegate_.Find(msgType, iter);
			if (ret)
			{
				iter->second(msg);
			}
		}

	private:
		delegate_type delegate_;
	};


} // namespace Chat
} // namespace Radish