#ifndef _HBTINYSTL_ALGOBASE_H_
#define _HBTINYSTL_ALGOBASE_H_

#include<stl_config.h>
#include<stl_pair.h>
#include<stl_relops.h>
#include<type_traits>

#include<string.h>
#include <limits.h>
#include <stdlib.h>
#include <stddef.h>
#include <new.h>
#include <iostream.h>

#include<iterator.h>

namespace HBtinySTL {
	/*  equel  */
	/*  若两序列在[first, last)中相等则true，若第二序列较长则忽略较长部分，第一序列较长则结果未知 */
	template<typename InputIterator1, typename InputIterator2>
	inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
		for (; first1 != last1; ++first1, ++first2) {
			if (*first1 != *first2) {
				return false;
			}
		}
		return true;
	}template<typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
		inline bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BinaryPredicate binary_pred) {
		for (; first1 != last1; ++first1, ++first2) {
			if (binary_pred(*first1, *first2) != true) {
				return false;
			}
		}
		return true;
	}


	/*  fill  */
	template<typename ForwardIterator, typename T>
	void fill(ForwardIterator first, ForwardIterator last, const T& x) {
		for (; first != last; ++first) {
			*first = value;
		}
	}


	/*  fill_n  */
	template<typename OutputIterator, typename size, typename T>
	OutputIterator fill_n(OutputIterator first, size n, const T& x) {
		for (; n > 0; --n, ++first) {
			*first = x;
		}
		return first;
	}


	/*iter_swap*/
	template<typename ForwardIterator1, typename ForwardITerator2>
	inline void iter_swap(ForwardIterator1 a, ForwardITerator2 b) {
		typename iterator_traits<ForwardIterator1>::value_type tmp = *a;
		*a = *b;
		*b = tmp;
	}


	/* lexicographical_compare */
	template<typename InputIterator1, typename InputIterator2>
	bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
		for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
			if (*first1 < *first2) {
				return true;
			}
			if (*first2 < *first1) {
				return false;
			}
		}
		return first1 == last1 && first2 != last2;
	}
	template<typename InputIterator1, typename InputIterator2, typename binary_predicate>
	bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
		InputIterator2 first2, InputIterator2 last2, binary_predicate comp) {
		for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
			if (comp(first1, first2)) {
				return  true;
			}
			if (comp(first2, first1)) {
				return false;
			}
		}
		return first1 == last1 && first2 != last2;
	}
	inline bool lexicographical_compare(const unsigned char* first1, const unsigned char* last1,
		const unsigned char* first2, const unsigned char* last2) {
		const size_t len1 = last1 - first1;
		const size_t len2 = last2 - first2;
		const int result = memcmp(first1, first2, min(len1, len2));
		return result != 0 ? result < 0 : len1 < len2;
	}


	/* max */
	template<typename T>
	inline const T& max(const T& a, const T& b) {
		return a < b ? b : a;
	}
	template<typename T, typename binary_predicate>
	inline cosnt T& max(const T& a, const T& b, binary_predicate comp) {
		return comp(a, b) ? b : a;
	}


	/* min */
	template<typename T>
	inline const T& min(const T& a, const T& b) {
		return a < b ? a : b;
	}
	template<typename T, typename binary_predicate>
	inline cosnt T& min(const T& a, const T& b, binary_predicate comp) {
		return comp(a, b) ? a : b;
	}


	/* swap */
	template<typename T>
	inline void swap(T& a, T& b) {
		T temp = a;
		a = b;
		b = temp;
	}


	/*mismatch*/
	template<typename InputIterator1, typename InputIterator2>
	pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator last1, InputIterator2 first2) {
		while (first1 != last1 && *first1 == *first2) {
			++first1;
			++first2;
		}
		return pair<InputIterator1, InputIterator2>(first1, first2);
	}
	template<typename InputIterator1, typename InputIterator2, typename binary_predicate>
	pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1, InputIterator last1, InputIterator2 first2
		binary_predicate binary_pre) {
		while (first1 != last1 && binary_pred(*first1, *first2) {
			++first1;
			++first2;
		}
		return pair<InputIterator1, InputIterator2>(first1, first2);
	}


	/* copy */
	template<typename RandomAccessIterator, typename OutputIterator, typename Distance>
	inline OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*) {
		for (Distance n = last - first; n > 0; --n, ++result, ++first) {
			*result = *first;
		}
		return result;
	}
	template<typename InputIterator, typename OutputIterator>
	inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag) {
		for (; first != last; ++result; ++first) {
			*result = *first;
		}
		return result;
	}
	template<typename RandomAccessIterator, typename OutputIterator>
	inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last
		OutputIterator result, random_access_iterator_tag) {
		return  __copy_d(first, last, result, distance_type(first));
	}
	template<typename T>
	inline T* __copy_t(const T* first, const T* last, T* result, __true_type) {
		memmove(result, first, sizeof(T) * (last - first));
		return result + (last - first);
	}
	template<typename T>
	inline T* __copy_t(const T* first, const T* last, T* result, __false_type) {
		return __copy_d(first, last, result, (ptrdiff_t*)0);
	}
	template<typename InputIterator, typename OutputIterator>
	class __copy_dispatch {
		OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
			return __copy(first, last, result, iterator_category(first));
		}
	};
	template<class T>
	class __copy_dispatch<T*, T*> {
		T* operator()(T* first, T* last, T* result) {
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return __copy_t(first, last, result, t());
		}
	};
	template<class T>
	class __copy_dispatch<const T*, T*> {
		T* operator()(const T* first, const T* last, T* result) {
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return __copy_t(first, last, result, t());
		}
	};
	template<typename InputIterator, typename OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
		return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
	}
	inline char* copy(const char* first, const char* last, char* result) {
		memmove(result, first, last - first);
		return result + last - first;
	}
	inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
		memmove(result, first, last - first);
		return result + last - first;
	}


	/* copy_backward */
	template<typename BidirectionIterator1, typename BidirectionIterator2, typename Distance>
	inline BidirectionIterator2 __copy_backward_d(BidirectionIterator1 first, BidirectionIterator1 last, BidirectionIterator2 result, Distance*) {
		for (Distance n = last - first; n > 0; --n, --last) {
			*result = *last;
		}
		return result;
	};
	template<typename BidirectionIterator1, typename BidirectionIterator2>
	inline BidirectionIterator2 __copy_backward(BidirectionIterator1 first, BidirectionIterator1 last, BidirectionIterator2 result, bidirectional_iterator_tag) {
		for (; first != last; --last, --result) {
			*result = *last;
		}
		return result;
	};
	template<typename BidirectionIterator1, typename BidirectionIterator2>
	inline BidirectionIterator2 __copy_backward(BidirectionIterator1 first, BidirectionIterator1 last, BidirectionIterator2 result, random_access_iterator_tag) {
		return _copy_barkward_d(first, last, result, distance_type(first));
	};
	template<typename BidirectionIterator1, typename BidirectionIterator2>
	inline BidirectionIterator2 __copy_backward_t(BidirectionIterator1 first, BidirectionIterator1 last, BidirectionIterator2 result, __true_type) {
		memmove(result, first, sizeof(T) * (last - first));
		return result;
	}
	template<typename BidirectionIterator1, typename BidirectionIterator2>
	inline BidirectionIterator2 __copy_backward_t(BidirectionIterator1 first, BidirectionIterator1 last, BidirectionIterator2 result, __true_type) {
		return _copy_barkward_d(first, last, result, (ptrdiff_t*)0);
	}
	template<typename BidirectionIterator1, typename BidirectionIterator2>
	class __copy_backward_dispatch {
		BidirectionIterator1 operator()(BidirectionIterator1 first, BidirectionIterator1 last, BidirectionIterator2 result) {
			return __copy_backward(first, last, result, iterator_category(first));
		}
	};
	template<typename T>
	class __copy_backward_dispatch<T*, T*> {
		T* operator()(T* first, T* last, T*) {
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return _copy_backward_t(first, last, t());
		}
	};
	template<typename T>
	class __copy_backward_dispatch<const T*, T*> {
		T* operator()(const T* first, const T* last, T*) {
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return _copy_backward_t(first, last, t());
		}
	};
	template<typename BidirectionIterator1, typename BidirectionIterator2>
	inline BidirectionIterator2 copy_backward(BidirectionIterator1 first, BidirectionIterator1 last, BidirectionIterator2 result) {
		return __copy_backward_dispatch<BidirectionIterator1, BidirectionIterator2>()(first, last, result);
	}

}

#endif