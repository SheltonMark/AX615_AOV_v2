
#include "bt_statistics.h"

namespace bt
{
	template<class TYPE>
	seq_lock<TYPE>::seq_lock()
	{
		
	}

	template<class TYPE>
	TYPE seq_lock<TYPE>::get_data()
	{
#if defined(BT_STATISTICS)
		int times = 0;
#endif

		TYPE data[2];
		long seq;
		do 
		{
			seq = _seq_num.get_count();
			read_barrier();

			data[0] = (TYPE&)_data[0];
			data[1] = (TYPE&)_data[1];

#if defined(BT_STATISTICS)
			times++;
#endif

			read_barrier();
		} while (_seq_num.get_count() != seq);

#if defined(BT_STATISTICS)
		if (times > 1)
		{
			g_statis_seq_lock++;
		}
#endif

		if (seq & 0x1)
		{
			return data[0];
		}
		else
		{
			return data[1];
		}
	}

	template<class TYPE>
	void seq_lock<TYPE>::set_data(const TYPE& data)
	{
		(TYPE&)_data[0] = data;
		_seq_num.inc_count();
		(TYPE&)_data[1] = data;
		_seq_num.inc_count();
	}
}
