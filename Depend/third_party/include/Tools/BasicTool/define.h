
/*
作者：文昌鱼
时间：2010.06.12
说明：定义一些通用的宏。

时间：2011.02.28
说明：将min\max改成BT_MIN\BT_MAX，防止冲突

*/


#ifndef __BASICTOOL_DEFINE__
#define __BASICTOOL_DEFINE__

#include "compile_env.h"
#include "stl.h"
#include "../../PrintGrade.h"

namespace bt
{

//常量定义

#ifndef TRUE
#define TRUE				1
#endif

#ifndef FALSE
#define FALSE				0
#endif

#ifndef NULL
#define NULL				0
#endif

#define BT_MAX(a, b)			(((a) > (b)) ? (a) : (b))
#define BT_MIN(a, b)			(((a) < (b)) ? (a) : (b))
#define BT_RANGE(n, m, v)		BT_MAX((n), BT_MIN((m), (v)))
#define BT_ABS(v)				(((v) > 0) ? (v) : -(v))

#define BT_MAX_UINT				(~0u)
#define BT_MAX_INT				((int)(~0u >> 1))

#define BT_SAFE_DELETE(p)		if ((p)) {delete (p);(p) = NULL;}
#define BT_SAFE_RELEASE(p)		if ((p)) {(p)->dec_ref();(p) = NULL;}

#define BT_MAKE_UINT32(l, h)	((((bt::nint32)(h) & 0xFFFF) << 16) + ((bt::nint32)(l) & 0xFFFF))
#define BT_GET_LOW16(v)			((bt::nint16)((bt::nint32)(v) & 0xFFFF))
#define BT_GET_HIGH16(v)		((bt::nint16)(((bt::nint32)(v) >> 16) & 0xFFFF))

#define BT_GET_BIT(v, b)		(((v) >> (b)) & 0x01)
#define BT_IS_BIT(v, b)			(BT_GET_BIT(v, b) == 0x01)
#define BT_SET_BIT(v, b)		((v) | (1 << (b)))
#define BT_CLEAR_BIT(v, b)		((v) & ~(1 << (b)))
#define BT_BITMSK(b)			(1u << (b))
#define BT_BITMSK64(b)			(1ull << (b))

#if defined(OS_WIN32)
#define BT_THROW_DEFAULT_EXCEPTION()		{throw std::exception(__FILE__, __LINE__);}
#elif defined(OS_LINUX)
#define BT_THROW_DEFAULT_EXCEPTION()		{throw std::exception();}
#endif

#if defined(OS_WIN32)
#define BT_BANG()							{MessageBoxA(NULL, __FILE__ ":" __FUNCTION__, "BANG", 0);exit(-1);}
#elif defined(OS_LINUX)
#define BT_BANG()							{PRINT_TRACE("BANG:%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);exit(-1);}
#endif

//
#define BT_GET_REF_OFF(first, index)		(*(&(first) + (index)))

#define BT_GET_ENTRY(TYPE, mem, p)				(TYPE*)((char*)p - offsetof(TYPE, mem))
#define BT_GET_CONST_ENTRY(TYPE, mem, p)		(const TYPE*)((char*)p - offsetof(TYPE, mem))

//构造虚构函数
#define BT_CALL_CONSTRUCT(p, TYPE, param)	{::new ((void*)(p)) TYPE param;}
#define BT_CALL_DESTRUCTOR(p, TYPE)			{typedef TYPE my_type;void* p2 = p;((my_type*)p2)->~my_type();}

//CONST函数可以修改的变量
#define BT_FALSE_CONST						mutable

//未实现便签宏
#define BT_UNIMPLEMENT()					assert(0)

//跟踪
#define BT_TRACE_BUG()						PRINT_TRACE("tracebug:%s[%d][%s]\n", __FILE__, __LINE__, __FUNCTION__);
#define BT_TRACE_POINT()					PRINT_TRACE("tracepoint:%s[%d][%s]\n", __FILE__, __LINE__, __FUNCTION__);
#define BT_TRACE_POINT_EX(p)\
{\
	PRINT_TRACE("tracepoint:%s[%d][%s]:", __FILE__, __LINE__, __FUNCTION__);\
	PRINT_TRACE p;\
	PRINT_TRACE("\n");\
}

#define BT_TIMER_TRACE_POINT(time)\
{\
	static bt::watch_timer timer(time);\
	static bt::uint times = 0;\
	times++;\
	if (timer.check_active())\
	{\
		PRINT_TRACE("timer tracepoint:%s[%d][%s]:times=%d\n", __FILE__, __LINE__, __FUNCTION__, times);\
		times = 0;\
	}\
}

#define BT_TIMER_TRACE_POINT_EX(time, p)\
{\
	static bt::watch_timer timer(time);\
	static bt::uint times = 0;\
	times++;\
	if (timer.check_active())\
	{\
		PRINT_TRACE("timer tracepoint:%s[%d][%s]:times=%d:", __FILE__, __LINE__, __FUNCTION__, times);\
		PRINT_TRACE p;\
		PRINT_TRACE("\n");\
		times = 0;\
	}\
}

//间隔打印
#define BT_TIMER_PRINT(time, p)\
{\
	static bt::watch_timer timer(time);\
	if (timer.check_active())\
	{\
		PRINT_TRACE p;\
	}\
}

//无符号型比较，考虑数值越界
#define BT_UINT_COMPARE(a, b)		(((int)(a) - (int)(b)))
#define BT_UINT64_COMPARE(a, b)		(((long long)(a) - (long long)(b)))

//结构体对齐宏
//#if defined(OS_WIN32)
//#pragma pack(1)
//#define BYTE_ALIGN_1
//#elif defined(OS_LINUX)
//#define BYTE_ALIGN_1 __attribute__(packed)
//#endif

//#if defined(OS_WIN32)
//#pragma pack()
//#elif defined(OS_LINUX)
//#endif


} //namespace bt

#endif //__BASICTOOL_DEFINE__

