
/*
作者：文昌鱼
时间：2010.10.08
说明：增加静态缓存数量，减少申请内存的频率
赋值操作符和构造函数是不继承的，要重新声明
*/

#ifndef __BASICTOOL_BTSTRING__
#define __BASICTOOL_BTSTRING__

#include "public_include.h"
#include "pool_allocator.h"
#include "stl.h"
#include "strapi.h"

namespace bt
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//std::string类的辅助接口
	template<class STRING_TYPE>
	inline void string_set(STRING_TYPE& str, const typename STRING_TYPE::value_type* str_buf, uint char_len/*字符数*/);
	template<class STRING_TYPE>
	inline void string_format(STRING_TYPE& str, const typename STRING_TYPE::value_type* format, ...);
	template<class STRING_TYPE>
	inline void string_format_large(STRING_TYPE& str, const typename STRING_TYPE::value_type* format, ...);
	template<class STRING_TYPE>
	inline void string_formatv(STRING_TYPE& str, const typename STRING_TYPE::value_type* format, va_list& vl);
	template<class STRING_TYPE>
	inline void string_formatv_large(STRING_TYPE& str, const typename STRING_TYPE::value_type* format, va_list& vl);
	template<class STRING_TYPE>
	inline void string_set_int(STRING_TYPE& str, int value);
	template<class STRING_TYPE>
	inline void string_set_uint(STRING_TYPE& str, uint value);
	template<class STRING_TYPE>
	inline void string_set_double(STRING_TYPE& str, double value, uint dot = -1);
	template<class STRING_TYPE>
	inline typename STRING_TYPE::value_type* string_get_buffer(STRING_TYPE& str);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class CHAR_TYPE>
	class basic_btstring : public std::basic_string<CHAR_TYPE, std::char_traits<CHAR_TYPE>, pool_allocator<CHAR_TYPE> >
	{
	public:
		typedef std::basic_string<CHAR_TYPE> std_class;
		typedef std::basic_string<CHAR_TYPE, std::char_traits<CHAR_TYPE>, pool_allocator<CHAR_TYPE> > my_parent_class;
		typedef basic_btstring<CHAR_TYPE> my_class;
		typedef const CHAR_TYPE* my_cstr;

	public:
		basic_btstring();
		basic_btstring(const my_class& other);
		basic_btstring(const my_parent_class& other);
		basic_btstring(const std_class& other);
		basic_btstring(my_cstr other);
		basic_btstring(my_cstr other, uint len);
		basic_btstring(int n);
		basic_btstring(uint u);
		basic_btstring(double f, uint dot = -1);

		basic_btstring<CHAR_TYPE>& operator=(const my_class& other);
		basic_btstring<CHAR_TYPE>& operator=(const my_parent_class& other);
		basic_btstring<CHAR_TYPE>& operator=(const std_class& other);
		basic_btstring<CHAR_TYPE>& operator=(my_cstr other);
		basic_btstring<CHAR_TYPE>& operator=(int n);
		basic_btstring<CHAR_TYPE>& operator=(uint u);

		void format(my_cstr format, ...);
		void format_large(my_cstr format, ...);
		void formatv(my_cstr format, va_list& vl);
		void formatv_large(my_cstr format, va_list& vl);

		void set_string(my_cstr str, uint char_len);		//不以0结尾的
		void set_double(double f, uint dot = -1);

		void make_lower();
		void make_upper();
		void trim();

		int  compare_nocase(my_cstr str) const;
		int  to_int() const;
		double to_double() const;

		CHAR_TYPE* buffer(nint size = 0);					//可写缓冲区
		void release_buffer();
	};

	typedef basic_btstring<char>		btstring;
#if defined(BT_SUPPORT_UNICODE)
	typedef basic_btstring<wchar_t>		wbtstring;
#endif
}

#include "btstring.inl"

#endif //__BASICTOOL_BTSTRING__

