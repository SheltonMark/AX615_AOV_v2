
/*

作者：文昌鱼
时间：2011.07.16

C语言里LIST的实现方法

性能高，很多场合都很有用

*/

#ifndef __BASICTOOL_C_LIST__
#define __BASICTOOL_C_LIST__

#include "public_include.h"

namespace bt
{

	class c_list_node
	{
		friend class c_list_head;
	public:
		inline c_list_node();

	public:
		inline bool in_list() const;

		inline const c_list_node* next() const;
		inline const c_list_node* prev() const;
		inline c_list_node* next();
		inline c_list_node* prev();

	private:
		c_list_node* _next;
		c_list_node* _prev;
	};

	class c_list_head
	{
	public:
		c_list_head();

	public:
		void push_tail(c_list_node* node);
		void push_head(c_list_node* node);
		void pop(c_list_node* node);

		inline const c_list_node* head() const;
		inline const c_list_node* tail() const;
		inline c_list_node* head();
		inline c_list_node* tail();

		void append(c_list_head* list);
		void clear();
		void swap(c_list_head* list);

		inline bool empty() const;
		inline uint size() const;

	private:
		c_list_node* _head;
		c_list_node* _tail;
		uint _num;
	};

	//即是节点，也是头部
	class c_list_next
	{
	public:
		inline c_list_next();

	public:
		//这三个接口队列头调用
		inline void push_tail(c_list_next* node);	//合并多个节点，到尾部，包含node
		inline void push_head(c_list_next* node);	//合并多个，到头部，包含node
		inline c_list_next* pop_head();				//移除头部
		inline void append_tail(c_list_next* head);	//合并多个节点，到尾部，不包含head
		inline void append_head(c_list_next* head);	//合并多个节点，到头部，不包含head

		//
		inline c_list_next* next();				//获取下一个节点

	private:
		c_list_next* _next;
		c_list_next* _tail;						//节点头部才有效
	};
}

#include "c_list.inl"

#endif

