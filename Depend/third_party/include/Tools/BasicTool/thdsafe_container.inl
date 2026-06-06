

namespace bt
{

	template<class KEY, class TYPE, class LOCK_TYPE>
	refer_map<KEY, TYPE, LOCK_TYPE>::refer_map()
	{

	}
	template<class KEY, class TYPE, class LOCK_TYPE>
	refer_map<KEY, TYPE, LOCK_TYPE>::~refer_map()
	{
		clear();
	}

	template<class KEY, class TYPE, class LOCK_TYPE>
	void refer_map<KEY, TYPE, LOCK_TYPE>::insert(KEY key, TYPE* dat)
	{
		dat->inc_ref();

		auto_lock<LOCK_TYPE> l(_mtx);
		_data.insert(typename btmap<KEY, TYPE*>::value_type(key, dat));
	}

	template<class KEY, class TYPE, class LOCK_TYPE>
	TYPE* refer_map<KEY, TYPE, LOCK_TYPE>::find(KEY key)
	{
		auto_lock<LOCK_TYPE> l(_mtx);
		typename btmap<KEY, TYPE*>::iterator iter = _data.find(key);
		if (iter == _data.end())
		{
			return NULL;
		}
		TYPE* dat = iter->second;
		dat->inc_ref();
		return dat;
	}

	template<class KEY, class TYPE, class LOCK_TYPE>
	void refer_map<KEY, TYPE, LOCK_TYPE>::erase(KEY key)
	{
		auto_lock<LOCK_TYPE> l(_mtx);
		typename btmap<KEY, TYPE*>::iterator iter = _data.find(key);
		if (iter == _data.end())
		{
			return;
		}
		TYPE* dat = iter->second;
		_data.erase(iter);
		l.unlock();

		dat->dec_ref();
	}

	template<class KEY, class TYPE, class LOCK_TYPE>
	void refer_map<KEY, TYPE, LOCK_TYPE>::clear()
	{
		btmap<KEY, TYPE*> tmp;

		auto_lock<LOCK_TYPE> l(_mtx);
		tmp.swap(_data);
		l.unlock();

		for (typename btmap<KEY, TYPE*>::iterator iter = tmp.begin();iter != tmp.end();++iter)
		{
			iter->second->dec_ref();
		}
	}

	template<class KEY, class TYPE, class LOCK_TYPE>
	void refer_map<KEY, TYPE, LOCK_TYPE>::iterate_lock()
	{
		_mtx.lock();
	}
	template<class KEY, class TYPE, class LOCK_TYPE>
	typename refer_map<KEY, TYPE, LOCK_TYPE>::iterator refer_map<KEY, TYPE, LOCK_TYPE>::begin()
	{
		return _data.begin();
	}
	template<class KEY, class TYPE, class LOCK_TYPE>
	typename refer_map<KEY, TYPE, LOCK_TYPE>::iterator refer_map<KEY, TYPE, LOCK_TYPE>::end()
	{
		return _data.end();
	}
	template<class KEY, class TYPE, class LOCK_TYPE>
	void refer_map<KEY, TYPE, LOCK_TYPE>::iterate_unlock()
	{
		_mtx.unlock();
	}
}

