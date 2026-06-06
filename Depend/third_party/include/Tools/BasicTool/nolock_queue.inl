#include "spin_mutex.h"
#include "bt_statistics.h"
#include "osapi.h"
#include "watch_timer.h"
#include "define.h"

namespace bt
{

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template <class LOCK_TYPE, class TYPE>
	lock_queue<LOCK_TYPE, TYPE>::lock_queue()
	{
		_max_size = BT_MAX_UINT;
	}
	template <class LOCK_TYPE, class TYPE>
	lock_queue<LOCK_TYPE, TYPE>::~lock_queue()
	{

	}

	template <class LOCK_TYPE, class TYPE>
	void lock_queue<LOCK_TYPE, TYPE>::push(TYPE const& data)
	{
		_mtx.lock();
		_data.push_back(data);
		_mtx.unlock();
	}

	template <class LOCK_TYPE, class TYPE>
	uint lock_queue<LOCK_TYPE, TYPE>::pop(TYPE& data)
	{
		_mtx.lock();
		if (_data.empty())
		{
			_mtx.unlock();
			return 0;
		}
		data = _data.front();
		_data.pop_front();
		_mtx.unlock();
		return 1;
	}

	template <class LOCK_TYPE, class TYPE>
	void lock_queue<LOCK_TYPE, TYPE>::push(TYPE const& data, uint count)
	{
		_mtx.lock();
		for (uint i = 0;i < count;i++)
		{
			_data.push_back(BT_GET_REF_OFF(data, i));
		}
		_mtx.unlock();
	}

	template <class LOCK_TYPE, class TYPE>
	uint lock_queue<LOCK_TYPE, TYPE>::pop(TYPE& data, uint count)
	{
		_mtx.lock();
		count = BT_MIN(count, (uint)_data.size());
		for (uint i = 0;i < count;i++)
		{
			BT_GET_REF_OFF(data, i) = _data.front();
			_data.pop_front();
		}
		_mtx.unlock();
		return count;
	}

	template <class LOCK_TYPE, class TYPE>
	void lock_queue<LOCK_TYPE, TYPE>::set_max_size(uint size)
	{
		_max_size = size;
	}

	//template <class LOCK_TYPE, class TYPE>
	//void lock_queue<LOCK_TYPE, TYPE>::clear()
	//{
	//	auto_lock<LOCK_TYPE> l(_mtx);
	//	_data.clear();
	//}

	template <class LOCK_TYPE, class TYPE>
	inline uint lock_queue<LOCK_TYPE, TYPE>::size() const
	{
		return (uint)_data.size();
	}

	template <class LOCK_TYPE, class TYPE>
	inline bool lock_queue<LOCK_TYPE, TYPE>::empty() const
	{
		return _data.size() == 0;
	}

	template <class LOCK_TYPE, class TYPE>
	inline bool lock_queue<LOCK_TYPE, TYPE>::full() const
	{
		return _data.size() >= _max_size;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	two_lock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::two_lock_queue()
	{
		//必须是2的N次方
		_read = _write = BT_MAX_UINT - 100;		//如果越界有问题，尽快暴露

		_max_size = 0;
		_data = NULL;

		set_max_size(0);
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	two_lock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::~two_lock_queue()
	{
		if (_data)
		{
			delete[] _data;
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	void two_lock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::push(TYPE const& data)
	{
		_write_mtx.lock();

		uint write = _write;
		uint left = write - _read;

		//保证都可以放入队列，所以满了就堵塞。
		if (SAFE_SIZE || (_reserve.empty() && ((left + 1 + 10) < _max_size)))
		{
			_data[write & (_max_size - 1)] = data;

			//确保_data已经写入
			write_barrier();

			_write = write + 1;

			_write_mtx.unlock();
		}
		else
		{
			_write_mtx.unlock();

#if defined(BT_STATISTICS)
			g_statis_two_queue_failed++;
#endif

			_reserve.push(data);
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	uint two_lock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::pop(TYPE& data)
	{
		_read_mtx.lock();

		uint read = _read;
		uint left = _write - read;	//必须先计算下
		if (left > 0)
		{
			//flush_all_cpu_cache从PUSH移到POP，因为POP的调用概率比PUSH低。在批量POP的情况下。
			//而且POP一般都是一个线程调用，而PUSH可能多个线程调用，冲突概率减小。
			//flush_all_cpu_cache();
			read_barrier();

			data = _data[read & (_max_size - 1)];
			_read = read + 1;

			_read_mtx.unlock();

			return 1;
		}
		else
		{
			_read_mtx.unlock();

			if ((SAFE_SIZE == 0) && (_reserve.empty() == false) && (_vec_inside_size() == 0))
			{
				return _reserve.pop(data);
			}

			return 0;
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	void two_lock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::push(TYPE const& data, uint count)
	{
		_write_mtx.lock();

		uint write = _write;
		uint left = write - _read;

		//保证都可以放入队列，所以满了就堵塞。
		if (SAFE_SIZE || (_reserve.empty() && ((left + count + 10) < _max_size)))
		{
			for (uint i = 0;i < count;i++)
			{
				_data[(write + i) & (_max_size - 1)] = BT_GET_REF_OFF(data, i);
			}

			//确保_data已经写入
			write_barrier();

			_write = write + count;

			_write_mtx.unlock();
		}
		else
		{
			_write_mtx.unlock();

#if defined(BT_STATISTICS)
			g_statis_two_queue_failed++;
#endif

			_reserve.push(data, count);
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	uint two_lock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::pop(TYPE& data, uint count)
	{
		_read_mtx.lock();

		uint read = _read;
		uint left = _write - read;	//必须先计算下
		uint copy_count = BT_MIN(count, left);

		//flush_all_cpu_cache从PUSH移到POP，因为POP的调用概率比PUSH低。在批量POP的情况下。
		//而且POP一般都是一个线程调用，而PUSH可能多个线程调用，冲突概率减小。
		//flush_all_cpu_cache();
		read_barrier();

		for (uint i = 0;i < copy_count;i++)
		{
			BT_GET_REF_OFF(data, i) = _data[(read + i) & (_max_size - 1)];
		}
		_read = read + copy_count;

		_read_mtx.unlock();

		if ((SAFE_SIZE == 0) && (copy_count < count) && (_reserve.empty() == false) && (_vec_inside_size() == 0))
		{
			copy_count += _reserve.pop(BT_GET_REF_OFF(data, copy_count), count - copy_count);
		}

		return copy_count;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline uint two_lock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::_vec_inside_size() const
	{
		uint read_index = _read;
		uint write_index = _write;
		return write_index - read_index;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline uint two_lock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::size() const
	{
		if (SAFE_SIZE)
		{
			return _vec_inside_size();
		}
		else
		{
			return _vec_inside_size() + _reserve.size();
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline uint two_lock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::max_size() const
	{
		return _max_size;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline bool two_lock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::empty() const
	{
		return size() == 0;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline bool two_lock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::full() const
	{
		return size() >= _max_size;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	void two_lock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::set_max_size(uint size)
	{
		_max_size = 1 << log2(BT_MAX(size, 32));
		if (_max_size < size)
		{
			_max_size = _max_size << 1;
		}
		if (_data)
		{
			delete[] _data;
		}
		_data = new(std::nothrow) TYPE[_max_size];
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	two_lock_queue_value<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::two_lock_queue_value()
	{
		//必须是2的N次方
		_read = _write = BT_MAX_UINT - 100;		//如果越界有问题，尽快暴露

		_max_size = 0;
		_data = NULL;

		set_max_size(0);
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	two_lock_queue_value<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::~two_lock_queue_value()
	{
		if (_data)
		{
			delete[] _data;
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	void two_lock_queue_value<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::push(TYPE const& data)
	{
		_write_mtx.lock();

		uint write = _write;
		uint left = write - _read;

		//保证都可以放入队列，所以满了就堵塞。
		if (SAFE_SIZE || (_reserve.empty() && ((left + 1 + 10) < _max_size)))
		{
			_data[write & (_max_size - 1)] = data;

			write_barrier();

			_write = write + 1;

			_write_mtx.unlock();
		}
		else
		{
			_write_mtx.unlock();

#if defined(BT_STATISTICS)
			g_statis_two_queue_failed++;
#endif

			_reserve.push(data);
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	uint two_lock_queue_value<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::pop(TYPE& data)
	{
		_read_mtx.lock();

		uint read = _read;
		uint left = _write - read;	//必须先计算下

		if (left > 0)
		{
			volatile TYPE& value = _data[read & (_max_size - 1)];
			if (value == 0)
			{
				//数值没刷新过来，刷新下
				//flush_all_cpu_cache();
				read_barrier();
			}
			data = value;
			value = 0;

			_read = read + 1;

			_read_mtx.unlock();

			return 1;
		}
		else
		{
			_read_mtx.unlock();

			if ((SAFE_SIZE == 0) && (_reserve.empty() == false) && (_vec_inside_size() == 0))
			{
				return _reserve.pop(data);
			}

			return 0;
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	void two_lock_queue_value<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::push(TYPE const& data, uint count)
	{
		_write_mtx.lock();

		uint write = _write;
		uint left = write - _read;

		//保证都可以放入队列，所以满了就堵塞。
		if (SAFE_SIZE || (_reserve.empty() && ((left + count + 10) < _max_size)))
		{
			for (uint i = 0;i < count;i++)
			{
				_data[(write + i) & (_max_size - 1)] = BT_GET_REF_OFF(data, i);
			}

			write_barrier();

			_write = write + count;

			_write_mtx.unlock();
		}
		else
		{
			_write_mtx.unlock();

#if defined(BT_STATISTICS)
			g_statis_two_queue_failed++;
#endif

			_reserve.push(data, count);
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	uint two_lock_queue_value<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::pop(TYPE& data, uint count)
	{
		_read_mtx.lock();

		uint read = _read;
		uint left = _write - read;	//必须先计算下
		uint copy_count = BT_MIN(count, left);

		for (uint i = 0;i < copy_count;i++)
		{
			volatile TYPE& value = _data[(read + i) & (_max_size - 1)];
			if (value == 0)
			{
				//数值没刷新过来，刷新下
				//flush_all_cpu_cache();
				read_barrier();
			}
			BT_GET_REF_OFF(data, i) = value;
			value = 0;
		}
		_read = read + copy_count;

		_read_mtx.unlock();

		if ((SAFE_SIZE == 0) && (copy_count < count) && (_reserve.empty() == false) && (_vec_inside_size() == 0))
		{
			copy_count += _reserve.pop(BT_GET_REF_OFF(data, copy_count), count - copy_count);
		}

		return copy_count;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline uint two_lock_queue_value<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::_vec_inside_size() const
	{
		uint read_index = _read;
		uint write_index = _write;
		return write_index - read_index;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline uint two_lock_queue_value<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::size() const
	{
		if (SAFE_SIZE)
		{
			return _vec_inside_size();
		}
		else
		{
			return _vec_inside_size() + _reserve.size();
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline uint two_lock_queue_value<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::max_size() const
	{
		return _max_size;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline bool two_lock_queue_value<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::empty() const
	{
		return size() == 0;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline bool two_lock_queue_value<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::full() const
	{
		return size() >= _max_size;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	void two_lock_queue_value<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::set_max_size(uint size)
	{
		_max_size = 1 << log2(BT_MAX(size, 32));
		if (_max_size < size)
		{
			_max_size = _max_size << 1;
		}
		if (_data)
		{
			delete[] _data;
		}
		_data = new(std::nothrow) TYPE[_max_size];
		for (uint i = 0;i < _max_size;i++)
		{
			_data[i] = 0;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	two_lock_queue_ptr<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::two_lock_queue_ptr()
	{
		//必须是2的N次方
		_read = _write = BT_MAX_UINT - 100;		//如果越界有问题，尽快暴露

		_max_size = 0;
		_data = NULL;

		set_max_size(0);
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	two_lock_queue_ptr<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::~two_lock_queue_ptr()
	{
		if (_data)
		{
			delete[] _data;
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	void two_lock_queue_ptr<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::push(TYPE const& data)
	{
		if (data == NULL)
		{
			return;
		}

		_write_mtx.lock();

		uint write = _write;
		uint left = write - _read;

		//保证都可以放入队列，所以满了就堵塞。
		if (SAFE_SIZE || (_reserve.empty() && ((left + 1 + 10) < _max_size)))
		{
			_data[write & (_max_size - 1)] = data;

			_write = write + 1;

			_write_mtx.unlock();
		}
		else
		{
			_write_mtx.unlock();

#if defined(BT_STATISTICS)
			g_statis_two_queue_failed++;
#endif

			_reserve.push(data);
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	uint two_lock_queue_ptr<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::pop(TYPE& data)
	{
		_read_mtx.lock();

		uint read = _read;
		uint left = _write - read;	//必须先计算下

		if (left > 0)
		{
			volatile TYPE& value = _data[read & (_max_size - 1)];
			while (value == 0)
			{
				//数值没刷新过来，刷新下
				//flush_all_cpu_cache();
				read_barrier();
			}
			data = value;
			value = 0;

			_read = read + 1;

			_read_mtx.unlock();

			return 1;
		}
		else
		{
			_read_mtx.unlock();

			if ((SAFE_SIZE == 0) && (_reserve.empty() == false) && (_vec_inside_size() == 0))
			{
				return _reserve.pop(data);
			}

			return 0;
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	void two_lock_queue_ptr<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::push(TYPE const& data, uint count)
	{
		_write_mtx.lock();

		uint write = _write;
		uint left = write - _read;

		//保证都可以放入队列，所以满了就堵塞。
		if (SAFE_SIZE || (_reserve.empty() && ((left + count + 10) < _max_size)))
		{
			for (uint i = 0;i < count;i++)
			{
				_data[(write + i) & (_max_size - 1)] = BT_GET_REF_OFF(data, i);
			}

			_write = write + count;

			_write_mtx.unlock();
		}
		else
		{
			_write_mtx.unlock();

#if defined(BT_STATISTICS)
			g_statis_two_queue_failed++;
#endif

			_reserve.push(data, count);
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	uint two_lock_queue_ptr<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::pop(TYPE& data, uint count)
	{
		_read_mtx.lock();

		uint read = _read;
		uint left = _write - read;	//必须先计算下
		uint copy_count = BT_MIN(count, left);

		for (uint i = 0;i < copy_count;i++)
		{
			volatile TYPE& value = _data[(read + i) & (_max_size - 1)];
			while (value == 0)
			{
				//数值没刷新过来，刷新下
				//flush_all_cpu_cache();
				read_barrier();
			}
			BT_GET_REF_OFF(data, i) = value;
			value = 0;
		}
		_read = read + copy_count;

		_read_mtx.unlock();

		if ((SAFE_SIZE == 0) && (copy_count < count) && (_reserve.empty() == false) && (_vec_inside_size() == 0))
		{
			copy_count += _reserve.pop(BT_GET_REF_OFF(data, copy_count), count - copy_count);
		}

		return copy_count;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline uint two_lock_queue_ptr<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::_vec_inside_size() const
	{
		uint read_index = _read;
		uint write_index = _write;
		return write_index - read_index;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline uint two_lock_queue_ptr<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::size() const
	{
		if (SAFE_SIZE)
		{
			return _vec_inside_size();
		}
		else
		{
			return _vec_inside_size() + _reserve.size();
		}
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline uint two_lock_queue_ptr<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::max_size() const
	{
		return _max_size;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline bool two_lock_queue_ptr<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::empty() const
	{
		return size() == 0;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	inline bool two_lock_queue_ptr<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::full() const
	{
		return size() >= _max_size;
	}

	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int SAFE_SIZE>
	void two_lock_queue_ptr<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, SAFE_SIZE>::set_max_size(uint size)
	{
		_max_size = 1 << log2(BT_MAX(size, 32));
		if (_max_size < size)
		{
			_max_size = _max_size << 1;
		}
		if (_data)
		{
			delete[] _data;
		}
		_data = new(std::nothrow) TYPE[_max_size];
		for (uint i = 0;i < _max_size;i++)
		{
			_data[i] = 0;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <class TYPE>
	hydra_queue<TYPE>::hydra_queue()
	{
		_tlp.set_destructor_cb(this);

		_max_size = 512;

		//可以马上稳定
		_write_index.set_count(-1000);
		_read_index = (nint)_write_index.get_count();
	}
	template <class TYPE>
	hydra_queue<TYPE>::~hydra_queue()
	{
		_tlp.set_destructor_cb(NULL);

		auto_lock_mutex l(_mtx);
		for (typename std::vector<thread_queue*>::iterator iter = _all_queue.begin();iter != _all_queue.end();++iter)
		{
			delete *iter;
		}
		for (typename std::list<thread_queue*>::iterator iter = _add.begin();iter != _add.end();++iter)
		{
			delete *iter;
		}
	}

	template <class TYPE>
	void hydra_queue<TYPE>::_dealwith_add_queue()
	{
		auto_lock_mutex l(_mtx);
		for (typename std::list<thread_queue*>::iterator iter = _add.begin();iter != _add.end();++iter)
		{
			_all_queue.push_back(*iter);
		}
		_add.clear();
	}

	template <class TYPE>
	void hydra_queue<TYPE>::_dealwith_delete_queue(int index)
	{
		thread_queue* queue = _all_queue[index];
		_all_queue.erase(_all_queue.begin() + index);
		delete queue;
	}

	template <class TYPE>
	uint hydra_queue<TYPE>::_pop(TYPE& data)
	{
		thread_queue* pop_queue = NULL;
		for (int i = 0;(i < (int)_all_queue.size());i++)
		{
			thread_queue* queue = _all_queue[i];

			if (queue->buf_read < queue->buf_write)
			{
				if (queue->item_buf[queue->buf_read].index == (_read_index + 1))
				{
					pop_queue = queue;
					break;
				}
			}
			else if ((queue->buf_read == queue->buf_write) && (queue->queue.size() > 0))
			{
				queue->buf_read = 0;
				queue->buf_write = queue->queue.pop(queue->item_buf[0], thread_queue::MAX_ITEM_BUF);

				if ((queue->buf_write > 0) && (queue->item_buf[0].index == (_read_index + 1)))
				{
					pop_queue = queue;
					break;
				}
			}
			else if (queue->will_delete)
			{
				_dealwith_delete_queue(i);
				i--;
			}
		}

		if (pop_queue == NULL)
		{
			return 0;
		}

		data = pop_queue->item_buf[pop_queue->buf_read].t;
		_read_index++;
		pop_queue->buf_read++;

		return 1;
	}

	//template <class TYPE>
	//uint hydra_queue<TYPE>::_pop(TYPE& data, uint count)
	//{
	//	thread_queue* pop_queue = NULL;
	//	for (int i = 0;i < (int)_all_queue.size();i++)
	//	{
	//		thread_queue* queue = _all_queue[i];

	//		if (queue->buf_read < queue->buf_write)
	//		{
	//			if (queue->item_buf[queue->buf_read].index == (_read_index + 1))
	//			{
	//				pop_queue = queue;
	//				break;
	//			}
	//		}
	//		else if ((queue->buf_read == queue->buf_write) && (queue->queue.size() > 0))
	//		{
	//			queue->buf_read = 0;
	//			queue->buf_write = queue->queue.pop(queue->item_buf[0], thread_queue::MAX_ITEM_BUF);

	//			if ((queue->buf_write > 0) && (queue->item_buf[0].index == (_read_index + 1)))
	//			{
	//				pop_queue = queue;
	//				break;
	//			}
	//		}
	//		else if (queue->will_delete)
	//		{
	//			_dealwith_delete_queue(i);
	//			i--;
	//		}
	//	}

	//	if (pop_queue == NULL)
	//	{
	//		return 0;
	//	}

	//	//uint read_count = 0;
	//	//do 
	//	//{
	//	//	BT_GET_REF_OFF(data, read_count) = pop_queue->item_buf[pop_queue->buf_read].t;
	//	//	_read_index++;
	//	//	pop_queue->buf_read++;
	//	//	read_count++;

	//	//	if ((pop_queue->buf_read == pop_queue->buf_write)
	//	//		|| (pop_queue->item_buf[pop_queue->buf_read].index != (_read_index + 1)))
	//	//	{
	//	//		break;
	//	//	}
	//	//} while (read_count < count);

	//	//return read_count;

	//	data = pop_queue->item_buf[pop_queue->buf_read].t;
	//	_read_index++;
	//	pop_queue->buf_read++;

	//	return 1;
	//}

	template <class TYPE>
	void hydra_queue<TYPE>::push(TYPE const& data)
	{
		thread_queue* queue = _get_thread_queue();

		queue_item item;
		item.t = data;
		item.index = _write_index.inc_count();
		queue->queue.push(item);
	}

	template <class TYPE>
	uint hydra_queue<TYPE>::pop(TYPE& data)
	{
		if (_add.empty() == false)
		{
			//把不常调用的代码封装成一个独立的接口，有助于优化性能。
			_dealwith_add_queue();
		}

		return _pop(data);
	}

	template <class TYPE>
	void hydra_queue<TYPE>::push(TYPE const& data, uint count)
	{
		nint write_index = (nint)_write_index.exchange_add(count);

		thread_queue* queue = _get_thread_queue();

		for (uint i = 0;i < count;i++)
		{
			queue_item item;
			item.t = BT_GET_REF_OFF(data, i);
			item.index = ++write_index;
			queue->queue.push(item, count);
		}
	}
	template <class TYPE>
	uint hydra_queue<TYPE>::pop(TYPE& data, uint count)
	{
		if (_add.empty() == false)
		{
			_dealwith_add_queue();
		}

		uint read_count = 0;
		while (read_count < count)
		{
			TYPE& now = BT_GET_REF_OFF(data, read_count);
			if (_pop(now) == 0)
			{
				break;
			}
			read_count ++;
		}

		return read_count;
	}
	template <class TYPE>
	void hydra_queue<TYPE>::set_max_size(uint size)
	{
		_max_size = size;
	}

	template <class TYPE>
	inline uint hydra_queue<TYPE>::size() const
	{
		return (nint)_write_index.get_count() - _read_index;
	}
	template <class TYPE>
	inline uint hydra_queue<TYPE>::max_size() const
	{
		return _max_size;
	}
	template <class TYPE>
	inline bool hydra_queue<TYPE>::empty() const
	{
		return size() == 0;
	}
	template <class TYPE>
	inline bool hydra_queue<TYPE>::full() const
	{
		return size() > _max_size;
	}

	template <class TYPE>
	void hydra_queue<TYPE>::on_tlp_free_resource(int threadid, void* value, ptr_int user_data)
	{
		thread_queue* queue = (thread_queue*)value;
		queue->will_delete = true;
	}

	template <class TYPE>
	typename hydra_queue<TYPE>::thread_queue* hydra_queue<TYPE>::_new_thread_queue()
	{
		thread_queue* queue = new thread_queue;
		queue->queue.set_max_size(_max_size);
		queue->will_delete = false;
		queue->buf_read = queue->buf_write = 0;

		_tlp.set_value(queue);

		_mtx.lock();
		_add.push_back(queue);
		_mtx.unlock();
		return queue;
	}

	template <class TYPE>
	inline typename hydra_queue<TYPE>::thread_queue* hydra_queue<TYPE>::_get_thread_queue()
	{
		thread_queue* queue = (thread_queue*)_tlp.get_value();
		if (queue == NULL)
		{
			queue = _new_thread_queue();
		}
		return queue;
	}

//
//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE>
//	nolock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE>::nolock_queue()
//	{
//		//必须是2的N次方
//		_read = _write = 0;
//		set_max_size(0);
//	}
//	
//	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE>
//	nolock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE>::~nolock_queue()
//	{
//
//	}
//
//	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE>
//	void nolock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE>::push(TYPE const& data, uint count)
//	{
//		auto_lock<PUSH_LOCK_TYPE> l(_write_mtx);
//
//		//保证都可以放入队列，所以满了就堵塞。
//		if ((_reserve.empty() == false) || ((_vec_inside_size() + count) > _data.size()))
//		{
//			l.unlock();
//
//#if defined(BT_STATISTICS)
//			g_nolock_queue_limit.inc_count();
//#endif
//			_reserve.push(data, count);
//		}
//		else
//		{
//			for (uint i = 0;i < count;i++)
//			{
//				_data[_write & (_data.size() - 1)] = BT_GET_REF_OFF(data, i);
//				_write++;
//			}
//		}
//	}
//
//	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE>
//	uint nolock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE>::pop(TYPE& data, uint count)
//	{
//		auto_lock<POP_LOCK_TYPE> l(_read_mtx);
//
//		uint copy_count = 0;
//		for (;(copy_count < count) && (_vec_inside_size() > 0);copy_count++)
//		{
//			BT_GET_REF_OFF(data, copy_count) = _data[_read & (_data.size() - 1)];
//			_read++;
//		}
//
//		l.unlock();
//
//		if ((_vec_inside_size() == 0) && (_reserve.empty() == false) && (copy_count < count))
//		{
//			copy_count += _reserve.pop(BT_GET_REF_OFF(data, copy_count), count - copy_count);
//		}
//
//		return copy_count;
//	}
//
//	//template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE, int MAX_ITEM_SIZE>
//	//void nolock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE, MAX_ITEM_SIZE>::clear()
//	//{
//	//	//只能变少，不能变多
//	//	_read = _write;
//	//}
//
//	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE>
//	inline uint nolock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE>::_vec_inside_size() const
//	{
//		uint read_index = _read;
//		uint write_index = _write;
//		return write_index - read_index;
//	}
//	
//	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE>
//	inline uint nolock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE>::size() const
//	{
//		return _vec_inside_size() + _reserve.size();
//	}
//
//	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE>
//	inline bool nolock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE>::empty() const
//	{
//		return size() == 0;
//	}
//
//	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE>
//	inline bool nolock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE>::full() const
//	{
//		return size() >= _data.size();
//	}
//
//	template <class PUSH_LOCK_TYPE, class POP_LOCK_TYPE, class TYPE>
//	void nolock_queue<PUSH_LOCK_TYPE, POP_LOCK_TYPE, TYPE>::set_max_size(uint size)
//	{
//		size = 1 << log2(BT_MAX(size, 256));
//		_data.resize(size);
//	}
//
//
//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	template <class PUSH_ATOMIC_TYPE, class POP_ATOMIC_TYPE, class TYPE>
//	atomic_queue<PUSH_ATOMIC_TYPE, POP_ATOMIC_TYPE, TYPE>::atomic_queue()
//	{
//		set_max_size(0);
//	}
//
//	template <class PUSH_ATOMIC_TYPE, class POP_ATOMIC_TYPE, class TYPE>
//	atomic_queue<PUSH_ATOMIC_TYPE, POP_ATOMIC_TYPE, TYPE>::~atomic_queue()
//	{
//
//	}
//
//	template <class PUSH_ATOMIC_TYPE, class POP_ATOMIC_TYPE, class TYPE>
//	void atomic_queue<PUSH_ATOMIC_TYPE, POP_ATOMIC_TYPE, TYPE>::push(TYPE const& data, uint count)
//	{
//		//保证都可以放入队列，所以满了就堵塞。
//		if ((_reserve.empty() == false) || ((_vec_inside_size() + count * 50) > (_data.size() - 50)))
//		{
//			//printf("_reserve.push(%d)\n", count);
//#if defined(BT_STATISTICS)
//			g_nolock_queue_limit.inc_count();
//#endif
//			_reserve.push(data, count);
//		}
//		else
//		{
//			nint32 write_value = (uint32_t)_write.exchange_add(count);
//			for (uint i = 0;i < count;i++, write_value++)
//			{
//				nint32 write_index = write_value & _size_mask();
//				nint32 seq = write_value & ~_size_mask();
//				volatile TYPE_EX* p = &_data[write_index];
//				//考虑到其他线程写的时候切换了
//				if (p->seq != seq)
//				{
//					flush_all_cpu_cache();
//
//					while (p->seq != seq)
//					{
//#if defined(BT_STATISTICS)
//						g_atcque_sleep.inc_count();
//#endif
//						sleep(0);
//						flush_all_cpu_cache();
//					}
//				}
//				//while ((*p == 0) == false);
//				(TYPE&)p->v = BT_GET_REF_OFF(data, i);
//				p->seq++;
//			}
//		}
//	}
//
//	template <class PUSH_ATOMIC_TYPE, class POP_ATOMIC_TYPE, class TYPE>
//	uint atomic_queue<PUSH_ATOMIC_TYPE, POP_ATOMIC_TYPE, TYPE>::pop(TYPE& data, uint count)
//	{
//		nint32 read_value;
//		nint32 read_count;
//		while (true)
//		{
//			//必须先读_read，后读_write。
//			//如果先读_write，后读_read。在这中间，如果其他线程有读写操作，那_read读出来的值会比_write大
//			read_value = (nint32)_read.get_count();
//			nint32 write_value = (nint32)_write.get_count();
//			read_count = BT_MIN(count, write_value - read_value);
//			if (read_count == 0)
//			{
//				break;
//			}
//			if (read_value == (uint32_t)_read.comp_exchange(read_value, read_value + read_count))
//			{
//				break;
//			}
//		}
//
//		for (uint i = 0;i < read_count;i++, read_value++)
//		{
//			nint32 read_index = read_value & _size_mask();
//			nint32 seq = read_value & ~_size_mask();
//			volatile TYPE_EX* p = &_data[read_index];
//			//考虑到其他线程写的时候切换了
//			if (p->seq != (seq + 1))
//			{
//				flush_all_cpu_cache();
//				while (p->seq != (seq + 1))
//				{
//#if defined(BT_STATISTICS)
//					g_atcque_sleep.inc_count();
//#endif
//					sleep(0);
//					flush_all_cpu_cache();
//				}
//			}
//			//while (*p == 0);
//			BT_GET_REF_OFF(data, i) = (TYPE&)p->v;
//			p->seq += _max_size() - 1;
//		}
//
//		if ((_vec_inside_size() == 0) && (_reserve.empty() == false) && (read_count < count))
//		{
//			//printf("_reserve.pop(%d)\n", count - read_count);
//			read_count += _reserve.pop(BT_GET_REF_OFF(data, read_count), count - read_count);
//		}
//
//		return read_count;
//	}
//
//	template <class PUSH_ATOMIC_TYPE, class POP_ATOMIC_TYPE, class TYPE>
//	inline uint atomic_queue<PUSH_ATOMIC_TYPE, POP_ATOMIC_TYPE, TYPE>::_vec_inside_size() const
//	{
//		//解释同上
//		nint32 read_index = (nint32)_read.get_count();
//		nint32 write_index = (nint32)_write.get_count();
//		return write_index - read_index;
//	}
//
//	template <class PUSH_ATOMIC_TYPE, class POP_ATOMIC_TYPE, class TYPE>
//	inline nint32 atomic_queue<PUSH_ATOMIC_TYPE, POP_ATOMIC_TYPE, TYPE>::_max_size() const
//	{
//		return (nint32)_data.size();
//	}
//
//	template <class PUSH_ATOMIC_TYPE, class POP_ATOMIC_TYPE, class TYPE>
//	inline nint32 atomic_queue<PUSH_ATOMIC_TYPE, POP_ATOMIC_TYPE, TYPE>::_size_mask() const
//	{
//		return (nint32)_data.size() - 1;
//	}
//
//	template <class PUSH_ATOMIC_TYPE, class POP_ATOMIC_TYPE, class TYPE>
//	inline uint atomic_queue<PUSH_ATOMIC_TYPE, POP_ATOMIC_TYPE, TYPE>::size() const
//	{
//		return _vec_inside_size() + _reserve.size();
//	}
//
//	template <class PUSH_ATOMIC_TYPE, class POP_ATOMIC_TYPE, class TYPE>
//	inline bool atomic_queue<PUSH_ATOMIC_TYPE, POP_ATOMIC_TYPE, TYPE>::empty() const
//	{
//		return size() == 0;
//	}
//
//	template <class PUSH_ATOMIC_TYPE, class POP_ATOMIC_TYPE, class TYPE>
//	inline bool atomic_queue<PUSH_ATOMIC_TYPE, POP_ATOMIC_TYPE, TYPE>::full() const
//	{
//		return size() > (_data.size() - 50);
//	}
//
//	template <class PUSH_ATOMIC_TYPE, class POP_ATOMIC_TYPE, class TYPE>
//	void atomic_queue<PUSH_ATOMIC_TYPE, POP_ATOMIC_TYPE, TYPE>::set_max_size(uint size)
//	{
//		size = 1 << log2(BT_MAX(size, 256));
//		_data.resize(size);
//		_read.count() = _write.count() = 0;
//
//		for (uint i = 0;i < _data.size();i++)
//		{
//			_data[i].seq = 0;
//		}
//	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//#if defined(BT_SUPPORT_ATOMIC)
	//
	//
	//
	//	template <int MAX_ITEM_SIZE, class TYPE>
	//	nolock_queue<MAX_ITEM_SIZE, TYPE>::nolock_queue()
	//	{
	//		for (int i = 0;i < MAX_ITEM_SIZE;i++)
	//		{
	//			_data[i] = 0;
	//		}
	//
	//#ifdef BT_DEBUG
	//		bool is_2_power = false;
	//		for (int i = 5;i < 32;i++)
	//		{
	//			if ((1 << i) == MAX_ITEM_SIZE)
	//			{
	//				is_2_power = true;
	//				break;
	//			}
	//		}
	//		assert(is_2_power);
	//#endif
	//	}
	//
	//	template <int MAX_ITEM_SIZE, class TYPE>
	//	nolock_queue<MAX_ITEM_SIZE, TYPE>::~nolock_queue()
	//	{
	//
	//	}
	//
	//	template <int MAX_ITEM_SIZE, class TYPE>
	//	void nolock_queue<MAX_ITEM_SIZE, TYPE>::push(TYPE const& data, uint count)
	//	{
	//		if (count == 1)
	//		{
	//			if (data == 0)
	//			{
	//				return;
	//			}
	//
	//			//留出一部分空余
	//			BT_SPIN_OP((size() <= (MAX_ITEM_SIZE - 50)), 10000);
	//			uint32_t write_index = ((uint32_t)_write.inc_count() - 1) % MAX_ITEM_SIZE;
	//			volatile TYPE* p = &_data[write_index];
	//			//考虑到其他线程写的时候切换了
	//			BT_SPIN_OP((*p == 0), 10000);
	//			//while ((*p == 0) == false);
	//			_data[write_index] = data;
	//			flush_all_cpu_cache();
	//		}
	//		else
	//		{
	//			for (uint i = 0;i < count;i++)
	//			{
	//				if (BT_GET_REF_OFF(data, i) == 0)
	//				{
	//					for (uint j = 0;j < count;j++)
	//					{
	//						const TYPE& tmp = BT_GET_REF_OFF(data, j);
	//						if ((tmp == 0) == false)
	//						{
	//							push(tmp);
	//						}
	//					}
	//					return;
	//				}
	//			}
	//
	//			BT_SPIN_OP(((size() + count * 10) <= (MAX_ITEM_SIZE - 50)), 10000);
	//			uint32_t write_index = (uint32_t)_write.exchange_add(count);
	//			for (uint i = 0;i < count;i++, write_index++)
	//			{
	//				write_index %= MAX_ITEM_SIZE;
	//				volatile TYPE* p = &_data[write_index];
	//				//考虑到其他线程写的时候切换了
	//				BT_SPIN_OP((*p == 0), 10000);
	//				//while ((*p == 0) == false);
	//				_data[write_index] = BT_GET_REF_OFF(data, i);
	//			}
	//			flush_all_cpu_cache();
	//		}
	//	}
	//
	//	template <int MAX_ITEM_SIZE, class TYPE>
	//	bool nolock_queue<MAX_ITEM_SIZE, TYPE>::pop(TYPE& data, uint count)
	//	{
	//		if (count == 1)
	//		{
	//			uint32_t read_index;
	//			while (true)
	//			{
	//				read_index = (uint32_t)_read.get_count();
	//				if (((uint32_t)_write.get_count() - read_index) == 0)
	//				{
	//					return false;
	//				}
	//				if (read_index == (uint32_t)_read.comp_exchange(read_index, read_index + 1))
	//				{
	//					break;
	//				}
	//			}
	//			read_index %= MAX_ITEM_SIZE;
	//			volatile TYPE* p = &_data[read_index];
	//			//考虑到其他线程写的时候切换了
	//			BT_SPIN_OP(((*p == 0) == false), 10000);
	//			//while (*p == 0);
	//			data = _data[read_index];			//这里编译的时候，应该会重新取内存
	//			_data[read_index] = 0;
	//			flush_all_cpu_cache();
	//			return true;
	//		}
	//		else
	//		{
	//			uint32_t read_index;
	//			uint read_count;
	//			while (true)
	//			{
	//				read_index = (uint32_t)_read.get_count();
	//				uint32_t write_index = (uint32_t)_write.get_count();
	//				read_count = BT_MIN(count, write_index - read_index);
	//				if (read_count == 0)
	//				{
	//					return false;
	//				}
	//				if (read_index == _read.comp_exchange(read_index, read_index + read_count))
	//				{
	//					break;
	//				}
	//			}
	//			for (uint i = 0;i < read_count;i++, read_index++)
	//			{
	//				read_index %= MAX_ITEM_SIZE;
	//				volatile TYPE* p = &_data[read_index];
	//				//考虑到其他线程写的时候切换了
	//				BT_SPIN_OP(((*p == 0) == false), 10000);
	//				//while (*p == 0);
	//				BT_GET_REF_OFF(data, i) = _data[read_index];
	//				_data[read_index] = 0;
	//			}
	//			flush_all_cpu_cache();
	//			return true;
	//		}
	//	}
	//
	//	template <int MAX_ITEM_SIZE, class TYPE>
	//	inline uint nolock_queue<MAX_ITEM_SIZE, TYPE>::size() const
	//	{
	//		return (uint32_t)_write.get_count() - (uint32_t)_read.get_count();
	//	}
	//
	//	template <int MAX_ITEM_SIZE, class TYPE>
	//	inline bool nolock_queue<MAX_ITEM_SIZE, TYPE>::empty() const
	//	{
	//		return size() == 0;
	//	}
	//
	//	template <int MAX_ITEM_SIZE, class TYPE>
	//	inline bool nolock_queue<MAX_ITEM_SIZE, TYPE>::full() const
	//	{
	//		return size() > (MAX_ITEM_SIZE - 100);
	//	}
	//
	//#endif //BT_SUPPORT_ATOMIC
}

