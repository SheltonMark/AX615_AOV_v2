

#ifndef __BASICTOOL_STRAPI_EX__
#define __BASICTOOL_STRAPI_EX__

#include "public_include.h"
#include "btstl_container.h"
#include "btstring.h"

namespace bt
{
	//字符串分割
	void split_string(const char* str, const char* split, btlist<btstring>& ret);
}


#endif
