
namespace bt
{
	template <class MSG>
	async_msg_thread<MSG>::async_msg_thread()
	{
		//默认最多4000个消息
		pipeline<amt_async_msg<MSG> >::set_max_object(4*1024);
	}

	template <class MSG>
	void async_msg_thread<MSG>::on_dealwith_object(amt_async_msg<MSG> const& amsg, ptr_int param)
	{
		amsg.lsner->on_async_msg(amsg.msg);
		if (amsg.ref_lsner)
		{
			amsg.ref_lsner->dec_ref();
		}
	}

	template <class MSG>
	void async_msg_thread<MSG>::async_msg(const MSG& msg, async_msg_listener<MSG>* lsner, refer* ref_lsner)
	{
		assert(lsner);
		if (ref_lsner)
		{
			ref_lsner->inc_ref();
		}

		amt_async_msg<MSG> amsg;
		amsg.msg = msg;
		amsg.lsner = lsner;
		amsg.ref_lsner = ref_lsner;
		post_object(amsg);
	}

	template <class MSG>
	async_msg_postor<MSG>::async_msg_postor()
	{
		_thread = NULL;
		_lsner = NULL;
		_ref_lsner = NULL;
	}

	template <class MSG>
	void async_msg_postor<MSG>::init(async_msg_thread<MSG>* thread, async_msg_listener<MSG>* lsner, refer* ref_lsner)
	{
		_thread = thread;
		_lsner = lsner;
		_ref_lsner = ref_lsner;
	}

	template <class MSG>
	void async_msg_postor<MSG>::post_msg(const MSG& msg)
	{
		_thread->async_msg(msg, _lsner, _ref_lsner);
	}
}

