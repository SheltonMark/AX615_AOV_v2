
namespace packet_memory
{
	inline bt::nint	CPacketList::Release()
	{
		return CPacket::Release();
	}

	inline bt::nint	CPacketList::AddRef()
	{
		return CPacket::AddRef();
	}

	inline bt::nint	CPacketList::GetRef()
	{
		return CPacket::GetRef();
	}

	inline const CPacket* CPacketList::FirstPacket() const
	{
		return (CPacket*)this;
	}
	
	inline const CPacket* CPacketList::LastPacket() const
	{
		return _write;
	}

	inline bt::nint CPacketList::GetDataLength() const
	{
		return _list_data_length;
	}

	inline bt::nint CPacketList::GetBufferLength() const
	{
		return _list_buffer_length;
	}

	inline bt::nint CPacketList::GetLeft() const
	{
		return _list_buffer_length - _list_data_length;
	}

	inline const CFrameSection* CPacketList::FirstFrameSection() const
	{
		return _first_frame_sec;
	}
}

