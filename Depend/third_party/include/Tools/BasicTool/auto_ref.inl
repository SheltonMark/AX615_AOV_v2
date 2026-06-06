
namespace bt
{
	template<class T>
	inline auto_ref<T>::auto_ref()
		:_p(NULL)
	{
	}
	template<class T>
	inline auto_ref<T>::auto_ref(T* p, bool inc_ref)
	{
		_p = p;
		if (inc_ref && _p)
		{
			((refer*)_p)->inc_ref();
		}
		//attach(p, inc_ref);
	}
	template<class T>
	inline auto_ref<T>::auto_ref(const auto_ref& other)
	{
		_p = other._p;
		if (_p)
		{
			((refer*)_p)->inc_ref();
		}
		//attach(other._p);
	}
	template<class T>
	inline auto_ref<T>::~auto_ref()
	{
		if (_p)
		{
			((refer*)_p)->dec_ref();
		}
		//detach();
	}

	template<class T>
	inline auto_ref<T>& auto_ref<T>::operator=(T* p)
	{
		attach(p);
		return (*this);
	}
	template<class T>
	inline auto_ref<T>& auto_ref<T>::operator=(const auto_ref& other)
	{
		attach(other._p);
		return (*this);
	}
	template<class T>
	inline bool auto_ref<T>::operator==(T* p) const
	{
		return _p == p;
	}
	template<class T>
	inline bool auto_ref<T>::operator!=(T* p) const
	{
		return _p != p;
	}
	template<class T>
	inline bool auto_ref<T>::operator==(const auto_ref& other) const
	{
		return _p == other._p;
	}
	template<class T>
	inline bool auto_ref<T>::operator!=(const auto_ref& other) const
	{
		return _p != other._p;
	}
	template<class T>
	inline auto_ref<T>::operator T* ()
	{
		return _p;
	}
	template<class T>
	inline auto_ref<T>::operator const T* () const
	{
		return _p;
	}
	template<class T>
	inline T* auto_ref<T>::operator->()
	{
		return _p;
	}
	template<class T>
	inline const T* auto_ref<T>::operator->() const
	{
		return _p;
	}

	template<class T>
	inline void auto_ref<T>::attach(T* p, bool inc_ref)
	{
		if (_p)
		{
			((refer*)_p)->dec_ref();
		}
		_p = p;
		if (inc_ref && _p)
		{
			((refer*)_p)->inc_ref();
		}
	}
	template<class T>
	inline T* auto_ref<T>::detach(bool dec_ref)
	{
		T* ret = _p;
		if (_p && dec_ref)
		{
			((refer*)_p)->dec_ref();
		}
		_p = NULL;
		return ret;
	}


	template<class T>
	inline auto_ref_ex<T>::auto_ref_ex()
		:_p(NULL), _ref(NULL)
	{
	}
	template<class T>
	inline auto_ref_ex<T>::auto_ref_ex(T* p, refer* ref, bool inc_ref)
	{
		_p = p;
		_ref = ref;
		if (inc_ref && _ref)
		{
			_ref->inc_ref();
		}
		//attach(p, ref, inc_ref);
	}
	template<class T>
	inline auto_ref_ex<T>::auto_ref_ex(const auto_ref_ex& other)
	{
		_p = other._p;
		_ref = other._ref;
		if (_ref)
		{
			_ref->inc_ref();
		}
		//attach(other._p, other._ref);
	}
	template<class T>
	inline auto_ref_ex<T>::~auto_ref_ex()
	{
		if (_ref)
		{
			_ref->dec_ref();
		}
		//detach();
	}

	template<class T>
	inline auto_ref_ex<T>& auto_ref_ex<T>::operator=(const auto_ref_ex& other)
	{
		attach(other._p, other._ref);
		return (*this);
	}
	template<class T>
	inline bool auto_ref_ex<T>::operator==(T* p) const
	{
		return _p == p;
	}
	template<class T>
	inline bool auto_ref_ex<T>::operator!=(T* p) const
	{
		return _p != p;
	}
	template<class T>
	inline bool auto_ref_ex<T>::operator==(const auto_ref_ex& other) const
	{
		return _p == other._p;
	}
	template<class T>
	inline bool auto_ref_ex<T>::operator!=(const auto_ref_ex& other) const
	{
		return _p != other._p;
	}
	template<class T>
	inline auto_ref_ex<T>::operator T* ()
	{
		return _p;
	}
	template<class T>
	inline auto_ref_ex<T>::operator const T* () const
	{
		return _p;
	}
	template<class T>
	inline T* auto_ref_ex<T>::operator->()
	{
		return _p;
	}
	template<class T>
	inline const T* auto_ref_ex<T>::operator->() const
	{
		return _p;
	}

	template<class T>
	inline void auto_ref_ex<T>::attach(T* p, refer* ref, bool inc_ref)
	{
		if (_ref)
		{
			_ref->dec_ref();
		}
		_p = p;
		_ref = ref;
		if (inc_ref && _ref)
		{
			_ref->inc_ref();
		}
	}
	template<class T>
	inline T* auto_ref_ex<T>::detach(refer** ref, bool dec_ref)
	{
		T* ret = _p;
		if (_ref && dec_ref)
		{
			_ref->dec_ref();
		}
		if (ref)
		{
			*ref = _ref;
		}
		_p = NULL;
		_ref = NULL;
		return ret;
	}
}


