
namespace packet_memory
{

	inline bt::nint	CPacket::Release()
	{
		return dec_ref();
	}

	inline bt::nint	CPacket::AddRef()
	{
		return inc_ref();
	}

	inline bt::nint	CPacket::GetRef()
	{
		return get_ref_count();
	}
	
	inline bt::abyte* CPacket::GetBuffer()
	{
		return _buffer;
	}

	inline const bt::abyte* CPacket::GetBuffer() const
	{
		return _buffer;
	}
	
	inline bt::nint	CPacket::GetBufferLength() const
	{
		return _buffer_length;
	}

	inline void CPacket::PutBuffer(const void* data, bt::nint len)
	{
		memcpy((bt::abyte*)_buffer + _data_length, data, len);
		_data_length += len;
	}

	inline bt::nint CPacket::GetDataLength() const
	{
		return _data_length;
	}

	inline void CPacket::SetDataLength(bt::nint length)
	{
		_data_length = length;
	}

	inline bt::nint CPacket::GetLeft() const
	{
		return _buffer_length - _data_length;
	}

	inline bt::abyte* CPacket::GetExpandBuffer() const
	{
		return _expand_buf;
	}
	inline bt::nint CPacket::GetExpandBufferLength() const
	{
		return sizeof(_expand_buf);
	}
	
	inline void CPacket::ClearExpandBuffer() const
	{
		memset(_expand_buf, 0, sizeof(_expand_buf));
	}

	inline const CPacket* CPacket::NextPacket() const
	{
		return _next;
	}

	inline void CPacket::_append_list(CPacket* pkt)
	{
		if (_next)
		{
			_tail->_next = pkt;
		}
		else
		{
			_next = pkt;
		}
		if (pkt->_next)
		{
			_tail = pkt->_tail;
		}
		else
		{
			_tail = pkt;
		}
	}
	
	inline bt::nint	CPacket::GetFrameSectionNum() const
	{
		return _frame_sec_num;
	}
	
	inline const CFrameSection* CPacket::GetFrameSection(bt::nint index) const
	{
		return &_frame_sec[index];
	}
}

