
namespace bt
{

	inline c_list_node::c_list_node()
	{
		_next = _prev = NULL;
	}

	inline bool c_list_node::in_list() const
	{
		return _next != NULL;
	}

	inline const c_list_node* c_list_node::next() const
	{
		return (_next == this) ? NULL : _next;
	}

	inline const c_list_node* c_list_node::prev() const
	{
		return (_prev == this) ? NULL : _prev;
	}

	inline c_list_node* c_list_node::next()
	{
		return (_next == this) ? NULL : _next;
	}

	inline c_list_node* c_list_node::prev()
	{
		return (_prev == this) ? NULL : _prev;
	}

	inline const c_list_node* c_list_head::head() const
	{
		return _head;
	}

	inline const c_list_node* c_list_head::tail() const
	{
		return _tail;
	}

	inline c_list_node* c_list_head::head()
	{
		return _head;
	}

	inline c_list_node* c_list_head::tail()
	{
		return _tail;
	}

	inline bool c_list_head::empty() const
	{
		return _num == 0;
	}

	inline uint c_list_head::size() const
	{
		return _num;
	}


	inline c_list_next::c_list_next()
	{
		_next = NULL;
		_tail = NULL;
	}

	inline void c_list_next::push_tail(c_list_next* node)
	{
		if (_next == NULL)
		{
			_next = node;
		}
		else
		{
			_tail->_next = node;
		}
		if (node->_tail == NULL)
		{
			//如果node就一个节点的那
			_tail = node;
		}
		else
		{
			_tail = node->_tail;
		}
	}

	inline void c_list_next::push_head(c_list_next* node)
	{
		if (_next == NULL)
		{
			_next = node;
			if (node->_tail == NULL)
			{
				//如果node就一个节点的那
				_tail = node;
			}
			else
			{
				_tail = node->_tail;
			}
		}
		else
		{
			if (node->_tail == NULL)
			{
				node->_next = _next;
			}
			else
			{
				node->_tail->_next = _next;
			}
			_next = node;
		}
	}

	inline c_list_next* c_list_next::pop_head()
	{
		if (_next == NULL)
		{
			return NULL;
		}
		c_list_next* node = _next;
		_next = _next->_next;
		if (_next == NULL)
		{
			_tail = NULL;
		}
		node->_next = NULL;
		node->_tail = NULL;
		return node;
	}

	inline void c_list_next::append_tail(c_list_next* head)
	{
		if (head->_next == NULL)
		{
			return;
		}

		if (_next == NULL)
		{
			_next = head->_next;
			_tail = head->_tail;
		}
		else
		{
			_tail->_next = head->_next;
			_tail = head->_tail;
		}
		head->_next = NULL;
		head->_tail = NULL;
	}

	inline void c_list_next::append_head(c_list_next* head)
	{
		if (head->_next == NULL)
		{
			return;
		}

		if (_next == NULL)
		{
			_next = head->_next;
			_tail = head->_tail;
		}
		else
		{
			_next = head->_next;
			head->_tail->_next = _tail;
			_tail = head->_tail;
		}
		head->_next = NULL;
		head->_tail = NULL;
	}

	//
	inline c_list_next* c_list_next::next()
	{
		return _next;
	}
}
