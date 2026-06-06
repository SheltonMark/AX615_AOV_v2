

namespace bt
{

	template<class OBJECT_TYPE>
	pipeline<OBJECT_TYPE>::pipeline()
	{
		_callback = NULL;
		_user_param = 0;

		_wait_time = 0;

		_dealwith_count_ontimes = 100;
	}

	template<class OBJECT_TYPE>
	pipeline<OBJECT_TYPE>::~pipeline()
	{

	}

	template<class OBJECT_TYPE>
	int pipeline<OBJECT_TYPE>::heartbeat(bt::ptr_int param)
	{
		int wait_count = 0;
		uint sum_count = 0;
		const uint count_on_times = 11;

		while (true)
		{
			OBJECT_TYPE obj[count_on_times];	//动态
			uint count = _asy_list.pop(obj[0], count_on_times);
			if (count > 0)
			{
				for (uint i = 0;i < count;i++)
				{
					on_dealwith_object(obj[i], _user_param);
				}
				sum_count += count;
				if ((sum_count >= _dealwith_count_ontimes) || (count < count_on_times))
				{
					break;
				}
				//继续循环
			}
			else
			{
				//只等待一次
				if (wait_count > 0)
				{
					break;
				}
				wait_count++;

				if (_wait_time == 0)
				{
					break;
				}

				if (_asy_event.wait(_wait_time) == false)
				{
					//等待超时
					break;
				}
				//继续循环
			}
		}

		return (sum_count >= _dealwith_count_ontimes) ? 0 : BT_MAX(1, _wait_time);
	}

	template<class OBJECT_TYPE>
	void pipeline<OBJECT_TYPE>::on_dealwith_object(OBJECT_TYPE const& obj, bt::ptr_int param)
	{
		if (_callback)
		{
			_callback->on_dealwith_object(obj, param);
		}
	}

	template<class OBJECT_TYPE>
	void pipeline<OBJECT_TYPE>::post_object(const OBJECT_TYPE& obj, bool active)
	{
		if ((active == false) || (_wait_time == 0))
		{
			_asy_list.push(obj);
		}
		else
		{
			bool empty = _asy_list.empty();
			_asy_list.push(obj);
			if (empty)
			{
				_asy_event.active();
			}
		}

	}

	template<class OBJECT_TYPE>
	void pipeline<OBJECT_TYPE>::post_object(const OBJECT_TYPE& obj, uint count, bool active)
	{
		if ((active == false) || (_wait_time == 0))
		{
			_asy_list.push(obj, count);
		}
		else
		{
			bool empty = _asy_list.empty();
			_asy_list.push(obj, count);
			if (empty)
			{
				_asy_event.active();
			}
		}
	}

	template<class OBJECT_TYPE>
	void pipeline<OBJECT_TYPE>::dealwith_all_object()
	{
		while (heartbeat(0) == 0);
	}

	template<class OBJECT_TYPE>
	void pipeline<OBJECT_TYPE>::set_callback(pipeline_dealwith<OBJECT_TYPE>* cb, bt::ptr_int param)
	{
		_callback = cb;
		_user_param = param;
	}

	template<class OBJECT_TYPE>
	void pipeline<OBJECT_TYPE>::set_wait_time(uint time)
	{
		_wait_time = BT_RANGE(0, 100, time);
	}

	template<class OBJECT_TYPE>
	void pipeline<OBJECT_TYPE>::set_dealwith_count_ontimes(uint count)
	{
		_dealwith_count_ontimes = BT_MAX(count, 1);
	}

	template<class OBJECT_TYPE>
	void pipeline<OBJECT_TYPE>::set_max_object(uint size)
	{
		_asy_list.set_max_size(size);
	}

	template<class OBJECT_TYPE>
	int pipeline<OBJECT_TYPE>::get_object_size()
	{
		return _asy_list.size();
	}

}

