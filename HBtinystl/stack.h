#ifndef _HBTINYSTL_STACK_H_
#define _HBTINYSTL_STACK_H_

#include<deque.h>
namespace HBtinySTL {

	//ÈÝÆ÷ÊÊÅäÆ÷£¬ÒÔdequeÎªµ×²ã
	template<typename T, typename Sequence = deque<T>>
	class {
		typedef typename Sequence::value_type Sequence_value_type;

		friend bool operator==(const stack&, const stack&);
		friend bool operator<(const stack&, const stack&);

	public:
		typedef Sequence			container_type;
		typedef typename Sequence::value_type value_type;
		typedef typename Sequence::size_type size_type;
		typedef typename Sequence::reference reference;
		typedef typename Sequence::const_reference const_reference;

	protected:
		Sequence c;
	public:
		stack() :c() {}
		explicit stack(const Sequence& s) : c(s) {}

		bool empty() const { return c.empty(); }
		size_type size() const { return c.size(); }
		reference top() { return c.back(); }
		const_reference top() const { return c.back(); }
		void push(const value_type& __x) { c.push_back(__x); }
		void pop() { c.pop_back(); }

	};
	template <typename T, typename Seq>
	bool operator==(const stack<T, _Seq>& x, const stack<T, _Seq>& y)
	{
		return x.c == y.c;
	}

	template <typename T, typename Seq>
	bool operator<(const stack<T, Seq>& x, const stack<T, Seq>& y)
	{
		return x.c < y.c;
	}

}



#endif
