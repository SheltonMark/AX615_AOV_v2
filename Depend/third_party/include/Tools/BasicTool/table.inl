#include "table.h"

namespace bt
{

	template<class TYPE>
	table<TYPE>::table()
	{
		_count = 0;
		_data_header = INVALIDE_INDEX;
		_free_header = INVALIDE_INDEX;
	}
	template<class TYPE>
	table<TYPE>::~table()
	{
		clear();
	}

	template<class TYPE>
	uint table<TYPE>::new_data(uint index)
	{
		if (index == INVALIDE_INDEX)
		{
			if (_free_header == INVALIDE_INDEX)
			{
				inc_node_buf(0);
			}
			index = _free_header;

			//从空闲列表移除
			_free_header = _data[index].next;
			if (_free_header != INVALIDE_INDEX)
			{
				_data[_free_header].prev = INVALIDE_INDEX;
			}
		}
		else
		{
			if (index >= _data.size())
			{
				inc_node_buf(index+1);
			}
			if (_data[index].is_free == false)
			{
				return INVALIDE_INDEX;
			}

			//从空闲列表移除
			if (_data[index].prev != INVALIDE_INDEX)
			{
				_data[_data[index].prev].next = _data[index].next;
			}
			else
			{
				_free_header = _data[index].next;
			}
			if (_data[index].next != INVALIDE_INDEX)
			{
				_data[_data[index].next].prev = _data[index].prev;
			}
		}

		//加入数据列表
		if (_data_header != INVALIDE_INDEX)
		{
			_data[_data_header].prev = index;
		}
		_data[index].next = _data_header;
		_data[index].prev = INVALIDE_INDEX;
		_data_header = index;

		//
		_data[index].is_free = false;
		_count++;

		//构造
		BT_CALL_CONSTRUCT(_data[index].data, TYPE, );

		return index;
	}

	template<class TYPE>
	void table<TYPE>::free_data(uint index)
	{
		if (index >= _data.size())
		{
			return;
		}
		if (_data[index].is_free)
		{
			return;
		}

		//从数据列表中移除
		if (_data[index].prev != INVALIDE_INDEX)
		{
			_data[_data[index].prev].next = _data[index].next;
		}
		else
		{
			_data_header = _data[index].next;
		}
		if (_data[index].next != INVALIDE_INDEX)
		{
			_data[_data[index].next].prev = _data[index].prev;
		}

		//加入到空闲列表
		if (_free_header != INVALIDE_INDEX)
		{
			_data[_free_header].prev = index;
		}
		_data[index].next = _free_header;
		_data[index].prev = INVALIDE_INDEX;
		_free_header = index;

		//
		_count--;
		_data[index].is_free = true;

		//析构
		BT_CALL_DESTRUCTOR(_data[index].data, TYPE);
	}

	template<class TYPE>
	void table<TYPE>::clear()
	{
		uint data_tailer = _data_header;
		for (uint i = _data_header;i != INVALIDE_INDEX;i = _data[i].next)
		{
			BT_CALL_DESTRUCTOR(_data[i].data, TYPE);
			_data[i].is_free = true;
			data_tailer = i;
		}
		_count = 0;

		//将两个队列相连
		if (_data_header != INVALIDE_INDEX)
		{
			if (_free_header != INVALIDE_INDEX)
			{
				_data[data_tailer].next = _free_header;
				_data[_free_header].prev = data_tailer;
			}
			_free_header = _data_header;
			_data_header = INVALIDE_INDEX;
		}
	}

	template<class TYPE>
	void table<TYPE>::swap(table<TYPE>& other)
	{
		_data.swap(other._data);
		std::swap(_count, other._count);
		std::swap(_data_header, other._data_header);
		std::swap(_free_header, other._free_header);
	}

	template<class TYPE>
	void table<TYPE>::copy(const table<TYPE>& src)
	{
		_data = src._data;
		_count = src._count;
		_data_header = src._data_header;
		_free_header = src._free_header;
	}

	template<class TYPE>
	TYPE* table<TYPE>::get_data(uint index)
	{
		if (index >= _data.size())
		{
			return NULL;
		}
		if (_data[index].is_free)
		{
			return NULL;
		}
		return (TYPE*)_data[index].data;
	}

	template<class TYPE>
	const TYPE* table<TYPE>::get_data(uint index) const
	{
		if (index >= _data.size())
		{
			return NULL;
		}
		if (_data[index].is_free)
		{
			return NULL;
		}
		return (TYPE*)_data[index].data;
	}

	template<class TYPE>
	uint table<TYPE>::get_count() const
	{
		return _count;
	}

	template<class TYPE>
	uint table<TYPE>::begin() const
	{
		return _data_header;
	}

	template<class TYPE>
	uint table<TYPE>::next(uint index) const
	{
		if (index >= _data.size())
		{
			return INVALIDE_INDEX;
		}
		if (_data[index].is_free)
		{
			return INVALIDE_INDEX;
		}
		return _data[index].next;
	}

	template<class TYPE>
	uint table<TYPE>::end() const
	{
		return INVALIDE_INDEX;
	}

	template<class TYPE>
	void table<TYPE>::inc_node_buf(uint size)
	{
		uint size_now = (uint)_data.size();
		size = BT_MAX(5, BT_MAX(size, size_now*2));
		_data.resize(size);

		for (uint i = size_now;i < size;i++)
		{
			node& n = _data[i];
			n.is_free = true;
			n.next = i + 1;
			n.prev = i - 1;
		}
		_data[size-1].next = _free_header;
		if (_free_header != INVALIDE_INDEX)
		{
			_data[_free_header].prev = size-1;
		}
		_data[size_now].prev = INVALIDE_INDEX;
		_free_header = size_now;
	}


	template<class TYPE>
	TYPE* table_ptr<TYPE>::get_data(uint index) const
	{
		TYPE** pp = (TYPE**)table<TYPE*>::get_data(index);
		if (pp == NULL)
		{
			return NULL;
		}
		return *pp;
	}

	template<class TYPE>
	bool  table_ptr<TYPE>::set_data(uint index, TYPE* p)
	{
		TYPE** pp = (TYPE**)table<TYPE*>::get_data(index);
		if (pp == NULL)
		{
			return false;
		}
		*pp = p;
		return true;
	}
}

