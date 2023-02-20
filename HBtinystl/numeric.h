#ifndef _HBTINYSTL_NUMERIC_H_
#define _HBTINYSTL_NUMERIC_H_

namespace HBtinySTL {
	template<typename InputIterator, typename T>
	T accumulate(InputIterator first, InputIterator last, T init) {
		for (; first != last; ++first) {
			init += *first;
		}
		return init;
	}
	template<typename InputIterator, typename T, typename binary_predicate>
	T accumulate(InputIterator first, InputIterator last, T init, binary_predicate binary_pre) {
		for (; first != last; ++first) {
			init = binary_pre(init, *first);
		}
		return init;
	}



	template<typename InputIterator, typename OutputIterator>
	OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result) {
		if (first == last) return result;
		*result = *first;
		iterator_traits<InputIterator>::value_type value = *first, temp;
		while (++first != last) {
			temp = *first;
			*++result = temp - value;
			value = temp;
		}
		return ++result;
	}
	template<typename InputIterator, typename OutputIterator, typename binary_predicate>
	OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result, binary_predicate binary_pre) {
		if (first == last) return result;
		*result = *first;
		iterator_traits<InputIterator>::value_type value = *first, temp;
		while (++first != last) {
			temp = *first;
			*++result = binary_pre(temp, value);
			value = temp;
		}
		return ++result;
	}



	template<typename InputIterator1, typename InputIterator2, typename T>
	T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init) {
		for (; first1 != last1; ++first1, ++first2) {
			init = init + (*first1 * *first2);
		}
		return init;
	}
	template<typename InputIterator1, typename InputIterator2, typename binary_predicate1, typename binary_predicate2, typename T>
	T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, binary_predicate1 binary_pre1, binary_predicate2 binary_pre2, T init) {
		for (; first1 != last1; ++first1, ++first2) {
			init = binary_pre1(init, binary_pre2(*first1, *first2));
		}
		return init;
	}




	template<typename InputIterator, typename OutputIterator>
	OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result) {
		if (first == last) return result;
		*result = *first;
		iterator_traits<InputIterator>::value_type value = *first;
		while (++first != last) {
			value = value + *first;
			*++result = value;
		}
		return ++result;
	}
	template<typename InputIterator, typename OutputIterator, typename binary_predicate>
	OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result, binary_predicate binary_pre) {
		if (first == last) return result;
		*result = *first;
		iterator_traits<InputIterator>::value_type value = *first;
		while (++first != last) {
			value = binary_pre(value, *first);
			*++result = value;
		}
		return ++result;
	}
}




#endif
