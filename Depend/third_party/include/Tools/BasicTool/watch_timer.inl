
namespace bt
{

	inline watch_timer::watch_timer()
	{
		_last_active_time = BT_MAX_UINT;
		_interval = BT_MAX_UINT;
	}
	
	inline watch_timer::watch_timer(uint interval)
	{
		_last_active_time = BT_MAX_UINT;
		_interval = BT_MAX_UINT;

		reset(interval);
	}

	inline void watch_timer::reset(uint interval)
	{
		_last_active_time = get_tick();
		_interval = BT_MAX(1, interval);
	}

	inline uint watch_timer::get_interval()
	{
		return _interval;
	}

	inline bool watch_timer::check_active()
	{
		uint cur_time = get_tick();
		if ((cur_time - _last_active_time) > _interval)
		{
			uint jump = (cur_time - _last_active_time) / _interval;
			_last_active_time += jump * _interval;
			return true;
		}
		return false;
	}


}

