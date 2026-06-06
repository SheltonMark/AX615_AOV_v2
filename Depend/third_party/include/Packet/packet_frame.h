
#ifndef __PACKET_PACKET_FRAME_INC__
#define __PACKET_PACKET_FRAME_INC__

#include "basic_tool_mini.h"

namespace packet_memory
{
	enum PackTypeFlag
	{
		PACK_TYPE_FRAME_P=0,
		PACK_TYPE_FRAME_I,
		PACK_TYPE_FRAME_B,
		PACK_TYPE_FRAME_AUDIO,
		PACK_TYPE_FRAME_JEPG,
		PACK_TYPE_FRAME_WATER,
		PACK_TYPE_FRAME_MOTION,
		PACK_TYPE_FRAME_MJPEG,
		PACK_TYPE_FRAME_FILL,		//填充帧，用来填充1K对齐
		PACK_TYPE_FRAME_METADATA,	//元数据帧
		PACK_TYPE_FRAME_IVS_ASSIST,	//智能分析帧
		PACK_TYPE_FRAME_NUM
	};

	enum FrameBeginEndFlag
	{
		PACK_CONTAIN_FRAME_NONHT = 0,
		PACK_CONTAIN_FRAME_HEAD,
		PACK_CONTAIN_FRAME_TAIL,
		PACK_CONTAIN_FRAME_HEADTAIL,
		PACK_CONTAIN_FRAME_NUM,
	};

	class CFrameSection
	{
		friend class CPacketList;
		friend class CPacket;
	public:
		inline int			GetFrameType() const;
		inline int			GetFrameLength() const;
		inline int			GetFrameFlag() const;
		inline const bt::abyte*	GetBuffer() const;
		inline bt::nint		GetDataLength() const;

		inline const CFrameSection* Next() const;

	protected:
		bt::abyte	_frame_type;
		bt::abyte	_frame_flag;
		bt::abyte*  _buffer;
		bt::nint	_data_length;
		bt::nint	_frame_length;

		CFrameSection* _next;
	};
}

#include "packet_frame.inl"

#endif
