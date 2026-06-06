

#ifndef __BASICTOOL_FILE_FIND__
#define __BASICTOOL_FILE_FIND__

#include "public_include.h"
#include "btstring.h"
#include "strapi.h"
#include <sys/stat.h>

#if defined(OS_LINUX)
#include <dirent.h>
#endif

namespace bt
{
	class file_find
	{
	public:
		file_find();
		~file_find();

	public:
		bool find(const char* dir);
		bool find_next();
		void close();

		const char*		get_name();
		nint64			get_size();
		bool			is_dir();
		bool			is_dot();

	protected:
#if defined(OS_LINUX)
		void update_file_stat();
#endif

	protected:
		btstring _dir_path;
#if defined(OS_WIN32)
		HANDLE _handle;
		WIN32_FIND_DATA _find_Data;
#elif defined(OS_LINUX)
		DIR* _dir;
		struct dirent* _dirent;
		struct stat _file_info;
#endif
	};

}

#endif

