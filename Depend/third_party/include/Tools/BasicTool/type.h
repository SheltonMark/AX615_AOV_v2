
/*
作者：文昌鱼
时间：2010.06.12
说明：定义一些通用的基本类型，尽量使用C++的默认类型。

时间：2010.11.26
说明：增加不容易冲突，又容易书写的新类型


*/


#ifndef __BASICTOOL_TYPE__
#define __BASICTOOL_TYPE__

#include "define.h"

namespace bt
{
	//固定长度的INT类型
	//{
	typedef char				int8_t;
	typedef unsigned char		uint8_t;
	typedef uint8_t				byte_t;

	typedef short				int16_t;
	typedef unsigned short		uint16_t;

	typedef int					int32_t;
	typedef unsigned int		uint32_t;

	typedef long long			int64_t;
	typedef unsigned long long	uint64_t;

	typedef int32_t				bool32_t;
	//}

	//内置类型的缩写
	//{
	typedef unsigned int		uint;			//不加前缀与LINUX的uint有冲突。以后用nint代替
	typedef unsigned short		ushort;
	typedef unsigned long		ulong;
	typedef unsigned char		uchar;
	typedef const char*			cstr;
	//}

	//以上的类型容易起冲突，加域名前缀输入麻烦。重新定义
	//n是u的倒立，表示无符号型。s是signed，表示有符号型
	//{
	typedef int					sint;
	typedef short				sshort;
	typedef long				slong;
	typedef char				schar;
	typedef bt::uint			nint;
	typedef bt::ushort			nshort;
	typedef bt::ulong			nlong;
	typedef bt::uchar			nchar;
	typedef bt::int8_t			sint8;
	typedef bt::uint8_t			nint8;
	typedef bt::uint8_t			abyte;
	typedef bt::int16_t			sint16;
	typedef bt::uint16_t		nint16;
	typedef bt::int32_t			sint32;
	typedef bt::uint32_t		nint32;
	typedef bt::int64_t			sint64;
	typedef bt::uint64_t		nint64;
	//}

	//定义一个与指针等长的类型,定义在BT外面，主要是为了不需要加BT::
	//{
#ifdef BT_64
#ifdef OS_LINUX	//LINUX 64位下，LONG也是64位
	typedef long				ptr_int;
	typedef unsigned long		ptr_uint;
#else
	typedef long long			ptr_int;
	typedef unsigned long long	ptr_uint;
#endif
#else
	typedef long				ptr_int;
	typedef unsigned long		ptr_uint;
#endif
	//}

	//约定：字符串用char*，数据用abyte*

	//一种时间格式
	struct ZLTIME_T
	{
		nint32 sec:6;			//seconds (0 - 59)
		nint32 min:6;			//minutes (0 - 59)
		nint32 hour:5;			//hours (0 - 23)
		nint32 mday:5;			//day of the month (1 - 31)
		nint32 mon:4;			//month (1-12)
		nint32 year:6;			//year (0-63) (2000 - 2063)
	};
	typedef nint32 ZLTIME;

} //namespace bt


#endif //__BASICTOOL_TYPE__

