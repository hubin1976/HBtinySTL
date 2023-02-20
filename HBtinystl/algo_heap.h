#ifndef _HBTINYSTL_ALGO_HEAP_H_
#define _HBTINYSTL_ALGO_HEAP_H_

#include<iterator.h>
namespace HBtinySTL {
	/*不是插入一个元素，而是已经将新元素插入至最后一个尾后，用push_heap排序*/
	template<typename RandomAccessIterator>
	inline void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
		_push_heap(first, last, distance_type(first), value_type(first));
	}
	template<typename RandomAccessIterator, typename Distance, typename T>
	inline void _push_heap(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*) {
		__push_heap(first, Distance(last - first - 1), Distance(0), T(*(last - 1)));
	}

	template<typename RandomAccessIterator, typename Distance, typename T>
	void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value) {
		Distance parent = (holeIndex - 1) / 2;
		while (holeIndex > topIndex && *(first + parent) < value) {
			*(first + holeIndex) = *(first + parent);
			holeIndex = parent;
			parent = (holeIndex - 1) / 2;
		}
		*(first + holeIndex) = value;
	}

	template<typename RandomAccessIterator, typename Compare>
	inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare Comp) {
		_push_heap(first, last, distance_type(first), value_type(first), Comp);
	}
	template<typename RandomAccessIterator, typename Distance, typename T, typename Compare>
	inline void _push_heap(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, Compare Comp) {
		__push_heap(first, Distance(last - first - 1), Distance(0), T(*(last - 1)), Comp);
	}

	template<typename RandomAccessIterator, typename Distance, typename T, typename Compare>
	void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value, Compare Comp) {
		Distance parent = (holeIndex - 1) / 2;
		while (holeIndex > topIndex && Comp(*(first + parent), value)) {
			*(first + holeIndex) = *(first + parent);
			holeIndex = parent;
			parent = (holeIndex - 1) / 2;
		}
		*(first + holeIndex) = value;
	}

	/*由客户端再执行pop_back*/
	template<typename RandomAccessIterator>
	inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
		_pop_heap(first, last, value_type(first));
	}
	template<typename RandomAccessIterator, class T>
	inline void _pop_heap(RandomAccessIterator first, RandomAccessIterator last, T*) {
		__pop_heap(first, last - 1, last - 1, T(*(last - 1), distance_type(first));
	}
	template<typename RandomAccessIterator, class T, class Distance>
	inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value, Distance*) {
		*result = *first;
		__adjust_heap(first, Distance(0), Distance(last - first), value);
	}
	template<typename RandomAccessIterator, typename Distance, typename T>
	void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value) {
		Distance topIndex = holeIndex;
		Distance secondChild = 2 * holeIndex + 2;
		while (secondChild < len) {
			if (*(first + secondChild) < *(first + (secondChild - 1)))
				secondeChild--;
			*(first + holeIndex) = *(first + secondChild);
			holeIndex = secondChild;
			secondChild = 2 * (secondChild + 1);
		}
		if (secondChild == len) {
			*(first + holeIndex) = *(first + secondChild - 1);
			holeIndex = secondChild - 1;
		}
		*(first + holeIndex) = value;
	}

	template<typename RandomAccessIterator, typename Compare>
	inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare Comp) {
		_pop_heap(first, last, value_type(first), Comp);
	}
	template<typename RandomAccessIterator, class T, typename Compare>
	inline void _pop_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Compare Comp) {
		__pop_heap(first, last - 1, T(*(last - 1), distance_type(first), Comp);
	}
	template<typename RandomAccessIterator, class T, class Distance, typename Compare>
	inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value, Distance*, Compare Comp) {
		*result = *first;
		__adjust_heap(first, Distance(0), Distance(last - first), value, Comp);
	}
	template<typename RandomAccessIterator, typename Distance, typename T, typename Compare>
	void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value, Compare Comp) {
		Distance topIndex = holeIndex;
		Distance secondChild = 2 * holeIndex + 2;
		while (secondChild < len) {
			if (Comp(*(first + secondChild), *(first + (secondChild - 1))))
				secondeChild--;
			*(first + holeIndex) = *(first + secondChild);
			holeIndex = secondChild;
			secondChild = 2 * (secondChild + 1);
		}
		if (secondChild == len) {
			*(first + holeIndex) = *(first + secondChild - 1);
			holeIndex = secondChild - 1;
		}
		*(first + holeIndex) = value;
	}

	template<typename RandomAccessIterator, typename T, typename Distance>
	void _make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*) {
		if (last - first < 2) return;
		Distance len = last - first;
		Distance parent = (len - 2) / 2;

		while (true) {
			__adjust_heap(first, parent, len, T(*(first + parent)));
			if (parent == 0) return;
			parent--;
		}
	}
	template<typename RandomAccessIterator>
	inline void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
		_make_heap(first, last, value_type(first), distance_type(first));
	}

	template<typename RandomAccessIterator, typename T, typename Distance, typename Compare>
	void _make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*, Compare Comp) {
		if (last - first < 2) return;
		Distance len = last - first;
		Distance parent = (len - 2) / 2;

		while (true) {
			__adjust_heap(first, parent, len, T(*(first + parent)), Comp);
			if (parent == 0) return;
			parent--;
		}
	}

	template<typename RandomAccessIterator, typename Compare>
	inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare Comp) {
		_make_heap(first, last, value_type(first), distance_type(first), Comp);
	}

	template<typename RandomAccessIterator>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
		while (last - first > 1)
			pop_heap(first, last--);
	}

	template<typename RandomAccessIterator, typename Compare>
	void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
		while (last - first > 1)
			pop_heap(first, last--, Comp);
	}



}



#endif