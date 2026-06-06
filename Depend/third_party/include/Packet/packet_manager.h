
#ifndef __PACKET_MANAGER_INC__
#define __PACKET_MANAGER_INC__

#include "basic_tool_mini.h"
#include "packet_impl.h"
#include "packet_list.h"
#include "buddy.h"
#include "slab.h"

namespace packet_memory
{

	class packet_manager_listener
	{
	public:
		virtual void on_free_packet() = 0;
	};

	class packet_manager : public bt::singleton<packet_manager>
	{
		friend class CPacket;
	public:
		packet_manager(void);
		~packet_manager(void);

	public:
		struct init_param
		{
			bt::nint mem_size;
			bt::nint init_size;
			bt::nint slab_size;		//0表示关闭SLAB

			//存放默认值
			init_param()
			{
				mem_size = 0;
				init_size = 0;
				slab_size = 32;
			}
		};
		bool init(init_param* param);
		bool uninit();
		bt::nint get_size();
		bt::nint get_left_size();
		void free_slab_buffer();

		CPacket* new_packet(bt::nint size, bool slab);			//slab是否使用SLAB算法
		CPacketList* new_packet_list(bt::nint size, bool slab);
		CPacket* new_empty_packet(bool slab = true);

		//监听
		void add_listener(packet_manager_listener* lsner);
		void del_listener(packet_manager_listener* lsner);

		//
		void dump();

	protected:
		CPacket* _new_packet(bt::nint size, bool one_block, bool slab);
		CPacket* __new_packet(bt::nint size, bool one_block, bool slab);
		void _free_packet(CPacket* pkt, bool slab);

	protected:
		slab _slab;

		//监听
		bt::mutex _mtx_lsner;
		std::set<packet_manager_listener*> _lsner_set;
	};

}

#endif
