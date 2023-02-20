#ifndef _HBTINYSTL_UNINITIALIZED_H_
#define _HBTINYSTL_UNINITIALIZED_H_
namespace HBtinySTL {

	template<typename InputIterator, typename ForwardIterator>
	inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator start, __true_type) {
		return copy(first, last, start);
	}
	template<typename InputIterator, typename ForwardIterator>
	inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator start, __false_type) {
		ForwardIterator cur = start;
		for (; first != last; ++cur, ++first) {
			construct(&*first, cur);
		}
		return start;
	}
	template<typename InputIterator, typename ForwardIterator, class Value_type>
	inline ForwardIterator _uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator start, Value_type*) {
		typedef typename __type_traits<Value_type>::is_POD_type is_POD;
		return __uninitialized_copy_aux(first, last, start, is_POD);
	}
	template<typename InputIterator, typename ForwardIterator>
	inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator start) {
		return _uninitialized_copy(first, last, start, value_type(start));
	}


	template<typename ForwardIterator, typename size, typename T>
	inline ForwardIterator _uninitialized_fill_n_aux(ForwardIterator start, size n, const T&, _true_type) {
		return fill_n(start, n, x);
	}
	template<typename ForwardIterator, typename size, typename T>
	inline ForwardIterator _uninitialized_fill_n_aux(ForwardIterator start, size n, const T&, _false_type) {
		ForwardIterator cur = start;
		for (; n > 0; --n, ++cur) {
			construct(&*cur, x);
		}
		return start;
	}
	template<typename ForwardIterator, typename size, typename T, typename Value_type>
	inline ForwardIterator _uninitialized_fill_n(ForwardIterator start, size n, const T& x, Value_type*) {
		typedef typename _type_traits<Value_type>::is_POD_type is_POD;
		return _uninitialized_fill_n_aux(start, n, x, is_POD);
	}
	template<typename ForwardIterator, typename size, typename T>
	inline ForwardIterator uninitialized_fill_n(ForwardIterator start, size n, const T& x) {
		return _uninitialized_fill_n(start, n, x, value_type(start));
	}

	template<typename ForwardIterator, typename T>
	inline ForwardIterator _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, false_type) {
		retrun fill(first, last, x);
	}
	template<typename ForwardIterator, typename T>
	inline ForwardIterator _uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, true_type) {
		ForwardIterator cur = first;
		for (; cur != last; ++cur) {
			construct(&*cur, x);
		}
		return first;
	}
	template<typename ForwardIterator, typename T, typename Value_type>
	inline ForwardIterator _uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, Value_type*) {
		typedef typename _type_traits<Value_type>::is_POD_type is_POD;
		return _uninitialize_fill_aux(first, last, x, is_POD);
	}
	template<typename ForwardIterator, typename T>
	inline ForwardIterator uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x) {
		return _uninitialized_fill(first, last, x, value_type(x));
	}



}
#endif
