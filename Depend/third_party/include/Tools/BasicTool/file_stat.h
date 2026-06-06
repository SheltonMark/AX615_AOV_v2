
#ifndef __BASICTOOL_FILE_STAT__
#define __BASICTOOL_FILE_STAT__

#include "public_include.h"
#include "btstring.h"
#include "strapi.h"
#include <sys/stat.h>

namespace bt
{

	class file_stat
	{
	public:
		file_stat();

	public:
		bool open_stat(const char* file_path);
		void copy(const file_stat& src);

		nint64			get_size();
		bool			is_dir();
		time_t			get_last_modify();

	protected:
		struct stat _stat;
	};
}

#endif

