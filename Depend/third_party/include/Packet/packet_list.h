
#ifndef __PACKET_PACKET_LIST_INC__
#define __PACKET_PACKET_LIST_INC__

#include "basic_tool_mini.h"
#include "auto_ref.h"
#include "packet_impl.h"

namespace packet_memory
{

	/*
	CPacketList是CPacket的壳。内存使用和CPacket一样。提供列表操作相关的接口
	CPacketList不能访问CPacket接口
	*/

	class CPacketList : protected CPacket
	{
		friend class packet_manager;
		friend class packet_buffer;
		friend class buddy;
		friend class slab;

	public:
		CPacketList();

	public:
		//引用索引操作
		inline bt::nint	Release();
		inline bt::nint	AddRef();
		inline bt::nint	GetRef();

		//队列的操作
		void Append(CPacket* pkt);							//添加pkt
		void Append(CPacketList* pktl);						//合并列表，合并后pktl&pkt就不能再使用了
		inline const CPacket* FirstPacket() const;			//返回自身，不会返回NULL
		inline const CPacket* LastPacket() const;			//获得最后一块有数据的块
		CPacketList* DetachLeftBuffer(bt::nint min_cut);	//如果剩余的空间大，会剥离剩余的空间出来。前提是空余大小大于整体的1/2。min_cut：如果块小于等于这个大小就不切割
		static CPacketList* ToList(CPacket* pkt);			//把一个包转换成列表，列表里面只有一个包。不增加引用索引。转换后pkt不能再使用。

		//帧数据的操作
		void  Clear();										//清空里面的数据
		void  Write(const void* data, bt::nint len);		//为了性能，没有做有效性校验
		void  Write(bt::nint len);							//增加写偏移
		void  WriteAnti(bt::nint len);						//倒偏移。往回倒
		void  Write(bt::nint offset, const void* data, bt::nint len);//只写数据，不修改写偏移
		void  Write(const CPacket* pkt);					//写pkt、pkt->NextPacket...直到NULL。
		void  Write(const CPacket* pkt, bt::nint pkt_offset, bt::nint pkt_len);	//循环同上。从pkt_offset开始拷贝，只拷贝pkt_len
		void  Write(const CPacketList* pktl);				//只拷贝数据，不拷贝帧信息
		void  WriteFinish(int frame_type);					//会完善帧信息，产生CFrame结构，_expand_buf和CFrame共用内存。如果不是写帧数据，不需要调用这个接口。

		//帧相关接口，用于帧合并
		inline const CFrameSection*	FirstFrameSection() const;	//如果没有帧信息，返回NULL
		void  WriteFrame(const CPacketList* pktl);			//拷贝帧数据，和帧信息。调用前确保前一帧写完成。而且要确保剩余缓冲足够
		void  WriteFrame(const void* data, bt::nint len, int frame_type);

		//整个列表信息。如果列表里面才一帧，那下面返回的是一帧的信息
		//如果是多帧，剩余空间不是连续的
		inline bt::nint  GetDataLength() const;
		inline bt::nint  GetBufferLength() const;
		inline bt::nint  GetLeft() const;

		//对列表的操作
		typedef void (*ListOperateFunc)(void* data, bt::nint len, void* user_data);
		void  ListOperate(ListOperateFunc func, void* user_data);
		void  ListOperate(bt::nint offset, bt::nint len, ListOperateFunc func, void* user_data);

	protected:
		CPacket* _get_packet_from_off(bt::nint& offset);
		void _add_frame_section(CFrameSection* fsec);
		void _append_frame_section(CPacketList* pktl);
		void _write_finish_1(int frame_type);
		void _write_finish_2(int frame_type);
	};

	typedef bt::auto_ref<CPacketList> CPacketListPtr;
}

#include "packet_list.inl"

#endif //__PACKET_PACKET_INC__

