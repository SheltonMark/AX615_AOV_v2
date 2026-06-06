
#ifndef __BASICTOOL_TIMER_KERNAL__
#define __BASICTOOL_TIMER_KERNAL__

#include "public_include.h"
#include "btstl_container.h"
#include "osapi.h"

namespace bt
{
	class timer_kernal_parent
	{
	public:
		virtual void on_kernal_active_time(uint64_t cur_time, uint64_t act_time, int data) = 0;
	};

	class timer_kernal_map
	{
	public:
		timer_kernal_map(timer_kernal_parent* parent);

	public:
		void add_timer(uint64_t act_time, int data);
		void del_timer(uint64_t act_time, int data);
		void check_active(uint64_t cur_time);

	protected:
		timer_kernal_parent* _parent;
		btmultimap<uint64_t, int> _data;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	class timer_kernal_hash
	{
	public:
		timer_kernal_hash(timer_kernal_parent* parent);

	public:
		void add_timer(uint64_t act_time, int data);
		void del_timer(uint64_t act_time, int data);
		void check_active(uint64_t cur_time);

		void set_option(uint max_interval, uint hash_count);

	protected:
		uint hash_interval();
		uint time_to_hash(uint64_t time);
		uint64_t adjust_time(uint64_t time);				//调整成筒的边界
		void active_time(uint64_t cur_time, uint hash);

	protected:
		timer_kernal_parent* _parent;
		struct timer_item
		{
			uint64_t act_adjust_time;
			uint64_t act_time;
			int data;
		};
		btvector<btdeque<timer_item> > _data;
		uint64_t _cur_time;
		uint _max_interval;
	};

}

#endif

