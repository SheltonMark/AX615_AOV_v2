
#include "bt_statistics.h"

namespace bt
{
	void sleep(uint msec)
	{
#if defined(BT_STATISTICS)
		g_statis_sleep_times++;
#endif

		if (msec == 0)
		{
#if defined(OS_WIN32)
			::SwitchToThread();
#elif defined(OS_LINUX)
			::pthread_yield();
#endif
		}
		else
		{
#if defined(OS_WIN32)
			::Sleep(msec);
#elif defined(OS_LINUX)
			::usleep(msec*1000);
#endif
		}
	}

	template<class TYPE>
	inline TYPE reverse_type(TYPE type)
	{
		std::reverse((char*)&type, (char*)&type + sizeof(type));
		return type;
	}

	template<>
	inline nint16 reverse_type(nint16 type)
	{
		nint16 ret;
		((nint8*)&ret)[0] = ((nint8*)&type)[1];
		((nint8*)&ret)[1] = ((nint8*)&type)[0];
		return ret;
	}

	template<>
	inline nint32 reverse_type(nint32 type)
	{
		nint32 ret;
		((nint8*)&ret)[0] = ((nint8*)&type)[3];
		((nint8*)&ret)[1] = ((nint8*)&type)[2];
		((nint8*)&ret)[2] = ((nint8*)&type)[1];
		((nint8*)&ret)[3] = ((nint8*)&type)[0];
		return ret;
	}
	
	template<class TYPE>
	inline TYPE net_to_host(TYPE data)
	{
#ifdef BT_BIG_ENDIAN
		return data;
#else
		return reverse_type(data);
#endif
	}
	
	template<class TYPE>
	inline TYPE host_to_net(TYPE data)
	{
#ifdef BT_BIG_ENDIAN
		return data;
#else
		return reverse_type(data);
#endif
	}


	inline uint log2(uint32_t v)
	{
		int ret = 0;
		if (v & 0xFFFF0000)
		{
			ret += 16;
			v >>= 16;
		}
		if (v & 0xFF00)
		{
			ret += 8;
			v >>= 8;
		}
		if (v & 0xF0)
		{
			ret += 4;
			v >>= 4;
		}
		if (v & 0x0C)
		{
			ret += 2;
			v >>= 2;
		}
		if (v & 0x02)
		{
			ret += 1;
			//v >>= 1;
		}
		return ret;
	}

	inline uint log2byte(abyte v)
	{
		int ret = 0;
		if (v & 0xF0)
		{
			ret += 4;
			v >>= 4;
		}
		if (v & 0x0C)
		{
			ret += 2;
			v >>= 2;
		}
		if (v & 0x02)
		{
			ret += 1;
			//v >>= 1;
		}
		return ret;
	}

	inline int compare_uint(uint a, uint b)
	{
		if (a == b)
		{
			return 0;
		}
		return ((a - b) < (BT_MAX_UINT / 2)) ? 1 : -1;
	}

	//template<class TYPE>
	//inline TYPE bt_min(TYPE a, TYPE b)
	//{
	//	return (a < b) ? a : b;
	//}

	//template<class TYPE>
	//inline TYPE bt_max(TYPE a, TYPE b)
	//{
	//	return (a > b) ? a : b;
	//}

	//template<class TYPE>
	//inline TYPE bt_range(TYPE n, TYPE m, TYPE v)
	//{
	//	return bt_max(n, bt_min(m, v));
	//}

} //namespace bt


