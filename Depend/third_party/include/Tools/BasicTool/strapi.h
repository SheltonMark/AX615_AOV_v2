
#ifndef __BASICTOOL_STRAPI__
#define __BASICTOOL_STRAPI__

#include "public_include.h"

namespace bt
{
	inline void strncpy_z(char* dest, const char* src, uint dest_buf_len);			//字符串后面自动加0，即使超出缓冲区
	inline void strncat_z(char* dest, const char* src, uint dest_buf_len);

	template<class TYPE>
	inline void make_lower(TYPE* str, uint len);
	template<class TYPE>
	inline void make_upper(TYPE* str, uint len);

	template<class TYPE>
	inline int to_int(const TYPE* str);
	template<class TYPE>
	inline double to_double(const TYPE* str);

	template<class TYPE>
	inline uint trim(TYPE* str, uint len);

	template<class TYPE>
	inline int strcmp_nocase(const TYPE* str1, const TYPE* str2);

	//比特转成16进制字符串
	void bit_to_hex_str(const unsigned char* bit, uint len, char* str);
	void hex_str_to_bit(const char* str, uint len, unsigned char* bit);

	//跨平台格式化
	#if defined(OS_LINUX)
	#define bt_snprintf snprintf
	#elif defined(OS_WIN32)
	#define bt_snprintf _snprintf
	#endif

}

#include "strapi.inl"

#endif

