
namespace bt
{
	inline void strncpy_z(char* dest, const char* src, uint dest_buf_len)
	{
		if (dest_buf_len <= 0)
		{
			return;
		}
		strncpy(dest, src, dest_buf_len - 1);
		dest[dest_buf_len - 1] = '\0';
	}

	inline void strncat_z(char* dest, const char* src, uint dest_buf_len)
	{
		uint dest_len = (uint)strlen(dest);
		if (dest_buf_len <= dest_len)
		{
			//dest_buf_len==dest_len时，返回后的dest可能不以0结尾。但不考虑输入错误的情况下，输出正确。
			return;
		}
		strncpy(dest + dest_len, src, dest_buf_len - dest_len - 1);
		dest[dest_buf_len - 1] = '\0';
	}

	template<class TYPE>
	inline void make_lower(TYPE* str, uint len)
	{
		for (uint i = 0;i < len;i++)
		{
			if ((str[i] >= 'A') && (str[i] <= 'Z'))
			{
				str[i] -= 'A' - 'a';
			}
		}
	}

	template<class TYPE>
	inline void make_upper(TYPE* str, uint len)
	{
		for (uint i = 0;i < len;i++)
		{
			if ((str[i] >= 'a') && (str[i] <= 'z'))
			{
				str[i] += 'A' - 'a';
			}
		}
	}

	template<>
	inline int to_int<char>(const char* str)
	{
		return atoi(str);
	}
	template<>
	inline double to_double<char>(const char* str)
	{
		return atof(str);
	}

#if defined(BT_SUPPORT_UNICODE)
	template<>
	inline int to_int<wchar_t>(const wchar_t* str)
	{
		int ret;
		wscanf(str, "%d", &ret);
		return ret;
	}

	template<>
	inline double to_double<wchar_t>(const wchar_t* str)
	{
		double ret;
		wscanf(str, "%f", &ret);
		return ret;
	}
#endif

	template<>
	inline uint trim<char>(char* str, uint len)
	{
		uint begin = 0;
		for (;begin < len;begin++)
		{
			if (isspace((abyte)str[begin]) == 0)
			{
				break;
			}
		}
		uint end = len;
		for (;end > begin;end--)
		{
			if (isspace((abyte)str[end-1]) == 0)
			{
				break;
			}
		}
		if ((end - begin) < len)
		{
			memmove(str, str + begin, end - begin);
			str[end - begin] = 0;
		}
		return end - begin;
	}

#if defined(BT_SUPPORT_UNICODE)
	template<>
	inline uint trim<wchar_t>(wchar_t* str, uint len)
	{
		uint begin = 0;
		for (;begin < len;begin++)
		{
			if (iswspace((nint16)str[begin]) == 0)
			{
				break;
			}
		}
		uint end = len;
		for (;end > begin;end--)
		{
			if (iswspace((nint16)str[end-1]) == 0)
			{
				break;
			}
		}
		if ((end - begin) < len)
		{
			memmove(str, str + begin, end - begin);
			str[end - begin] = 0;
		}
		return end - begin;
	}
#endif

	template<>
	inline int strcmp_nocase<char>(const char* str1, const char* str2)
	{
#if defined(OS_WIN32)
		return ::_stricmp(str1, str2);
#elif defined(OS_LINUX)
		return ::strcasecmp(str1, str2);
#endif
	}

#if defined(BT_SUPPORT_UNICODE)
	template<>
	inline int strcmp_nocase<wchar_t>(const wchar_t* str1, const wchar_t* str2)
	{
#if defined(OS_WIN32)
		return ::_wcsicmp(str1, str2);
#elif defined(OS_LINUX)
		return ::wcscasecmp(str1, str2);
#endif
	}
#endif
}

