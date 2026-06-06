

#ifndef __BASICTOOL_CPU_RATE__
#define __BASICTOOL_CPU_RATE__

#include "public_include.h"

namespace bt
{
	class cpu_rate
	{
	public:
		cpu_rate();

	public:
		void start();
		uint get_rate();		//1000的倍数。如果是WINDOWS，还要处于CPU数

	protected:
		uint _get_last_cpu();
		uint _get_last_process_cpu();

	protected:
		uint _last_cpu;
		uint _last_process_cpu;
	};

}

#endif //__BASICTOOL_CPU_RATE__






