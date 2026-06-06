

namespace bt
{
	template<int STATIC_BUF_SIZE>
	byte_buffer<STATIC_BUF_SIZE>::byte_buffer()
	{
		_dynamic_buf = NULL;
		_buf_size = STATIC_BUF_SIZE;
		_read = 0;
		_write = 0;
	}
	template<int STATIC_BUF_SIZE>
	byte_buffer<STATIC_BUF_SIZE>::~byte_buffer()
	{
		if (_dynamic_buf)
		{
			free_buffer(_dynamic_buf, _buf_size);
		}
	}

	template<int STATIC_BUF_SIZE>
	bool byte_buffer<STATIC_BUF_SIZE>::write(const void* data, uint len)
	{
		if ((_write + len) > _buf_size)
		{
			//重新分配内存
			if (set_buf_size(_write + len) == false)
			{
				return false;
			}
		}
		memcpy(get_buffer() + _write, data, len);
		_write += len;
		return true;
	}

	template<int STATIC_BUF_SIZE>
	bool byte_buffer<STATIC_BUF_SIZE>::read(void* data, uint len)
	{
		if ((_write - _read) < len)
		{
			return false;
		}
		memcpy(data, get_buffer() + _read, len);
		_read += len;
		return true;
	}

	template<int STATIC_BUF_SIZE>
	void byte_buffer<STATIC_BUF_SIZE>::clear(bool clear_dynamic_buf)
	{
		_write = 0;
		_read = 0;

		if (clear_dynamic_buf)
		{
			if (_dynamic_buf)
			{
				free_buffer(_dynamic_buf, _buf_size);
				_dynamic_buf = NULL;
			}
			_buf_size = STATIC_BUF_SIZE;
		}
	}

	template<int STATIC_BUF_SIZE>
	void  byte_buffer<STATIC_BUF_SIZE>::adjust()
	{
		memmove(get_buffer(), get_buffer() + _read, _write - _read);
		_write = _write - _read;
		_read = 0;
	}

	template<int STATIC_BUF_SIZE>
	bool byte_buffer<STATIC_BUF_SIZE>::set_buf_size(uint size)
	{
		if (size > _buf_size)
		{
			uint new_size = _buf_size * 2;
			while (size > new_size)
			{
				new_size *= 2;
			}
			char* new_buf = new_buffer(new_size);
			if (new_buf == NULL)
			{
				return false;
			}
			memcpy(new_buf, get_buffer(), _buf_size);
			if (_dynamic_buf)
			{
				free_buffer(_dynamic_buf, _buf_size);
			}
			_dynamic_buf = new_buf;
			_buf_size = new_size;
		}
		return true;
	}

	template<int STATIC_BUF_SIZE>
	inline char* byte_buffer<STATIC_BUF_SIZE>::get_data()
	{
		return get_buffer() + _read;
	}
	template<int STATIC_BUF_SIZE>
	inline uint  byte_buffer<STATIC_BUF_SIZE>::get_len()
	{
		return _write - _read;
	}

	template<int STATIC_BUF_SIZE>
	inline char* byte_buffer<STATIC_BUF_SIZE>::get_buffer()
	{
		if (_buf_size > STATIC_BUF_SIZE)
		{
			return _dynamic_buf;
		}
		else
		{
			return _static_buf;
		}
	}

	template<int STATIC_BUF_SIZE>
	char* byte_buffer<STATIC_BUF_SIZE>::new_buffer(uint& size)
	{
		return new(std::nothrow) char[size];
	}
	template<int STATIC_BUF_SIZE>
	void byte_buffer<STATIC_BUF_SIZE>::free_buffer(char* buf, uint size)
	{
		delete[] buf;
	}

	template<int STATIC_BUF_SIZE>
	inline void byte_buffer<STATIC_BUF_SIZE>::set_write_pos(uint pos)
	{
		_write = BT_MIN(pos, _buf_size);
		set_read_pos(_read);
	}
	template<int STATIC_BUF_SIZE>
	inline void byte_buffer<STATIC_BUF_SIZE>::set_read_pos(uint pos)
	{
		_read = BT_MIN(pos, _write);
	}
	template<int STATIC_BUF_SIZE>
	inline uint byte_buffer<STATIC_BUF_SIZE>::get_read_pos()
	{
		return _read;
	}
	template<int STATIC_BUF_SIZE>
	inline uint byte_buffer<STATIC_BUF_SIZE>::get_write_pos()
	{
		return _write;
	}

}

