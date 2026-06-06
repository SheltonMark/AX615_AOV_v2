
#ifndef __BASICTOOL_STRTOK__
#define __BASICTOOL_STRTOK__

#include "public_include.h"

namespace bt
{
	class cstrtok
	{
	public:
		cstrtok(char* src, const char* split);			//会破坏src，但是不需要重新拷贝，性能好
		cstrtok(const char* src, const char* split);	//重新分配一段内存，拷贝src。再做SPLIT
		~cstrtok();

	public:
		const char* next();

	protected:


	private:
		char* _src;
		bool  _new_src;
		const char* _split;
		bool  _first;

#if defined(OS_LINUX)
		char* _save_str;
#endif
	};

}

#endif

