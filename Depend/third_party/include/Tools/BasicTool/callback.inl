#include "global_delay_del_obj.h"


namespace bt
{

	//////////////////////////////////////////////////////////////////////////////////////////
	template<class LSNER_TYPE>
	inline callback_variable<LSNER_TYPE>::callback_variable(LSNER_TYPE* lsner)
		:_lsner(lsner)
	{

	}
		
	template<class LSNER_TYPE>
	inline callback_variable<LSNER_TYPE>::callback_variable(const callback_variable& cbv)
		:_lsner(cbv._lsner)
	{

	}

	template<class LSNER_TYPE>
	inline void callback_variable<LSNER_TYPE>::set_lsner(LSNER_TYPE* lsner)
	{
		_lsner = lsner;
	}

	template<class LSNER_TYPE>
	inline void* callback_variable<LSNER_TYPE>::get_lsner() const
	{
		return _lsner;
	}

	template<class LSNER_TYPE>
	inline callback_variable<LSNER_TYPE>& callback_variable<LSNER_TYPE>::operator=(const callback_variable& cbv)
	{
		_lsner = cbv._lsner;
		return *this;
	}

	template<class LSNER_TYPE>
	inline callback_variable<LSNER_TYPE>::operator void* () const
	{
		return _lsner;
	}

	//
	template<class LSNER_TYPE>
	inline call_callback<LSNER_TYPE>::call_callback(const callback_variable<LSNER_TYPE>& cbv)
		:_lsner(cbv._lsner)
	{

	}

	template<class LSNER_TYPE>
	inline call_callback<LSNER_TYPE>::call_callback(const call_callback& ccb)
		:_lsner(ccb._lsner)
	{
		
	}

	template<class LSNER_TYPE>
	inline call_callback<LSNER_TYPE>::call_callback()
		:_lsner(NULL)
	{

	}

	template<class LSNER_TYPE>
	inline LSNER_TYPE* call_callback<LSNER_TYPE>::operator->() const
	{
		assert(_lsner);
		return _lsner;
	}

	template<class LSNER_TYPE>
	inline call_callback<LSNER_TYPE>::operator LSNER_TYPE* () const
	{
		return _lsner;
	}

	template<class LSNER_TYPE>
	inline call_callback<LSNER_TYPE>& call_callback<LSNER_TYPE>::operator=(const call_callback& ccb)
	{
		_lsner = ccb._lsner;
		return *this;
	}

	template<class LSNER_TYPE>
	inline call_callback<LSNER_TYPE>& call_callback<LSNER_TYPE>::operator=(const callback_variable<LSNER_TYPE>& cbv)
	{
		_lsner = cbv._lsner;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////////////////////

	template<class LSNER_TYPE, class DATA_TYPE>
	inline callback_variable_ex<LSNER_TYPE, DATA_TYPE>::callback_variable_ex(LSNER_TYPE* lsner, DATA_TYPE data, refer* ref)
		:_lsner(lsner), _data(data), _ref(ref)
	{

	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline callback_variable_ex<LSNER_TYPE, DATA_TYPE>::callback_variable_ex(const callback_variable_ex& cbv)
		:_lsner(cbv._lsner), _data(cbv._data), _ref(cbv._ref)
	{

	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline void callback_variable_ex<LSNER_TYPE, DATA_TYPE>::set_lsner(LSNER_TYPE* lsner, DATA_TYPE data, refer* ref)
	{
		_lsner = lsner;
		_data = data;
		_ref = ref;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline void* callback_variable_ex<LSNER_TYPE, DATA_TYPE>::get_lsner() const
	{
		return _lsner;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline DATA_TYPE& callback_variable_ex<LSNER_TYPE, DATA_TYPE>::get_data()
	{
		return _data;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline const DATA_TYPE& callback_variable_ex<LSNER_TYPE, DATA_TYPE>::get_data() const
	{
		return _data;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline refer* callback_variable_ex<LSNER_TYPE, DATA_TYPE>::get_refer() const
	{
		return _ref;
	}


	template<class LSNER_TYPE, class DATA_TYPE>
	inline void callback_variable_ex<LSNER_TYPE, DATA_TYPE>::inc_ref() const
	{
		if (_ref)
		{
			_ref->inc_ref();
		}
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline void callback_variable_ex<LSNER_TYPE, DATA_TYPE>::dec_ref() const
	{
		if (_ref)
		{
			_ref->dec_ref();
		}
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline callback_variable_ex<LSNER_TYPE, DATA_TYPE>& callback_variable_ex<LSNER_TYPE, DATA_TYPE>::operator=(const callback_variable_ex& cbv)
	{
		_lsner = cbv._lsner;
		_data = cbv._data;
		_ref = cbv._ref;
		return *this;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline callback_variable_ex<LSNER_TYPE, DATA_TYPE>::operator void* () const
	{
		return _lsner;
	}

	//
	template<class LSNER_TYPE, class DATA_TYPE>
	inline call_callback_ex<LSNER_TYPE, DATA_TYPE>::call_callback_ex(const callback_variable_ex<LSNER_TYPE, DATA_TYPE>& cbv)
		:_lsner(cbv._lsner), _data(cbv._data), _ref(cbv._ref)
	{
		
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline call_callback_ex<LSNER_TYPE, DATA_TYPE>::call_callback_ex(const call_callback_ex& ccb)
		:_lsner(ccb._lsner), _data(ccb._data), _ref(ccb._ref)
	{

	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline call_callback_ex<LSNER_TYPE, DATA_TYPE>::call_callback_ex()
		:_lsner(NULL), _data(0), _ref(NULL)
	{

	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline const DATA_TYPE& call_callback_ex<LSNER_TYPE, DATA_TYPE>::get_data() const
	{
		return _data;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline void call_callback_ex<LSNER_TYPE, DATA_TYPE>::inc_ref() const
	{
		if (_ref)
		{
			_ref->inc_ref();
		}
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline void call_callback_ex<LSNER_TYPE, DATA_TYPE>::dec_ref() const
	{
		if (_ref)
		{
			_ref->dec_ref();
		}
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline LSNER_TYPE* call_callback_ex<LSNER_TYPE, DATA_TYPE>::operator->() const
	{
		assert(_lsner);
		return _lsner;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline call_callback_ex<LSNER_TYPE, DATA_TYPE>::operator LSNER_TYPE* () const
	{
		return _lsner;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline call_callback_ex<LSNER_TYPE, DATA_TYPE>& call_callback_ex<LSNER_TYPE, DATA_TYPE>::operator=(const call_callback_ex& ccb)
	{
		_lsner = ccb._lsner;
		_data = ccb._data;
		_ref = ccb._ref;
		return *this;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline call_callback_ex<LSNER_TYPE, DATA_TYPE>& call_callback_ex<LSNER_TYPE, DATA_TYPE>::operator=(const callback_variable_ex<LSNER_TYPE, DATA_TYPE>& cbv)
	{
		_lsner = cbv._lsner;
		_data = cbv._data;
		_ref = cbv._ref;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////////////////////

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	inline autoref_callback_variable<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::autoref_callback_variable(LSNER_TYPE* lsner, DATA_TYPE data, refer* ref, bool inc_ref)
		:_lsner(NULL), _data(0), _ref(NULL)
	{
		set_lsner(lsner, data, ref, inc_ref);
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	inline autoref_callback_variable<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::autoref_callback_variable(const autoref_callback_variable& cbv)
		:_lsner(NULL), _data(0), _ref(NULL)
	{
		set_lsner(cbv._lsner, cbv._data, cbv._ref, true);
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	inline autoref_callback_variable<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::~autoref_callback_variable()
	{
		BT_SAFE_RELEASE(_ref);
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	void autoref_callback_variable<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::set_lsner(LSNER_TYPE* lsner, DATA_TYPE data, refer* ref, bool inc_ref)
	{
		//
		if (inc_ref && ref)
		{
			ref->inc_ref();
		}

		//
		auto_lock<LOCK_TYPE> l(_mtx);

		refer* old_ref = _ref;

		_lsner = lsner;
		_ref = ref;
		_data = data;

		l.unlock();

		//
		if (old_ref)
		{
			old_ref->dec_ref();
		}
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	inline const DATA_TYPE& autoref_callback_variable<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::get_data() const
	{
		return _data;
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	inline void* autoref_callback_variable<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::get_lsner() const
	{
		return _lsner;
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	inline autoref_callback_variable<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>& autoref_callback_variable<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::operator=(const autoref_callback_variable& cb)
	{
		set_lsner(cb._lsner, cb._data, cb._ref);
		return *this;	
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	inline autoref_callback_variable<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::operator void* () const
	{
		return _lsner;
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	call_autoref_callback<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::call_autoref_callback(const autoref_callback_variable<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>& cbv)
	{
		auto_lock<LOCK_TYPE> l(cbv._mtx);
		_lsner = cbv._lsner;
		_ref = cbv._ref;
		_data = cbv._data;

		//增索引要在锁里面。在锁里面才能保证值有效
		if (_ref)
		{
			_ref->inc_ref();
		}
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	call_autoref_callback<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::call_autoref_callback(const call_autoref_callback& ccb)
	{
		_lsner = ccb._lsner;
		_data = ccb._data;
		_ref = ccb._ref;

		if (_ref)
		{
			_ref->inc_ref();
		}
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	inline call_autoref_callback<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::call_autoref_callback()
		:_lsner(NULL), _data(0), _ref(NULL)
	{

	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	inline call_autoref_callback<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::~call_autoref_callback()
	{
		BT_SAFE_RELEASE(_ref);
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	inline const DATA_TYPE& call_autoref_callback<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::get_data() const
	{
		return _data;
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	inline LSNER_TYPE* call_autoref_callback<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::operator->() const
	{
		assert(_lsner);
		return _lsner;
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	inline call_autoref_callback<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::operator LSNER_TYPE* () const
	{
		return _lsner;
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	call_autoref_callback<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>& call_autoref_callback<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::operator=(const call_autoref_callback& ccb)
	{
		BT_SAFE_RELEASE(_ref);

		_lsner = ccb._lsner;
		_data = ccb._data;
		_ref = ccb._ref;

		if (_ref)
		{
			_ref->inc_ref();
		}
		return *this;
	}

	template<class LSNER_TYPE, class DATA_TYPE, class LOCK_TYPE>
	call_autoref_callback<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>& call_autoref_callback<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>::operator=(const autoref_callback_variable<LSNER_TYPE, DATA_TYPE, LOCK_TYPE>& cbv)
	{
		BT_SAFE_RELEASE(_ref);

		auto_lock<LOCK_TYPE> l(cbv._mtx);
		_lsner = cbv._lsner;
		_ref = cbv._ref;
		_data = cbv._data;

		//增索引要在锁里面。在锁里面才能保证值有效
		if (_ref)
		{
			_ref->inc_ref();
		}
		return *this;
	}


	//////////////////////////////////////////////////////////////////////////////////////////

	template<class LSNER_TYPE, class DATA_TYPE>
	inline delayref_callback_variable<LSNER_TYPE, DATA_TYPE>::delayref_callback_variable(LSNER_TYPE* lsner, DATA_TYPE data, refer* ref, bool inc_ref)
		:_lsner(NULL), _data(0), _ref(NULL), _delay_30s_dec(true)
	{
		set_lsner(lsner, data, ref, inc_ref);
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline delayref_callback_variable<LSNER_TYPE, DATA_TYPE>::delayref_callback_variable(const delayref_callback_variable& cbv)
		:_lsner(NULL), _data(0), _ref(NULL), _delay_30s_dec(true)
	{
		set_lsner(cbv._lsner, cbv._data, cbv._ref);
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline delayref_callback_variable<LSNER_TYPE, DATA_TYPE>::~delayref_callback_variable()
	{
		BT_SAFE_RELEASE(_ref);
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	void delayref_callback_variable<LSNER_TYPE, DATA_TYPE>::set_lsner(LSNER_TYPE* lsner, DATA_TYPE data, refer* ref, bool inc_ref)
	{
		refer* old_ref = _ref;

		//
		_lsner = lsner;
		_ref = ref;
		_data = data;

		//
		if (inc_ref && _ref)
		{
			_ref->inc_ref();
		}

		//
		if (old_ref)
		{
			//延时30秒销毁
			if (_delay_30s_dec)
			{
				old_ref->inc_ref();
				global_delay_del_obj::instance()->delay_del_obj(old_ref);
			}

			//延时对象
			for (btlist<delay_dec_refer*>::iterator iter = _delay_dec_refer.begin();iter != _delay_dec_refer.end();++iter)
			{
				old_ref->inc_ref();
				(*iter)->delay_del(old_ref);
			}

			//
			old_ref->dec_ref();
		}
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline const DATA_TYPE& delayref_callback_variable<LSNER_TYPE, DATA_TYPE>::get_data() const
	{
		return _data;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline void* delayref_callback_variable<LSNER_TYPE, DATA_TYPE>::get_lsner() const
	{
		return _lsner;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline delayref_callback_variable<LSNER_TYPE, DATA_TYPE>& delayref_callback_variable<LSNER_TYPE, DATA_TYPE>::operator=(const delayref_callback_variable& cb)
	{
		set_lsner(cb._lsner, cb._data, cb._ref);
		return *this;	
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline delayref_callback_variable<LSNER_TYPE, DATA_TYPE>::operator void* () const
	{
		return _lsner;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline void delayref_callback_variable<LSNER_TYPE, DATA_TYPE>::onoff_delay_30s_dec(bool onoff)
	{
		_delay_30s_dec = onoff;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline void delayref_callback_variable<LSNER_TYPE, DATA_TYPE>::add_delay_dec_refer(delay_dec_refer* obj)
	{
		if (find(_delay_dec_refer.begin(), _delay_dec_refer.end(), obj) == _delay_dec_refer.end())
		{
			_delay_dec_refer.push_back(obj);
		}
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline void delayref_callback_variable<LSNER_TYPE, DATA_TYPE>::add_delay_dec_refer(const btlist<delay_dec_refer*>& obj_list)
	{
		for (btlist<delay_dec_refer*>::const_iterator iter = obj_list.begin();iter != obj_list.end();++iter)
		{
			add_delay_dec_refer(*iter);
		}
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline call_delayref_callback<LSNER_TYPE, DATA_TYPE>::call_delayref_callback(const delayref_callback_variable<LSNER_TYPE, DATA_TYPE>& cbv)
		:_lsner(cbv._lsner), _data(cbv._data)
	{

	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline call_delayref_callback<LSNER_TYPE, DATA_TYPE>::call_delayref_callback(const call_delayref_callback& ccb)
		:_lsner(ccb._lsner), _data(ccb._data)
	{

	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline call_delayref_callback<LSNER_TYPE, DATA_TYPE>::call_delayref_callback()
		:_lsner(NULL), _data(0)
	{

	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline const DATA_TYPE& call_delayref_callback<LSNER_TYPE, DATA_TYPE>::get_data() const
	{
		return _data;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline LSNER_TYPE* call_delayref_callback<LSNER_TYPE, DATA_TYPE>::operator->() const
	{
		assert(_lsner);
		return _lsner;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline call_delayref_callback<LSNER_TYPE, DATA_TYPE>::operator LSNER_TYPE* () const
	{
		return _lsner;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline call_delayref_callback<LSNER_TYPE, DATA_TYPE>& call_delayref_callback<LSNER_TYPE, DATA_TYPE>::operator=(const call_delayref_callback& ccb)
	{
		_lsner = ccb._lsner;
		_data = ccb._data;
		return *this;
	}

	template<class LSNER_TYPE, class DATA_TYPE>
	inline call_delayref_callback<LSNER_TYPE, DATA_TYPE>& call_delayref_callback<LSNER_TYPE, DATA_TYPE>::operator=(const delayref_callback_variable<LSNER_TYPE, DATA_TYPE>& cbv)
	{
		_lsner = cbv._lsner;
		_data = cbv._data;
		return *this;
	}
}

