
#ifndef __BASICTOOL_FILE__
#define __BASICTOOL_FILE__

#include "public_include.h"
#include "btstring.h"

namespace bt
{
	class file
	{
	public:
		file();
		~file();

	public:
		bool open(const char* name, const char* mode);
		void close();
		nint write(const void* data, nint size);
		nint read(void* buf, nint size);
		void flush();

		nint64 get_size();
		bool is_open();
		bool is_eof();
		nint64 get_pos();

		//SEEK_CUR
		//SEEK_END
		//SEEK_SET
		bool seek(int type, nint64 off);

	protected:
		FILE* _handle;
		btstring _name;
	};
}

#endif

