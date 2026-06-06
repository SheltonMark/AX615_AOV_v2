
/*
寄生类。我们可以在其他类里面存些自己的数据
*/

#ifndef __BASICTOOL_PARASITIC__
#define __BASICTOOL_PARASITIC__

#include "public_include.h"
#include "callback.h"
#include "btstl_container.h"

namespace bt
{

	class parasitic_listener
	{
	public:
		virtual void on_host_delete(void* host, bt::uint index, void* data, ptr_int param) = 0;
	};
	typedef autoref_callback_variable<parasitic_listener, int> parasitic_listener_cb;

	class parasitic
	{
	public:
		parasitic(void* host = NULL, uint max_index = 1);
		~parasitic();

	public:
		void  set_parasitic(uint index, void* data, parasitic_listener_cb cb = NULL);
		void* get_parasitic(uint index);

	protected:
		struct item
		{
			void* data;
			parasitic_listener_cb cb;

			item()
			{
				data = NULL;
			}
		};
		btvector<item> _data;
		void* _host;
	};

}

#endif

