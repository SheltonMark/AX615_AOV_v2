

namespace bt
{


	template <class PARENT_REFER>
	inside_refer<PARENT_REFER>::inside_refer()
	{
		_parent = NULL;
	}

	template <class PARENT_REFER>
	void inside_refer<PARENT_REFER>::set_parent_refer(refer* parent_ref)
	{
		_parent = parent_ref;
	}

	template <class PARENT_REFER>
	void inside_refer<PARENT_REFER>::delete_this(refer* my)
	{
		refer* parent = _parent;
		if (parent)
		{
			parent->delete_this(my);
		}
	}

	template <class REFER, class INSIDE_REFER>
	tow_refer<REFER, INSIDE_REFER>::tow_refer()
	{
		_inside_ref.set_parent_refer(this);
	}

	template <class REFER, class INSIDE_REFER>
	refer* tow_refer<REFER, INSIDE_REFER>::get_inside_refer()
	{
		return &_inside_ref;
	}

	template <class REFER, class INSIDE_REFER>
	void tow_refer<REFER, INSIDE_REFER>::delete_this(refer* my)
	{
		if (my == this)
		{
			//这里可能会多次进来，但on_outside_refer_zero只能调用一次
			if (_call_on_outside_refer_zero.exchange(1) == 0)
			{
				on_outside_refer_zero();
			}

			_inside_ref.dec_ref();
		}
		else
		{
			REFER::delete_this(my);
		}
	}

	template <class REFER, class INSIDE_REFER>
	void tow_refer<REFER, INSIDE_REFER>::on_outside_refer_zero()
	{

	}

	template <class REFER, class INSIDE_REFER>
	int tow_refer<REFER, INSIDE_REFER>::inc_ref()
	{
		int ret = REFER::inc_ref();
		if (ret == 1)
		{
			//外部索引被置0后，又添加了
			_inside_ref.inc_ref();
		}
		return ret;
	}

	template <class REFER, class INSIDE_REFER>
	int tow_refer<REFER, INSIDE_REFER>::dec_ref()
	{
		return REFER::dec_ref();
	}
}

