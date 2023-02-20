#ifndef _HBTINYSTL_VECTOR_H_
#define _HBTINYSTL_VECTOR_H_

#include <concept_checks.h>
#include <stddef.h>
#include <memory.h>
namespace HBtinySTL {

	template <typename T, class Alloc>
	class vector_base {
	protected:
		T* m_start;
		T* m_finish;
		T* m_end_of_storage;//记录内存使用

		typedef simple_alloc<T, Alloc> m_data_allocator;
		T* m_allocate(size_t n) {
			return m_data_allocator::allocate(n);
		}
		void m_deallocate(T* p, size_t n) {//对alloc做了封装, 作为vector内存的申请和释放函数
			m_data_allocator::deallocate(p, n);
		}

	public:
		typedef Alloc alloctor_type;
		allocator_type get_allocator() const {
			return allocator_type();
		}

		vector_base(const Alloc&) :m_start(0), m_finish(0), m_end_of_storage(0) {}
		vector_base(size_t n, const Alloc&) :m_start(0), m_finish(0), m_end_of_storage(0) {
			m_start = m_allocator(n);
			m_finish = m_start;
			m_end_of_storage = m_start + n;
		}
		~vector_base() { m_deallocate(m_start, m_end_of_storage - m_start); }//构造和析构中包含了对内存的申请和释放
	};

	template<typename T, typename Alloc = alloc>
	class vector :protected vector_base<T, Alloc> {
	private:
		typedef vector_base<T, Alloc> m_base;
	public:
		typedef T					value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef value_type* iterator;
		typedef const value_type* const_iterator;
		typedef const value_type& const_reference;
		typedef size_t				size_type;
		typedef ptrdiff_t			difference_type;

		typedef typename m_base::allocator_type allocator_type;
		allocator_type get_allocator() const { return m_base::get_allocator(); }

		typedef reverse_iterator<iterator> reverse_iterator;
		typedef reverse_iterator<const_iterator> const_reverse_iterator;

	protected:
		/*想指定位置插入一个元素，可以处理需要重新申请内存的请求*/
		void m_insert_aux(iterator position, const T& x) {
			if (m_finish != m_end_of_storage) {
				construct(m_finish, *(m_finish - 1));
				++m_finish;
				T x_copy = x;
				copy_backward(position, m_finish - 2, m_finish - 1);
				*position = x_copy;
			}
			else {
				const size_type old_size = size();
				const size_type len = old_size == 0 ? 1 : 2 * old_size;
				iterator new_start = m_allocate(len);
				iterator new_finish = new_start;
				try {
					new_finish = uninitialized_copy(m_start, position, new_start);
					construct(new_finish, x);
					++new_finish;
					new_finish = uninitialized_copy(position, m_finish, new_finish);
				}
				catch (...) {
					destroy(new_start, new_finish);
					m_deallocate(new_start, len);
				}
				destroy(begin(), end());
				m_deallocate(m_start, m_end_of_storage - m_start);
				m_start = new_start;
				m_finish = new_finish;
				m_end_of_storage = new_start + len;
			}
		}
		void m_insert_aux(iterator position) {
			m_insert_aux(position, T());
		}

	public:
		iterator begin() { return m_start; }
		const_iterator begin() const { return m_start; }
		iterator end() { return m_finish; }
		const_iterator end() { return m_finish; }

		reverse_iterator rbegin() {
			return reverse_iterator(end());
		}
		const_reverse_iterator rbegin() const {
			return const_reverse_iterator(end());
		}
		reverse_iterator rend() {
			return reverse_iterator(begin());
		}
		const_reverse_iterator rend() const {
			return const_reverse_iterator(begin());
		}

		size_type size() const {
			return size_type(end() - begin());
		}
		size_type max_size() const {
			return size_type(-1) / sizeof(T);
		}
		size_type capacity() const {
			return size_type(m_end_of_storage - begin());
		}
		bool empty() const { return begin() == end(); }

		reference operator[](size_type n) { return *(begin() + n); }
		const_reference operator[](size_type n) const { return *(begin() + n); }

		explicit vector(const allocator_type& x = allocator_type()) :m_base(x) {}
		vector(size_type n, const T& value, cosnt allocator_type& a = allocator_type())
			:m_base(n, a) {
			m_finish = uninitialized_fill_n(m_start, n, value);
		}
		explicit vector(const size_type n) :m_base(n, allocator_type()) {
			m_finish = uninitialized_fill_n(m_start, n, T());
		}
		vector(const vector<T, Alloc>& x) :m_base(x.size, x.get_allocator()) {
			m_finish = uninitialized_copy(x.begin(), x.end(), m_start);
		}
		vector(const T* first, const T* last, const allocator_type& a = allocator_type())
			: m_base(last - first, a) {
			m_finish = uninitialized_copy(first, last, m_start);
		}

		~vector() { destroy(m_start, m_finish); }//仅有内容的初始化和析构，内存的申请和释放在vector_base中进行

		vector<T, Alloc>& operator=(const vector<T, Alloc>& x);

		/*使得vector至少可以容纳 n 个元素*/
		void reserve(size_type n) {
			/
				if (capacity() < n) {
					const size_type old_size = size();
					iterator temp = m_allocate_and_copy(n, m_start, m_finish);
					destroy(m_start, m_finish);
					m_deallocate(m_start, m_end_of_storage - m_start);
					m_start = temp;
					m_finish = temp + old_size;
					m_end_of_storage = temp + n;
				}
		}

		/*指定vector内容 为 n 个 val */
		void assign(size_type n, const T& val) { m_fill_assign(n, val); }
		void m_fill_assign(size_type n, const T& val) {
			if (n > capacity()) {
				vector<T, Alloc> temp(n, val, get_allocateor());
				temp.swap(*this);
			}
			else if (n > size()) {
				fill(begin(), end(), val);
				m_finish = uninitialize_fill_n(m_finish, n - size(), val);
			}
			else
				erase(fill_n(begin(), n, val), end());
		}

		reference front() { return *begin(); }
		const_reference front() const { return *begin(); }
		reference back() { return *(end() - 1); }
		const_reference back() const { return *(end() - 1); }

		void push_back(const T& x) {
			if (m_finish != m_end_of_storage) {
				construct(m_finish, x);
				++m_finish;
			}
			else
				m_insert_aux(end(), x);
		}
		void push_back() {
			if (m_finish != m_end_of_storage) {
				construct(m_finish);
				++m_finish;
			}
			else
				m_insert_aux(end(), x);
		}
		void swap(vector<T, Allco>& x) {
			std::swap(m_start, x.m_start);
			std::swap(m_finish, x.m_finish);
			std::swap(m_end_of_storage, x.m_end_of_storage);
		}

		iterator insert(iterator position, const T& x) {
			size_type n = position - begin();
			if (m_finish != m_end_of_storage && position == end()) {
				construct(m_finish, x);
				++m_finish;
			}
			else
				m_insert_aux(position, x);
			return begin() + n;
		}
		iterator insert(iterator position) {
			size_type n = position - begin();
			if (m_finish != m_end_of_storage && position == end()) {
				construct(m_finish);
				++m_finish;
			}
			else
				m_insert_aux(position, x);
			return begin() + n;
		}

		/*向指定位置插入[first, last)中的内容，可以处理需要申请内存的情况*/
		void insert(iterator position, const_iterator first, const_iterator last) {
			if (first == last) return;
			size_type n = 0;
			distance(first, last, n);
			if (size_type(m_end_of_storage - m_finish) >= n) {
				const size_type elems = m_finish - position;
				iterator old_finish = m_finish;
				if (elems > n) {
					uninitialized_copy(m_finish - n, m_finish, m_finish);
					m_finish += n;
					copy_backward(position, old_finish - n, old_finish);
					copy(first, last, position);
				}
				else {
					uninitialized_copy(first + elems, last, m_finish);
					m_finish += n - elems;
					uninitialized_copy(position, elems, m_finish);
					m_finish += elems;
					copy(first, first + elems, position);
				}
			}
			else {
				const size_type old_size = size();
				const size_type len = old_size + max(old_size, n);
				iterator new_start = m_allocate(len);
				iterator new_finish = new_start;
				try {
					new_finish = uninitialized_copy(m_start, position, new_start);
					new_finish = uninitialized_copy(first, last, new_finish);
					new_finish = uninitialized_copy(position, m_finish, new_finish);
				}
				catch (...) {
					destroy(new_start, new_finish);
					m_deallocate(new_start, len);
				}
				destroy(m_start, m_finish);
				m_deallocate(m_start, m_end_of_storage - m_start);
				m_start = new_start;
				m_finish = new_finish;
				m_end_of_storage = new_start + len;
			}
		}

		/*向指定位置插入n个为x的内容，可以处理需要申请内存的情况*/
		void insert(iterator position, size_type n, const T& x) {
			m_fill_insert(position, n, x);
		}

		void m_fill_insert(iterator position, size_type n, const T& x) {
			if (n == 0) return;
			if (size_type(m_end_of_storage - m_finish) >= n) {
				T x_copy = x;
				const size_type elems = m_finish - position;
				iterator old_finish = m_finish;
				if (elems > n) {
					uninitialized_copy(m_finish - n, m_finish, m_finish);
					m_finish += n;
					copy_backward(position, m_finish - n, old_finish);
					fill(position, position + n, x_copy);
				}
				else {
					uninitialized_fill_n(m_finish, n - elems, x_copy);
					m_finish += n - elems;
					uninitialized_copy(position, old_finish, m_finish);
					m_finish += elems;
					fill(position, eld_finish, x_copy);
				}
			}
			else {
				const size_type old_size = size();
				const size_type len = old_size + max(old_size, n);
				iterator new_start = m_allocate(len);
				iterator new_finish = new_start;
				try {
					new_finish = uninitialized_copy(m_start, position, new_start);
					new_finish = uninitialized_fill_n(new_finish, n, x);
					new_finish = uninitialized_copy(position, m_finish, new_finish);
				}
				catch (...) {
					destroy(new_start, new_finish);
					m_deallocate(new_start, len);
				}
				destroy(m_start, m_finish);
				m_deallocate(m_start, m_end_of_storage - m_start);
				m_start = new_start;
				m_finish = new_finish;
				m_end_of_storage = new_start + len;
			}
		}

		void pop_back() {
			--m_finish;
			destroy(m_finish);
		}

		/*擦除指定位置中的元素，不释放内存*/
		iterator erase(iterator position) {
			if (position + 1 != end())
				copy(position + 1, m_finish, position);
			--m_finish;
			destroy(m_finish);
			return position;
		}
		iterator erase(iterator fisrt, iterator last) {
			iterator i = copy(last, m_finish, first);
			destroy(i, m_finish);
			m_finish = m_finish - (last - first);
			return first;
		}

		/*重新调整vector为new_size个 x */
		void resize(size_type new_size, const T& x) {
			if (new_size < size())
				erase(begin() + new_size, end());
			else
				insert(end(), new_size - size(), x);
		}
		void resize(size_type new_size) { resize(new_size, T()); }
		void clear() { erase(begin(), end()); }

	protected:
		iterator m_allocate_and_copy(size_type n, const_iterator first, const_iterator last) {
			iterator result = m_allocate(n);
			uninitialized_copy(first, last, result);
			return result;
		}

	};

	template<class T, class Alloc>
	inline bool operator==(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {
		return x.size() == y.size() && equeal(x.begin(), x.end(), y.begin());
	}

	template<class T, class Alloc>
	inline bool operator<(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {
		return lexicographical_compare(x.begin(), x.end(),
			y.begin(), y.end());
	}
	template<class T, class Alloc>
	vector<T, Alloc>& vector<T, Alloc>::operator=(const vector<T, Alloc>& x) {
		if (&x == this) return *this;
		const size_type xlen = x.size();
		if (xlen > capacity()) {
			iterator temp = m_allocate_and_copy(xlen, x.begin(), x.end());
			destroy(m_start, m_finish);
			m_deallocate(m_start, m_end_of_storage - m_start);
			m_start = temp;
			m_end_of_storage = m_start + xlen;
		}
		else if (size() >= xlen) {
			iterator i = copy(x.begin(), x.end(), begin());
			destroy(i, m_finish);
		}
		else {
			copy(x.begin(), x.begin() + size(), m_start);
			uninitialized_copy(x.begin() + size(), x.end(), m_finish);
		}
		m_finish = m_start + xlen;
	}


}
#endif
