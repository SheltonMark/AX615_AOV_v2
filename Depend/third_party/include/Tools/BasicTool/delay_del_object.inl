
namespace bt
{
	template<class TYPE>
	delay_del_object<TYPE>::delay_del_object()
	{
		_timer.reset(2*1000);
	}

	template<class TYPE>
	delay_del_object<TYPE>::~delay_del_object()
	{
		//不会调用到子类的del_object。所以无效，也会崩溃
		//del_all();
	}

	template<class TYPE>
	int delay_del_object<TYPE>::heartbeat(ptr_int param)
	{
		if (_timer.check_active())
		{
			btlist<TYPE> tmp;

			auto_lock<dyna_lock> l(_mtx);
			tmp.swap(_obj2);
			_obj2.swap(_obj);
			l.unlock();

			for (typename btlist<TYPE>::iterator iter = tmp.begin();iter != tmp.end();++iter)
			{
				del_object(*iter);
			}
		}
		return (int)_timer.get_interval() / 2;
	}

	template<class TYPE>
	void delay_del_object<TYPE>::set_delay_interval(uint interval)
	{
		interval = BT_MAX(10, interval);
		_timer.reset(interval);
	}

	template<class TYPE>
	void delay_del_object<TYPE>::delay_del(TYPE obj)
	{
		auto_lock<dyna_lock> l(_mtx);
		_obj.push_back(obj);
	}

	template<class TYPE>
	void delay_del_object<TYPE>::del_all()
	{
		btlist<TYPE> tmp;
		btlist<TYPE> tmp2;

		auto_lock<dyna_lock> l(_mtx);
		tmp.swap(_obj);
		tmp2.swap(_obj2);
		l.unlock();

		for (typename btlist<TYPE>::iterator iter = tmp2.begin();iter != tmp2.end();++iter)
		{
			del_object(*iter);
		}
		for (typename btlist<TYPE>::iterator iter = tmp.begin();iter != tmp.end();++iter)
		{
			del_object(*iter);
		}
	}

	template<class TYPE>
	void delay_del_object<TYPE>::set_lock_type(dyna_lock::DYNA_LOCK_TYPE type)
	{
		_mtx.set_lock_type(type);
	}

	template<class TYPE>
	nint delay_del_object<TYPE>::get_obj_size()
	{
		return (nint)_obj.size() + (nint)_obj2.size();
	}
}

