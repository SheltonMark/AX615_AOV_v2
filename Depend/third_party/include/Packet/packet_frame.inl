
namespace packet_memory
{
	inline int CFrameSection::GetFrameType() const
	{
		return _frame_type;
	}
	inline int CFrameSection::GetFrameLength() const
	{
		return _frame_length;
	}
	inline int CFrameSection::GetFrameFlag() const
	{
		return _frame_flag;
	}
	inline const bt::abyte* CFrameSection::GetBuffer() const
	{
		return _buffer;
	}
	inline bt::nint CFrameSection::GetDataLength() const
	{
		return _data_length;
	}

	inline const CFrameSection* CFrameSection::Next() const
	{
		return _next;
	}
}

