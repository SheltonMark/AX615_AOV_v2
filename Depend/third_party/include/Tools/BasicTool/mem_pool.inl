
namespace bt
{
//	template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM>
//	mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::mem_pool()
//		:_free_node(NULL), _free_node_num(0), _max_size(BT_MAX_UINT)
//	{
//		assert(BLOCK_SIZE > 0);
//
//		//构造静态成员
//		for (uint i = 0;i < STATIC_BLOCK_NUM;i++)
//		{
//			BT_CALL_CONSTRUCT(&get_static_block()[i], mem_block, ());
//		}
//
//		//静态成员
//		for (uint i = 0;i < STATIC_BLOCK_NUM;i++)
//		{
//			for (uint j = 0;j < BLOCK_SIZE;j++)
//			{
//				push_free_node(&(get_static_block()[i].node[j]));
//			}
//		}
//	}
//	template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM>
//	mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::~mem_pool()
//	{
//		free_pool();
//
//		//内存泄漏判断
//		if (_free_node_num != BLOCK_SIZE*STATIC_BLOCK_NUM)
//		{
//			printf("mem_pool内存泄漏。");
//		}
//
//		//析构
//		for (uint i = 0;i < STATIC_BLOCK_NUM;i++)
//		{
//			BT_CALL_DESTRUCTOR(&get_static_block()[i], mem_block);
//		}
//	}
//
//	template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM>
//	typename mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::mem_block* mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::get_node_block(mem_node* node)
//	{
//		return (mem_block*)(node - node->index);
//	}
//
//	template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM>
//	void mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::push_free_node(mem_node* node)
//	{
//		if (_free_node == NULL)
//		{
//			_free_node = node;
//			_free_node->next_free_node = NULL;
//			_free_node->prev_free_node = NULL;
//		}
//		else
//		{
//			node->next_free_node = _free_node;
//			node->prev_free_node = NULL;
//			_free_node->prev_free_node = node;
//			_free_node = node;
//		}
//		get_node_block(node)->free_node_num++;
//		_free_node_num++;
//	}
//
//	template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM>
//	typename mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::mem_node* mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::pop_free_node()
//	{
//		if (_free_node == NULL)
//		{
//			return NULL;
//		}
//		mem_node* node = _free_node;
//		pop_free_node(_free_node);
//		return node;
//	}
//
//	template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM>
//	void mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::pop_free_node(mem_node* node)
//	{
//		if (node->next_free_node)
//		{
//			node->next_free_node->prev_free_node = node->prev_free_node;
//		}
//		if (node->prev_free_node)
//		{
//			node->prev_free_node->next_free_node = node->next_free_node;
//		}
//		else
//		{
//			assert(node == _free_node);
//			_free_node = node->next_free_node;
//		}
//		//这两个值为0并不能表示不再free列表。比如free列表就一个项的时候，这两个值都是0。
//		//node->next_free_node = NULL;
//		//node->prev_free_node = NULL;
//		get_node_block(node)->free_node_num--;
//		_free_node_num--;
//	}
//
//	template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM>
//	bool mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::new_free_block(uint block_num)
//	{
//		for (uint i = 0;i < block_num;i++)
//		{
//			mem_block* block = new(std::nothrow) mem_block;
//			if (block == NULL)
//			{
//				return false;
//			}
//			block->is_static = false;
//
//			auto_lock l(_mtx);
//			for (uint j = 0;j < BLOCK_SIZE;j++)
//			{
//				push_free_node(&(block->node[j]));
//			}
//		}
//		return true;
//	}
//
//	template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM>
//	void mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::remove_free_block(mem_block* block)
//	{
//		for (int i = 0;i < BLOCK_SIZE;i++)
//		{
//			pop_free_node(&block->node[i]);
//		}
//	}
//
//	template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM>
//	typename mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::mem_block* mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::get_static_block()
//	{
//		return (mem_block*)_static_block;
//	}
//
//	template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM>
//	bool mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::new_item(TYPE*& item, uint count)
//	{
//		auto_lock l(_mtx);
//		if (_free_node_num < count)
//		{
//			uint block_num;
//
//fuck:
//			block_num = (count - _free_node_num + BLOCK_SIZE - 1) / BLOCK_SIZE;
//			l.unlock();
//			if (new_free_block(block_num) == false)
//			{
//				for (uint i = 0;i < count;i++)
//				{
//					BT_GET_REF_OFF(item, i) = NULL;
//				}
//				return false;
//			}
//
//			l.lock();
//			if (_free_node_num < count)
//			{
//				goto fuck;
//			}
//		}
//
//		for (uint i = 0;i < count;i++)
//		{
//			BT_GET_REF_OFF(item, i) = &(pop_free_node()->data);
//		}
//
//		return true;
//	}
//
//	template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM>
//	void mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::free_item(TYPE* const& item, uint count)
//	{
//		auto_lock l(_mtx);
//		for (uint i = 0;i < count;i++)
//		{
//			TYPE* p = BT_GET_REF_OFF(item, i);
//			if (p)
//			{
//				mem_node* node = (mem_node*)p;
//				push_free_node(node);
//
//				if (_free_node_num > _max_size)
//				{
//					mem_block* block = get_node_block(node);
//					if ((block->free_node_num == BLOCK_SIZE)
//						&& (block->is_static == false))
//					{
//						//从自由列表中删除
//						remove_free_block(block);
//						l.unlock();
//
//						delete block;
//
//						l.lock();
//					}
//				}
//			}
//		}
//	}
//
//	template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM>
//	void mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::set_pool_size(uint size)
//	{
//		_max_size = BT_MAX(size, BLOCK_SIZE * STATIC_BLOCK_NUM);
//	}
//
//	template<class TYPE, class LOCK_TYPE, int BLOCK_SIZE, int STATIC_BLOCK_NUM>
//	void mem_pool<TYPE, LOCK_TYPE, BLOCK_SIZE, STATIC_BLOCK_NUM>::free_pool()
//	{
//#pragma warning(push)
//#pragma warning(disable:4311)
//#pragma warning(disable:4312)
//		mem_block* delete_block = NULL;
//
//		auto_lock l(_mtx);
//		mem_node* node = _free_node;
//		while (node)
//		{
//			mem_block* block = get_node_block(node);
//			if ((block->free_node_num == BLOCK_SIZE)
//				&& (block->is_static == false))
//			{
//				mem_node* prev_node = node->prev_free_node;
//				remove_free_block(block);
//
//				//添加到即将删除的列表，利用free_node_num作为next_block字段
//				if (delete_block)
//				{
//					block->free_node_num = (uint)delete_block;
//					delete_block = block;
//				}
//				else
//				{
//					block->free_node_num = 0;
//					delete_block = block;
//				}
//
//				//下一个
//				if (prev_node)
//				{
//					node = prev_node->next_free_node;
//				}
//				else
//				{
//					node = _free_node;
//				}
//			}
//			else
//			{
//				node = node->next_free_node;
//			}
//		}
//		l.unlock();
//
//		//删除BLOCK
//		while (delete_block)
//		{
//			mem_block* b = delete_block;
//			delete_block = (mem_block*)delete_block->free_node_num;
//			delete b;
//		}
//#pragma warning(pop)
//	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<class TYPE, class LOCK_TYPE>
	simple_mem_pool<TYPE, LOCK_TYPE>::simple_mem_pool()
	{
		_free_node = NULL;
		_free_num = 0;
		_max_size = BT_MAX_UINT;
	}

	template<class TYPE, class LOCK_TYPE>
	simple_mem_pool<TYPE, LOCK_TYPE>::~simple_mem_pool()
	{
		free_pool();
	}

	template<class TYPE, class LOCK_TYPE>
	bool simple_mem_pool<TYPE, LOCK_TYPE>::new_item(TYPE*& item, uint count)
	{
		auto_lock<LOCK_TYPE> l(_mtx);
		if (_free_num < count)
		{
			l.unlock();

			for (uint i = 0;i < count;i++)
			{
				mem_node* t = new(std::nothrow) mem_node;
				if (t == NULL)
				{
					for (uint j = i - 1;j >= 0;j--)
					{
						delete (mem_node*)BT_GET_REF_OFF(item, j);
					}
					for (uint j = 0;j < count;j++)
					{
						BT_GET_REF_OFF(item, j) = NULL;
					}
					return false;
				}
				BT_GET_REF_OFF(item, i) = &t->data;
			}
		}
		else
		{
			for (uint i = 0;i < count;i++)
			{
				BT_GET_REF_OFF(item, i) = &_free_node->data;
				_free_node = _free_node->next;
			}
			_free_num -= count;
		}
		return true;
	}

	template<class TYPE, class LOCK_TYPE>
	void simple_mem_pool<TYPE, LOCK_TYPE>::delete_item(TYPE* const& item, uint count)
	{
		auto_lock<LOCK_TYPE> l(_mtx);
		if (_free_num > _max_size)
		{
			l.unlock();

			for (uint i = 0;i < count;i++)
			{
				mem_node* t = (mem_node*)BT_GET_REF_OFF(item, i);
				if (t)
				{
					delete t;
				}
			}
		}
		else
		{
			for (uint i = 0;i < count;i++)
			{
				mem_node* t = (mem_node*)BT_GET_REF_OFF(item, i);
				if (t)
				{
					t->next = _free_node;
					_free_node = t;
					_free_num++;
				}
			}
		}
	}

	template<class TYPE, class LOCK_TYPE>
	void simple_mem_pool<TYPE, LOCK_TYPE>::set_pool_size(uint size)
	{
		_max_size = size;
	}

	template<class TYPE, class LOCK_TYPE>
	void simple_mem_pool<TYPE, LOCK_TYPE>::free_pool()
	{
		mem_node* free_node;
		auto_lock<LOCK_TYPE> l(_mtx);
		free_node = _free_node;
		_free_node = NULL;
		_free_num = 0;
		l.unlock();

		while (free_node)
		{
			mem_node* t = free_node;
			free_node = free_node->next;
			delete t;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template<class TYPE>
	nolock_mem_pool<TYPE>::mem_node_list::mem_node_list()
	{
		first = last = NULL;
		size = 0;
	}

	template<class TYPE>
	inline void nolock_mem_pool<TYPE>::mem_node_list::push(mem_node* node)
	{
		if (size == 0)
		{
			first = last = node;
			last->next = NULL;
			size = 1;
		}
		else
		{
			node->next = first;
			first = node;
			size++;
		}
	}
	template<class TYPE>
	inline typename nolock_mem_pool<TYPE>::mem_node* nolock_mem_pool<TYPE>::mem_node_list::pop()
	{
		if (size == 0)
		{
			return NULL;
		}
		else
		{
			mem_node* node = first;
			first = first->next;
			size--;
			return node;
		}
	}
	template<class TYPE>
	void nolock_mem_pool<TYPE>::mem_node_list::push(mem_node_list& nl)
	{
		if (nl.size == 0)
		{
			return;
		}
		else if (size == 0)
		{
			*this = nl;
		}
		else
		{
			last->next = nl.first;
			last = nl.last;
			size += nl.size;
		}
	}

	template<class TYPE>
	nolock_mem_pool<TYPE>::nolock_mem_pool()
	{
		_array_size = 1;
		_block_size = BT_MAX(1, 4096 / get_node_size());
		_backup_size = BT_MAX(10, _block_size);
		_max_size = BT_MAX_UINT;
		_global_backup_node[0].reserve(10);
		_global_backup_node[1].reserve(4);
		_global_node_size = 0;

		_thread_node_list.set_destructor_cb(this);
	}
	template<class TYPE>
	nolock_mem_pool<TYPE>::~nolock_mem_pool()
	{
		_thread_node_list.set_destructor_cb(NULL);

		//不可销毁
		if (_block_size == 1)
		{
			for (int i = 0;i < 2;i++)
			{
				for (uint j = 0;j < _global_backup_node[i].size();j++)
				{
					mem_node_list& nl = _global_backup_node[i][j];
					delete_node_list(nl);
				}
				_global_backup_node[i].clear();
			}
		}
	}

	template<class TYPE>
	void nolock_mem_pool<TYPE>::set_option(uint block_size, uint backup_size)
	{
		if (block_size != BT_MAX_UINT)
		{
			_block_size = BT_MAX(1, block_size);
		}
		if (backup_size != BT_MAX_UINT)
		{
			_backup_size = backup_size;
		}
	}

	template<class TYPE>
	void nolock_mem_pool<TYPE>::set_max_size(uint max_size)
	{
		_max_size = max_size;
	}

	template<class TYPE>
	void nolock_mem_pool<TYPE>::set_array_size(uint array_size)
	{
		_array_size = BT_MAX(1, array_size);
		_block_size = BT_MAX(1, 4096 / get_node_size());
		_backup_size = BT_MAX(10, _block_size);
	}

	template<class TYPE>
	void nolock_mem_pool<TYPE>::on_tlp_free_resource(int threadid, void* value, ptr_int user_data)
	{
		mem_node_list* tnode = (mem_node_list*)value;
		if (tnode->size > 0)
		{
			push_global_backup_node(*tnode);
		}
		delete tnode;
	}

	template<class TYPE>
	bool nolock_mem_pool<TYPE>::need_thread_node_list_size(mem_node_list* tnode, uint count)
	{
		while (tnode->size < count)
		{
			mem_node_list nl;
			if (need_node_list(nl) == false)
			{
				return false;
			}
			tnode->push(nl);
		}
		return true;
	}

	//
	template<class TYPE>
	bool nolock_mem_pool<TYPE>::new_item(TYPE*& item)
	{
		mem_node_list* tnode = get_thread_node_list();
		if (tnode->size < 1)
		{
			if (need_thread_node_list_size(tnode, 1) == false)
			{
				item = NULL;
				return false;
			}
		}
		mem_node* node = tnode->pop();
		item = node->data;
		return true;
	}

	//
	template<class TYPE>
	bool nolock_mem_pool<TYPE>::new_item(TYPE*& item, uint count)
	{
		mem_node_list* tnode = get_thread_node_list();
		if (tnode->size < count)
		{
			if (need_thread_node_list_size(tnode, count) == false)
			{
				for (uint i = 0;i < count;i++)
				{
					BT_GET_REF_OFF(item, i) = NULL;
				}
				return false;
			}
		}
		for (uint i = 0;i < count;i++)
		{
			mem_node* node = tnode->pop();
			BT_GET_REF_OFF(item, i) = node->data;
		}
		return true;
	}

	template<class TYPE>
	void nolock_mem_pool<TYPE>::dec_thread_node_list_size(mem_node_list* tnode)
	{
		while (tnode->size > (_backup_size * 2))
		{
			mem_node_list nl;
			for (uint i = 0;i < _backup_size;i++)
			{
				mem_node* node = tnode->pop();
				nl.push(node);
			}
			push_global_backup_node(nl);
		}
	}

	template<class TYPE>
	void nolock_mem_pool<TYPE>::delete_item(TYPE* const& item)
	{
		mem_node_list* tnode = get_thread_node_list();
		TYPE* pitem = item;
		if (pitem)
		{
			mem_node* node = (mem_node*)((char*)pitem - sizeof(mem_node*));
			tnode->push(node);
		}
		if (tnode->size > (_backup_size * 2))
		{
			dec_thread_node_list_size(tnode);
		}
	}

	template<class TYPE>
	void nolock_mem_pool<TYPE>::delete_item(TYPE* const& item, uint count)
	{
		mem_node_list* tnode = get_thread_node_list();
		for (uint i = 0;i < count;i++)
		{
			TYPE* pitem = BT_GET_REF_OFF(item, i);
			if (pitem)
			{
				mem_node* node = (mem_node*)((char*)pitem - sizeof(mem_node*));
				tnode->push(node);
			}
		}
		if (tnode->size > (_backup_size * 2))
		{
			dec_thread_node_list_size(tnode);
		}
	}

	template<class TYPE>
	void nolock_mem_pool<TYPE>::push_global_backup_node(mem_node_list& nl)
	{
		if (_block_size == 1)
		{
			if ((_global_node_size + nl.size) > _max_size)
			{
				delete_node_list(nl);
				return;
			}
		}
		
		//保持辅队列的数量为2
		if (_global_backup_node[1].size() < 2)
		{
			if (_global_backup_node_mtx[1].try_lock())
			{
				_global_backup_node[1].push_back(nl);
				_global_backup_node_mtx[1].unlock();
				return;
			}
		}

		if (_global_backup_node_mtx[0].try_lock())
		{
			_global_backup_node[0].push_back(nl);
			_global_node_size += nl.size;
			_global_backup_node_mtx[0].unlock();
			return;
		}

		_global_backup_node_mtx[1].lock();
		_global_backup_node[1].push_back(nl);
		_global_backup_node_mtx[1].unlock();
	}

	template<class TYPE>
	bool nolock_mem_pool<TYPE>::pop_global_backup_node(mem_node_list& nl)
	{
		if (_global_backup_node[1].size() > 2)
		{
			//辅队列最多放2个，防止锁冲突的时候用。其余的优先分配
			if (_global_backup_node_mtx[1].try_lock())
			{
				if (_global_backup_node[1].empty() == false)
				{
					nl = _global_backup_node[1].back();
					_global_backup_node[1].pop_back();
					_global_backup_node_mtx[1].unlock();
					return true;
				}

				_global_backup_node_mtx[1].unlock();
			}
		}

		if (_global_backup_node[0].empty() == false)
		{
			if (_global_backup_node_mtx[0].try_lock())
			{
				if (_global_backup_node[0].empty() == false)
				{
					nl = _global_backup_node[0].back();
					_global_backup_node[0].pop_back();
					_global_node_size -= nl.size;
					_global_backup_node_mtx[0].unlock();
					return true;
				}

				_global_backup_node_mtx[0].unlock();
			}
		}

		if (_global_backup_node[1].empty() == false)
		{
			_global_backup_node_mtx[1].lock();
			if (_global_backup_node[1].empty() == false)
			{
				nl = _global_backup_node[1].back();
				_global_backup_node[1].pop_back();			//_global_node_size不包含辅队列的东西
				_global_backup_node_mtx[1].unlock();
				return true;
			}
			_global_backup_node_mtx[1].unlock();
		}

		if (_global_backup_node[0].empty() == false)
		{
			_global_backup_node_mtx[0].lock();
			if (_global_backup_node[0].empty() == false)
			{
				nl = _global_backup_node[0].back();
				_global_backup_node[0].pop_back();
				_global_node_size -= nl.size;
				_global_backup_node_mtx[0].unlock();
				return true;
			}
			_global_backup_node_mtx[0].unlock();
		}

		return false;
	}

	template<class TYPE>
	bool nolock_mem_pool<TYPE>:: need_node_list(mem_node_list& nl)
	{
		if (pop_global_backup_node(nl))
		{
			return true;
		}

		return new_node_list(nl);
	}

	template<class TYPE>
	bool nolock_mem_pool<TYPE>::new_node_list(mem_node_list& nl)
	{
		//申请内存
		uint node_size = get_node_size();	//必须保证字对齐
		uint mem_size = node_size * _block_size;
		char* mem = new (std::nothrow) char[mem_size];
		if (mem == NULL)
		{
			return false;
		}

		//调用构造函数
		for (uint i = 0;i < _block_size;i++)
		{
			mem_node* node = (mem_node*)(mem + node_size * i);
			for (uint j = 0;j < _array_size;j++)
			{
				BT_CALL_CONSTRUCT(node->data + j, TYPE, );
			}

			nl.push(node);
		}

		return true;
	}

	template<class TYPE>
	void nolock_mem_pool<TYPE>::delete_node_list(mem_node_list& nl)
	{
		while (true)
		{
			mem_node* node = nl.pop();
			if (node == NULL)
			{
				break;
			}

			//调用析构函数
			for (uint j = 0;j < _array_size;j++)
			{
				BT_CALL_DESTRUCTOR(node->data + j, TYPE);
			}

			//销毁内存
			delete[] (char*)node;
		}
	}

	template<class TYPE>
	uint nolock_mem_pool<TYPE>::get_node_size()
	{
		return sizeof(mem_node*) + (_array_size * sizeof(TYPE) + sizeof(int) - 1) / sizeof(int) * sizeof(int);
	}

	template<class TYPE>
	inline typename nolock_mem_pool<TYPE>::mem_node_list* nolock_mem_pool<TYPE>::get_thread_node_list()
	{
		mem_node_list* tnode = (mem_node_list*)_thread_node_list.get_value();
		if (tnode == NULL)
		{
			tnode = new mem_node_list;
			_thread_node_list.set_value(tnode);
		}
		return tnode;
	}
}

