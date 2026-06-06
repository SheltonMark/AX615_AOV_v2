
#ifndef __PACKET_BUDDY_INC__
#define __PACKET_BUDDY_INC__

#include "basic_tool_mini.h"
#include "packet_buffer.h"





namespace packet_memory
{
	class buddy
	{
	public:
		buddy(void);
		~buddy(void);

	public:
		bool init(bt::nint mem_size, bt::nint init_size);
		bt::nint get_size();
		bt::nint get_left_size();

		bt::nint malloc(bt::nint size, CPacket*& pkt);									//申请固定大小的块
		bt::nint malloc_ex(bt::nint size, bt::nint max_block_size, CPacket*& pkt);			//申请空间，大小无限制，要么申请成功，要么失败。
		bt::nint malloc_batch(bt::nint block_size, bt::nint block_num, CPacket*& pkt);		//申请固定大小的多个块。可能申请的结果小于block_num。
		void free(CPacket* pkt);
		CPacket* malloc_empty_packet(bt::nint num);

		void dump();

	/////////////////////////////////////////////////////////////////////////////////
	protected:
		typedef struct _PBN{//CPacket buffer node
			_PBN *	pPrev;
			_PBN *	pNext;
			int		nIndex;
		}PBN;

		typedef struct _PBA{//CPacket buffer array
			PBN *	pArray;
			PBN *	pHeader;
			bt::nint	nCount;
		}PBA;

		bt::abyte *		m_pOriginBuffer;
		bt::abyte *		m_pBuffer;
		PBA 		m_PBAs[32];	//管理每种缓冲的节点数组
		int			m_nTypes;
		int			m_nPages;
		int			m_nLeftPages;

		packet_buffer m_pb;

	protected:
		void RemoveFree(PBN * & pHeader, PBN * pThis);
		void InsertFree(PBN * & pHeader, PBN * pThis);
		bt::nint GetFreeCount(int index);

	};

}

#endif
