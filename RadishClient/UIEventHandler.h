#pragma once
#include "Common.h"
#include "Delegate.h"
#include <functional>


class EventHandler
{
	using delegate_type = Utils::Delegate<CControlUI*, std::function<void(TNotifyUI*)>>;
public:
	void AddHandler(CControlUI* pSender, std::function<void(TNotifyUI*)> callback)
	{
		delegate_.Insert(pSender, callback);
	}

	void Dispatch(TNotifyUI* pObj)
	{
		delegate_type::AssocMap::const_iterator iter;
		bool ret = delegate_.Find(pObj->pSender, iter);
		if (ret)
		{
			iter->second(pObj);
		}
	}

private:
	delegate_type delegate_;
};
