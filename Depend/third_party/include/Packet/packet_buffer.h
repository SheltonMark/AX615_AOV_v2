#ifndef __PACKET_PACKET_BUFFER_INC__
#define __PACKET_PACKET_BUFFER_INC__

#include "basic_tool_mini.h"
#include "packet_impl.h"

namespace packet_memory
{
	class CPacket;
	class buddy;
	class packet_buffer
	{
	public:
		packet_buffer(buddy* bd);

	public:
		CPacket* new_packet();
		CPacket* new_packet(bt::nint num);
		void free_packet(CPacket* pkt);

		bt::nint get_malloc_num();
		void dump();

	protected:
		bool _add_buffer(bt::nint num);
		void __add_buffer(void* buf, bt::nint len);

	protected:
		CPacket* _head;
		bt::nint _malloc_num;

		buddy* _bd;
		bool _malloc_from_bd;
	};
}

#endif
