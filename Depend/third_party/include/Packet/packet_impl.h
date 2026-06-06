
#ifndef __PACKET_PACKET_INC__
#define __PACKET_PACKET_INC__

#include "basic_tool_mini.h"
#include "auto_ref.h"
#include "packet_frame.h"

namespace packet_memory
{

	/*
	推荐遍历的例子

	void CPacket::FrameOperate(bt::nint offset, bt::nint len, FrameOperateFunc2 func, void* user_data)
	{
		CPacket* pkt = this;
		while (offset >= pkt->_header.DataLength)
		{
			offset -= pkt->_header.DataLength;
			pkt = pkt->_next;
		}

		bt::nint left_len = pkt->_header.DataLength - offset;
		bt::nint cpy_len = BT_MIN(len, left_len);
		(*func)((bt::abyte*)pkt->_header.Buffer + offset, cpy_len, user_data);

		if (cpy_len < len)
		{
			do
			{
				len -= cpy_len;

				pkt = pkt->_next;

				cpy_len = BT_MIN(len, pkt->_header.DataLength);
				(*func)((bt::abyte*)pkt->_header.Buffer, cpy_len, user_data);
			}
			while (cpy_len < len);
		}
	}

	*/

	enum
	{
		PACK_EXPEND_BUF_SIZE = 104,
	};

	class CPacketList;
	class CPacket : protected bt::refer
	{
		friend class packet_manager;
		friend class packet_buffer;
		friend class buddy;
		friend class slab;
		friend class CPacketList;
		friend class CFrame;
		friend class CFrameDataIter;
	public:
		CPacket();
		virtual ~CPacket();

	public:
		//引用索引操作
		inline bt::nint	Release();
		inline bt::nint	AddRef();
		inline bt::nint	GetRef();

		//针对本块的操作
		inline const bt::abyte* GetBuffer() const;								//返回第一块的缓存和大小
		inline bt::abyte*	GetBuffer();										//返回第一块的缓存和大小
		inline void			PutBuffer(const void* data, bt::nint len);			//单块的写
		void				PutBuffer(const CPacket* pkt);						//写pkt、pkt->NextPacket...直到NULL。
		void				PutBuffer(const CPacket* pkt, bt::nint pkt_offset, bt::nint pkt_len);	//循环同上。从pkt_offset开始拷贝，只拷贝pkt_len
		inline bt::nint		GetBufferLength() const;							//单块缓冲
		inline bt::nint		GetDataLength() const;
		inline void			SetDataLength(bt::nint length);
		inline bt::nint		GetLeft() const;

		//扩展结构体相关，给外部使用
		inline bt::abyte*	GetExpandBuffer() const;
		inline bt::nint		GetExpandBufferLength() const;
		inline void			ClearExpandBuffer() const;

		//列表遍历，遍历出来的数据，只读，不能写。
		inline const CPacket* NextPacket() const;								//如果有多个PACKET的话，通过这个接口获得，直到NULL

		//获取帧信息
		inline bt::nint		GetFrameSectionNum() const;
		inline const CFrameSection* GetFrameSection(bt::nint index) const;

	protected:
		virtual void delete_this(refer* my);
		void _init_packet(bool slab);											//初始化附属变量，刚申请完调用
		inline void _append_list(CPacket* pkt);

	protected:
		//缓冲管理
		bt::abyte*	_buffer;
		bt::nint	_buffer_length;
		bt::nint	_data_length;

		//列表管理
		CPacket* _next;
		CPacket* _tail;										//_next!=NULL，才有效

		//是否通过SLAB申请的，如果不是通过SLAB申请，释放也不用SLAB
		bool _from_slab;

		//写相关数据，指向最后写的块（即使最后一块写满了），而不是将要写的块。_write一直有效
		CPacket* _write;

		//列表统计数据
		bt::nint _list_buffer_length;
		bt::nint _list_data_length;

		//应用在包协议头的时候，需要104个字节的缓冲。这属于功能之外的东西，但是没办法
		union
		{
			BT_FALSE_CONST bt::abyte _expand_buf[PACK_EXPEND_BUF_SIZE];
			struct
			{
				CFrameSection* _first_frame_sec;
				CFrameSection* _tail_frame_sec;

				bt::nint _frame_sec_num;
				CFrameSection _frame_sec[4];
			};
		};
	};

	typedef bt::auto_ref<CPacket> CPacketPtr;
}

#include "packet_impl.inl"

#endif //__PACKET_PACKET_INC__

