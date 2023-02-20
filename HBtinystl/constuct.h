#ifndef _HBTINYSTL_CONSTRUCT_H_
#define _HBTINYSTL_CONSTRUCT_H_

#include<new.h>
namespace HBtinySTL {
	template<class T1, class T2>
	inline void _construct(T1* p, T2 value) {
		new((void*)p) T1(value);
	}
	template<class T1>
	inline void _construct(T1* p) {
		new((void*)p) T1();
	}

	template<class T1, class T2>
	inline void construct(T1* p, const T2& value) {
		_construct(p, value);
	}
	template<class T1>
	inline void construct(T1* p) {
		_construct(p);
	}

	template<class ForwardIterator>
	inline void __Destory_aux(ForwardIterator first, ForwardIterator last, __true_type) {
		for (; first != last; ++first) {
			destory(&*first);
		}
	}
	template<class ForwardIterator>
	inline void __Destory_aux(ForwardIterator first, ForwardIterator last, __false_type) {

	}
	template<class ForwardIterator, class _value_type>
	inline __Destory(ForwardIterator first, ForwardIterator last, _value_type vt) {
		typedef typename __type_traits<_value_type>::has_trivial_destructor
			_Trivial_destructor;
		__Destory_aux(first, last, _Trivial_destructor);
	}
	template<class T>
	inline void _destory(T* pointer) {
		pointer->~T();
	}
	template<class ForwardIterator>
	inline void _destory(ForwardIterator first, ForwardIterator last) {
		__Destory(first, last, value_type(first));
	}

	template<class T>
	inline void destroy(T* pointer) {
		_destroy(pointer);
	}
	template<class ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last) {
		_destroy(frist, last);
	}
}

#endif
