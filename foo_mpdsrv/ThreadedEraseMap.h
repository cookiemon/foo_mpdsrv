#ifndef THREADDUMPSTER
#define THREADDUMPSTER

#include "MessageSender.h"
#include "MPDMessageHandler.h"
#include "SleepyThread.h"
#include <algorithm>
#include <map>
#include <vector>

namespace foo_mpdsrv
{
	template<typename _Kty, typename _Vty>
	class ThreadedEraseMap
		: public std::map<_Kty, _Vty>,
		public SleepyThread
	{
	private:
		std::vector<_Kty> _toErase;
	public:
		iterator erase ( const_iterator position )
		{
			erase(position->first);
			return end();
		}
		size_type erase ( const key_type& k )
		{
			_toErase.push_back(k);
			Wake();
			// TODO: more sensible return
			return 0;
		}
		iterator erase ( const_iterator first, const_iterator last )
		{
			std::for_each(first, last, [this](const value_type& i){
				erase(i.first);
			});
			return end();
		}

		virtual bool WakeProc(abort_callback& p_abort)
		{
			while(!_toErase.empty())
			{
				std::map<_Kty, _Vty>::erase(_toErase[0]);
				_toErase.erase(_toErase.begin());
			}
			return false;
		}
	};
}

#endif