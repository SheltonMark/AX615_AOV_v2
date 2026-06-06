
namespace bt
{
#if defined(OS_WIN32)
#	define BT_FORMAT_INT64			"%I64d"
#else
#	define BT_FORMAT_INT64			"%lld"
#endif

#if defined(OS_WIN32)
#	define BT_FORMAT_UINT64			"%I64u"
#else
#	define BT_FORMAT_UINT64			"%llu"
#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//内部函数
	template<class CHAR_TYPE>
	void btstring_vsnprintf(CHAR_TYPE* buf, uint len/*字符*/, const CHAR_TYPE* fmt, va_list& vl);
	template<>
	inline void btstring_vsnprintf(char* buf, uint len, const char* fmt, va_list& vl)
	{
		vsnprintf(buf, len, fmt, vl);
	}

	template<class CHAR_TYPE>
	void btstring_snprintf(CHAR_TYPE* buf, uint len/*字符*/, const CHAR_TYPE* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		btstring_vsnprintf(buf, len, fmt, ap);
		va_end(ap);
	}

	template<class CHAR_TYPE>
	uint btstring_len(const CHAR_TYPE* str);
	template<>
	inline uint btstring_len<char>(const char* str)
	{
		return (uint)strlen(str);
	}

	template<class CHAR_TYPE>
	const CHAR_TYPE* btstring_get_fmt_d();
	template<>
	inline const char* btstring_get_fmt_d<char>(){return "%d";}

	template<class CHAR_TYPE>
	const CHAR_TYPE* btstring_get_fmt_u();
	template<>
	inline const char* btstring_get_fmt_u<char>(){return "%u";}

	template<class CHAR_TYPE>
	const CHAR_TYPE* btstring_get_fmt_f();
	template<>
	inline const char* btstring_get_fmt_f<char>(){return "%f";}

	template<class CHAR_TYPE>
	const CHAR_TYPE* btstring_get_fmt_df();
	template<>
	inline const char* btstring_get_fmt_df<char>(){return "%%.%df";}


	//支持UNICODE的接口
#if defined(BT_SUPPORT_UNICODE)

	template<>
	inline void btstring_vsnprintf(wchar_t* buf, uint len, const wchar_t* fmt, va_list& vl)
	{
#if defined(OS_WIN32)
		_vsnwprintf(buf, len, fmt, vl);
#else
		vswprintf(buf, len, fmt, vl);
#endif
	}

	template<>
	inline uint btstring_len<wchar_t>(const wchar_t* str)
	{
		return (uint)wcslen(str);
	}

	template<>
	inline const wchar_t* btstring_get_fmt_d<wchar_t>(){return L"%d";}

	template<>
	inline const wchar_t* btstring_get_fmt_u<wchar_t>(){return L"%u";}

	template<>
	inline const wchar_t* btstring_get_fmt_f<wchar_t>(){return L"%f";}

	template<>
	inline const wchar_t* btstring_get_fmt_df<wchar_t>(){return L"%%.%df";}

#endif //BT_SUPPORT_UNICODE

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class STRING_TYPE>
	inline void string_set(STRING_TYPE& str, const typename STRING_TYPE::value_type* str_buf, uint char_len)
	{
		str.assign(str_buf, char_len);
	}

	template<class STRING_TYPE>
	inline void string_format(STRING_TYPE& str, const typename STRING_TYPE::value_type* format, ...)
	{
		va_list ap;
		va_start(ap, format);
		string_formatv(str, format, ap);
		va_end(ap);
	}

	template<class STRING_TYPE>
	inline void string_formatv(STRING_TYPE& str, const typename STRING_TYPE::value_type* format, va_list& vl)
	{
		typename STRING_TYPE::value_type buf[1024];		//最大1K
		buf[sizeof(buf)/sizeof(typename STRING_TYPE::value_type)-1] = 0;
		btstring_vsnprintf(buf, sizeof(buf)/sizeof(typename STRING_TYPE::value_type)-1, format, vl);
		str = buf;
	}

	template<class STRING_TYPE>
	inline void string_format_large(STRING_TYPE& str, const typename STRING_TYPE::value_type* format, ...)
	{
		va_list ap;
		va_start(ap, format);
		string_formatv_large(str, format, ap);
		va_end(ap);
	}

	template<class STRING_TYPE>
	inline void string_formatv_large(STRING_TYPE& str, const typename STRING_TYPE::value_type* format, va_list& vl)
	{
		typename STRING_TYPE::value_type buf[4096];		//最大4K
		buf[sizeof(buf)/sizeof(typename STRING_TYPE::value_type)-1] = 0;
		btstring_vsnprintf(buf, sizeof(buf)/sizeof(typename STRING_TYPE::value_type)-1, format, vl);
		str = buf;
	}

	template<class STRING_TYPE>
	inline void string_set_int(STRING_TYPE& str, int value)
	{
		string_format(str, btstring_get_fmt_d<typename STRING_TYPE::value_type>(), value);
	}

	template<class STRING_TYPE>
	inline void string_set_uint(STRING_TYPE& str, uint value)
	{
		string_format(str, btstring_get_fmt_u<typename STRING_TYPE::value_type>(), value);
	}

	template<class STRING_TYPE>
	inline void string_set_double(STRING_TYPE& str, double value, uint dot)
	{
		if (dot == -1)
		{
			string_format(str, btstring_get_fmt_f<typename STRING_TYPE::value_type>(), value);
		}
		else
		{
			typename STRING_TYPE::value_type fmt[32];
			fmt[31] = 0;
			btstring_snprintf(fmt, 31, btstring_get_fmt_df<typename STRING_TYPE::value_type>(), dot);
			string_format(str, fmt, value);
		}
	}
	
	template<class STRING_TYPE>
	inline typename STRING_TYPE::value_type* string_get_buffer(STRING_TYPE& str)
	{
		if (str.empty())
		{
			static typename STRING_TYPE::value_type tmp = 0;
			tmp = 0;
			return &tmp;
		}
		else
		{
			return &str.at(0);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>::basic_btstring()
	{
	}
	
	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>::basic_btstring(const my_class& other)
		:my_parent_class(other)
	{
	}

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>::basic_btstring(const my_parent_class& other)
		:my_parent_class(other)
	{
	}

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>::basic_btstring(const std_class& other)
		:my_parent_class(other.c_str())
	{
	}

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>::basic_btstring(my_cstr other)
		:my_parent_class(other)
	{
	}

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>::basic_btstring(my_cstr other, uint len)
	{
		set_string(other, len);
	}

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>::basic_btstring(int n)
	{
		operator=(n);
	}

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>::basic_btstring(uint u)
	{
		operator=(u);
	}

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>::basic_btstring(double f, uint dot)
	{
		set_double(f, dot);
	}

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>& basic_btstring<CHAR_TYPE>::operator=(const my_class& other)
	{
		my_parent_class::operator =(other);
		return (*this);
	}

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>& basic_btstring<CHAR_TYPE>::operator=(const my_parent_class& other)
	{
		my_parent_class::operator =(other);
		return (*this);
	}

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>& basic_btstring<CHAR_TYPE>::operator=(const std_class& other)
	{
		my_parent_class::operator =(other.c_str());
		return (*this);
	}

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>& basic_btstring<CHAR_TYPE>::operator=(my_cstr other)
	{
		my_parent_class::operator =(other);
		return (*this);
	}

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>& basic_btstring<CHAR_TYPE>::operator=(int n)
	{
		string_set_int(*this, n);
		return (*this);
	}

	template<class CHAR_TYPE>
	basic_btstring<CHAR_TYPE>& basic_btstring<CHAR_TYPE>::operator=(uint u)
	{
		string_set_uint(*this, u);
		return (*this);
	}

	template<class CHAR_TYPE>
	void basic_btstring<CHAR_TYPE>::format(my_cstr fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		formatv(fmt, ap);
		va_end(ap);
	}

	template<class CHAR_TYPE>
	void basic_btstring<CHAR_TYPE>::format_large(my_cstr fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		formatv_large(fmt, ap);
		va_end(ap);
	}
	template<class CHAR_TYPE>
	void basic_btstring<CHAR_TYPE>::formatv(my_cstr fmt, va_list& vl)
	{
		string_formatv(*this, fmt, vl);
	}

	template<class CHAR_TYPE>
	void basic_btstring<CHAR_TYPE>::formatv_large(my_cstr fmt, va_list& vl)
	{
		string_formatv_large(*this, fmt, vl);
	}
	template<class CHAR_TYPE>
	void basic_btstring<CHAR_TYPE>::set_string(my_cstr str, uint char_len)
	{
		string_set(*this, str, char_len);
	}

	template<class CHAR_TYPE>
	void basic_btstring<CHAR_TYPE>::set_double(double f, uint dot)
	{
		string_set_double(*this, f, dot);
	}

	template<class CHAR_TYPE>
	void basic_btstring<CHAR_TYPE>::make_lower()
	{
		bt::make_lower<CHAR_TYPE>(buffer(), (uint)my_parent_class::length());
	}

	template<class CHAR_TYPE>
	void basic_btstring<CHAR_TYPE>::make_upper()
	{
		bt::make_upper<CHAR_TYPE>(buffer(), (uint)my_parent_class::length());
	}

	template<class CHAR_TYPE>
	void basic_btstring<CHAR_TYPE>::trim()
	{
		uint new_len = bt::trim<CHAR_TYPE>(buffer(), (uint)my_parent_class::length());
		my_parent_class::resize(new_len);
	}

	template<class CHAR_TYPE>
	int basic_btstring<CHAR_TYPE>::compare_nocase(my_cstr str) const
	{
		return strcmp_nocase(my_parent_class::c_str(), str);
	}

	template<class CHAR_TYPE>
	int basic_btstring<CHAR_TYPE>::to_int() const
	{
		return bt::to_int<CHAR_TYPE>(my_parent_class::c_str());
	}

	template<class CHAR_TYPE>
	double basic_btstring<CHAR_TYPE>::to_double() const
	{
		return bt::to_double<CHAR_TYPE>(my_parent_class::c_str());
	}

	template<class CHAR_TYPE>
	CHAR_TYPE* basic_btstring<CHAR_TYPE>::buffer(nint size)
	{
		if (my_parent_class::length() < size)
		{
			my_parent_class::resize(size);
		}
		return string_get_buffer(*this);
	}

	template<class CHAR_TYPE>
	void basic_btstring<CHAR_TYPE>::release_buffer()
	{
		my_parent_class::resize(btstring_len(my_parent_class::c_str()));
	}
}

