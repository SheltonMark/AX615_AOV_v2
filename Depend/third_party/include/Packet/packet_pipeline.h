
#ifndef __PACKET_PIPELINE_INC__
#define __PACKET_PIPELINE_INC__

#include "basic_tool_mini.h"
#include "packet_impl.h"
#include "packet_list.h"
#include "singleton.h"
#include "nolock_queue.h"

namespace packet_memory
{
	class packet_pipeline : public bt::singleton<packet_pipeline>
	{
	public:
		packet_pipeline();
		~packet_pipeline();

	public:
		//会堵塞
		struct ITEM
		{
			CPacketList* pktl;
			bt::nint8 chn;				//通道
			bt::sint8 stream_type;		//码流
			bt::sint8 use;				//用途
			bt::sint8 reserve;			//备用
		};
		void push(ITEM* item);
		void pop(ITEM* item);

		bt::nint size(){return _queue.size();}
		bt::nint max_size(){return _queue.max_size();}

	protected:
		bt::hydra_queue<ITEM> _queue;
	};
}

#endif
