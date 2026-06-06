
#ifndef __BASICTOOL_FILE_PATH_API__
#define __BASICTOOL_FILE_PATH_API__

#include "public_include.h"
#include "btstring.h"

namespace bt
{
#define BT_IS_PATH_SEPARATE(c)			(((c) == '/') || ((c) == '\\'))
#define BT_PATH_SEPARATE				'/'
#define BT_FILE_EXT_PREFIX				'.'

	//目录的/后缀
	void add_dir_suffix(btstring& path);
	void del_dir_suffix(btstring& path);

	//
	void append_file_name(btstring& path, const char* name);
	btstring get_dir(const char* path);							//返回的值，不带后缀
	btstring get_file_name(const char* path);
	btstring get_file_title(const char* path);
	btstring get_file_ext(const char* path);

}

#endif


