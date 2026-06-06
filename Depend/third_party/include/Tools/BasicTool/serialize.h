
/*
作者：文昌鱼
时间：2010.07.24
说明：序列化类


时间：2010.07.31
说明：变成字节缓存类，这个更有用

*/


#ifndef __BASICTOOL_SERIALIZE__
#define __BASICTOOL_SERIALIZE__

#include "public_include.h"

namespace bt
{
	template<int STATIC_BUF_SIZE>
	class byte_buffer
	{
	public:
		byte_buffer();
		virtual ~byte_buffer();

	public:
		bool write(const void* data, uint len);
		template<class TYPE>
		inline bool write(const TYPE& data)
		{
			return write(&data, sizeof(TYPE));
		}
		template<class TYPE>
		inline byte_buffer& operator << (const TYPE& data)
		{
			write(data);
			return *this;
		}

		bool read(void* data, uint len);
		template<class TYPE>
		inline bool read(TYPE& data)
		{
			return read(&data, sizeof(data));
		}
		template<class TYPE>
		inline byte_buffer& operator >> (TYPE& data)
		{
			read(data);
			return *this;
		}

		void  clear(bool clear_dynamic_buf = false);
		void  adjust();									//把数据移到最开头

		bool  set_buf_size(uint size);
		inline char* get_buffer();

		inline char* get_data();
		inline uint  get_len();
		inline uint  get_read_pos();
		inline uint  get_write_pos();
		inline void  set_write_pos(uint pos);
		inline void  set_read_pos(uint pos);

	public:
		const static uint STATIC_BUF_SIZE_V = STATIC_BUF_SIZE;

	//缓存分配函数，可以重载。
	protected:
		virtual char* new_buffer(bt::uint& size);
		virtual void free_buffer(char* buf, bt::uint size);

	protected:
		char _static_buf[STATIC_BUF_SIZE];
		char* _dynamic_buf;
		uint _buf_size;
		uint _read;
		uint _write;
	};

	typedef byte_buffer<1> byte_buffer_1;
	typedef byte_buffer<256> byte_buffer_256;
	typedef byte_buffer<4*1024> byte_buffer_4k;


	class simple_byte_buffer
	{
	public:
		simple_byte_buffer();
		~simple_byte_buffer();

	public:
		bool write(const void* data, nint len);
		void remove(nint len);
		void clear(bool clear_dynamic_buf = false);

		char* get_data(){return _dynamic_buf;}
		nint  get_len(){return _data_size;}
		void  set_len(nint len){_data_size = len;}

		bool set_buf_size(uint size);

	protected:
		char* _dynamic_buf;
		uint _buf_size;
		uint _data_size;
	};
}

#include "serialize.inl"

#endif //__BASICTOOL_SERIALIZE__

