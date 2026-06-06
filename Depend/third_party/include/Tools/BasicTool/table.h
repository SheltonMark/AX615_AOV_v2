/*

作者：文昌鱼
时间：2011.04.20
说明：真正0(1)级的表

*/


#ifndef __BASICTOOL_TABLE__
#define __BASICTOOL_TABLE__

#include "public_include.h"
#include "btstl_container.h"

namespace bt
{
	template<class TYPE>
	class table
	{
	public:
		table();
		~table();

	//接口
	public:
		uint new_data(uint index = INVALIDE_INDEX);			//
		void free_data(uint index);
		void clear();
		void swap(table<TYPE>& other);
		void copy(const table<TYPE>& src);

		TYPE* get_data(uint index);
		const TYPE* get_data(uint index) const;
		uint get_count() const;

		uint begin() const;									//返回INVALIDE_INDEX表示无数据
		uint next(uint index) const;
		uint end() const;

	//类型
	public:
		const static uint INVALIDE_INDEX = BT_MAX_UINT;

	//内部接口
	protected:
		void inc_node_buf(uint size);

	//数据
	protected:
		struct node
		{
			bool is_free;
			
			uint prev;
			uint next;

			char data[sizeof(TYPE)];
		};
		btvector<node> _data;
		uint _count;

		uint _data_header;
		uint _free_header;
	};

	template<class TYPE>
	class table_ptr : public table<TYPE*>
	{
	public:
		TYPE* get_data(uint index) const;
		bool  set_data(uint index, TYPE* p);
	};

}

#include "table.inl"

#endif

