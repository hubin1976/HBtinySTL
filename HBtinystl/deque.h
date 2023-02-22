#ifndef _HBTINYSTL_DEQUE_H_
#define _HBTINYSTL_DEQUE_H_

#include<iterator.h>

#include<memory.h>
namespace HBtinySTL {
	inline size_t __deque_buf_size(size_t __size) {            //计算一个node中元素的个数
		return __size < 512 ? size_t(512 / __size) : size_t(1);
	}

	template<typename T, typename Ref, typename Ptr>
	class deque_iterator {
	private:
		typedef T value_type;
		typedef Ptr	pointer;
		typedef Ref reference;
		typedef ptrdiff_t difference_type;
		typedef random_access_iterator_tag	iterator_category;
		typedef size_t	size_type;
		static size_t _S_buffer_size() { return __deque_buf_size(sizeof(T))； }

		typedef deque_iterator		self;
		typedef deque_iterator<T, T&, T*>	iterator;
		typedef deque_iterator<T, const T&, const T*>	const_iterator;
		typedef T** map_pointer;

		T* m_cur;
		T* m_first;
		T* m_last;		//此node中的当前位置、头和尾
		map_pointer m_node;//指向中控器map中的当前node

		deque_iterator(T* x, map_pointer y)
			:m_cur(x), m_first(*y), m_last(*y + _S_buffer_size()), m_node(y) {}
		deque_iterator() :m_cur(0), m_first(0), m_last(0), m_node(0) {}
		deque_iterator(const iterator& x) :m_cur(x.m_cur), m_first(x.m_first), m_last(x.m_last)
			m_node(x.m_node) {}

		reference operator*()const { return *m_cur; }
		pointer operator->()const { return m_cur; }

		difference_type operator-(const self& x) const {
			return difference_type(_S_buffer_size()) * (m_node - x.m_node - 1) +
				（m_cur - m_first) + (x.m_last - x.m_cur);
		}

		self& operator++() {
			++m_cur;
			if (m_cur == m_last) {
				m_set_node(m_node + 1);
				m_cur = m_first;
			}
			return *this;
		}
		self& operator++(int) {
			self temp = *this;
			++（* this）;
			return temp;
		}
		self& operator--() {
			if (m_cur == m_first) {
				m_set_node(m_node - 1);
				m_cur = m_last;
			}
			--m_cur;
			return *this;
		}
		self& operator--(int) {
			self temp = *this;
			--(*this);
			return *this;
		}
		self& operator+=(difference_type n) {
			difference_type offset = n + (m_cur - m_first);//以m_first为下标0
			if (offset >= 0 && offset < difference_type(_S_buffer_size()))
				m_cur += n;
			else {
				differece_type node_offset =
					offset > 0 ? offset / difference_type(_S_buffer_size())
					: -difference_type((-offset - 1) / _S_buffer_size()) - 1;//m_first为下标0，所以需要先-1，然后向上取整
				m_set_node(m_node + node_offset);
				m_cur = m_first + (offset - node_offset * difference_type(_S_buffer_size()));
			}
			return *this;
		}
		self& operator+(difference_type n) {
			self temp = *this;
			return temp += n;
		}
		self& operator-=(difference_type n) { return *this += -n; }
		self& operator-(difference_type n) {
			self temp = *this;
			return temp -= n;
		}

		reference operator[](difference_type n) { return *(*this + n); }

		bool operator==(const self& x) const { return m_cur == x.m_cur; }
		bool operator!=(const self& x) const { return !(*this == x); }
		bool operator<(const self& x) const {
			return (m_node == x.m_node) ? m_cur < x.m_cur : m_node < x.m_node;
		}
		bool operator>(const self& x) const { return x < *this; }
		bool operator<=(const self& x) const { return !(x < *this); }
		bool operator>=(const self& x) const { return !(*this < x); }

		void m_set_node(map_pointer new_node) {//只是移动m_node，并不申请或释放内存
			m_node = new_node;
			m_first = *new_node;
			m_last = m_first + difference_type(_S_buffer_size());
		}
	};

	template<typename T, typename Ref, typename Ptr>
	inline deque_iterator<T, Ref, Ptr>
		operator+(ptrdiff_t n, const deque_iterator<T, Ref, Ptr>& x) {
		return x + n;
	}

	template<typename T, typename Alloc>
	class deque_base {
		typedef deque_iterator<T, T&, T*> iterator;
		typedef deque_iterator<T, const T&, const T*> const_iterator;
		typedef Alloc allocator_type;
		allocator_type get_allocator() const { return allocator_type(); }

		deque_base(const allocator_type&) :m_map(0), m_map_size(0), m_start(), m_finish() {}
		deque_base(const allocator_type&, size_t num_element) :m_map(0), m_map_size(0), m_start(), m_finish() {
			m_initialize_map(num_element);
		}
		~deque_base();

	protected:
		void m_initialize_map(size_t num_element);
		void m_create_nodes(T** node_start, T** node_finish);
		void m_destroy_nodes(T** node_start, T** node_finish);
		enum { _S_initial_map_size = 8 };

	protected:
		T** m_map;			//中控器的开始
		size_t m_map_size;	//中控器的大小
		iterator m_start;	//头节点
		iterator m_finish;	//尾节点

		typedef simple_alloc<T, Alloc> node_alloc_type;
		typedef simple_alloc<T*, Alloc> map_alloc_type;		//map中元素是node地址，为T*类型，所以m_map为T**类型

		T* m_allocate_node() { return node_alloc_type::allocate(__deque_buf_size(sizeof(T))); }
		void m_deallocate_node(T* p) {
			node_alloc_type::deallocate(p, __deque_buf_size(sizeof(T)));
		}
		T** m_allocate_map(size_t n) { return map_alloc_type::allocate(n); }
		void m_deallocate_map(T** p, size_t n) { map_alloc_type::deallocate(p, n); }
	};

	template<typename T, typename Alloc>
	deque_base<T, Alloc>::~deque_base() {
		if (m_map) {
			m_deatroy_nodes(m_start.m_node, m_finish.m_node + 1);
			m_deallocate_map(m_map, m_map_size);
		}
	}
	template<typename T, typename Alloc>
	void deque_base<T, Alloc>::m_initialize_map(size_t num_elements) {
		size_t num_node = num_element / __deque_buf_size(sizeof(T)) + 1;
		m_map_size = max((size_t)_S_initial_map_size, num_node + 2);
		m_map = m_allocate_map(m_map_size);
		T** node_start = m_map + (m_map_size - num_node) / 2;//预留的空间两侧均匀分配
		T** node_finish = node_start + num_node;
		try {
			m_create_nodes(node_start, node_finish);	//为map上申请node内存并记录
		}
		catch (...) {
			m_deallocate_map(m_map, m_map_size);
			m_map = 0;
			m_map_size = 0;
		}
		m_start.m_set_node(node_start);
		m_finish.m_set_node(node_finish - 1);
		m_start.m_cur = m_start.m_first;
		m_finish.m_cur = m_finish.m_first + num_element % __deque_buf_size(sizeof(T));
	}

	/*作用是申请内存填充map中的node*/
	template<typename T, typename Alloc>
	void deque_base<T, Alloc>::m_create_nodes(T** node_start, T** node_finish) {
		T** cur;
		try {
			for (cur = node_start; cur < node_finish; ++cur) {
				*cur = m_allocate_node();
			}
		}
		catch (...) {
			m_destroy_nodes(node_start, cur);
		}
	}
	/*作用是释放map中的node*/
	template<typename T, typename Alloc>
	void deque_base<T, Alloc>::m_destroy_nodes(T** node_start, T** node_finish) {
		for (T** temp = node_start; temp < node_finish; ++temp)
			m_deallocate_node(*n);
	}

	template<typename T, typename Alloc = alloc>
	class deque :protected deque_base<T, Alloc> {
		typedef deque_base<T, Alloc> Base;
	public:
		typedef T value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		typedef typename Base::allocator_type allocator_type;
		allocator_type get_allocator() const { return Base::get_allocator(); }

	public:
		typedef Base::iterator iterator;
		typedef Base::const_iterator const_iterator;

		typedef reverse_iterator<iterator> reverse_iterator;
		typedef reverse_iterator<const_iterator> const_reverse_iterator;
	protected:
		typedef pointer* map_pointer;
		static size_t _S_buffer_size() { return __deque_buf_size(sizeof(T)); }

	public:
		iterator begin() { return m_start; }
		iterator end() { return m_finish; }
		const_iterator begin() const { return m_start; }
		cosnt_iterator end() const { return m_finish; }

		reverse_iterator rbegin() { return reverse_iterator(m_finish); }
		reverse_iterator rend() { return reverse_iterator(m_start); }
		const_reverse_iterator rbegin() const { return const_reverse_iterator(m_finish); }
		const_reverse_iterator rend() const { return const_reverse_iterator(m_start); }

		reference operator[](size_type n) { return m_start[difference_type(n)]; }
		const_reference operator[](size_type n) const { return m_start[difference_type(n)]; }

		reference front() { return *m_start; }
		reference back() {
			const_iterator temp = m_finish;
			return *(--temp);
		}
		const_reference front() const { return *m_start; }
		const_reference back() const {
			const_iterator temp = m_finish;
			return *(--temp);
		}

		size_type size() const { return m_finish - m_start; }
		size_type max_size() const { return size_type(-1); }
		bool empty() const { return m_finish == m_start; }

	public:
		explicit deque(const allocator_type& a = allocator_type()) :Base(a, 0) {}
		deque(const deque& x) :Base(x.get_allocator(), x.size()) {
			uninitialzed_copy(x.begin(), x.end(), m_start);
		}
		deque(size_type n, const value_type& value, const allocator_type& a = allocator_type())
			:Base(a, n) {
			m_fill_initialize(value);
		}
		explicit deque(size_type n) :Base(allocator_type(), n) {
			m_fill_initialize(value_type());
		}
		deque(const value_type* first, const value_type* last, const allocator_type& a = allocator_type())
			:Base(a, last - first) {
			uninitialized_copy(first, last, m_start);
		}
		deque(const_iterator first, const_iterator last, const allocator_type& a = allocator_type())
			:Base(a, last - first) {
			uninitialized_copy(first, last, m_start);
		}
		~deque() { destroy(m_start, m_finish); }

		deque& operator=(const deque& x) {
			const size_type len = size();
			if (&x == this) return *this;
			if (len >= x.size()) {//擦除多余元素
				erase(copy(x.begin(), x.end(), m_start), m_finish);
			}
			else {//插入不足元素
				const_iterator mid = x.begin() + difference_type(len);
				copy(x.begin(), mid, m_start());
				insert(m_finish, mid, x.end());
			}
			return *this;
		}

		void swap(deque& x) {
			std::swap(m_start, x.m_start);
			std::swap(m_finish, x.m_finish);
			std::swap(m_map, x.m_map);
			std::swap(m_map_size, x.m_map_size);
		}

	public:
		void assign(size_type n, const T& val) {
			m_fill_assign(n, val);
		}
		void m_fill_assign(size_type n, const T& val);

	public:
		void push_back(const value_type& t) {
			if (m_finish.m_cur != m_finish.m_last - 1) {
				construct(m_finish.m_cur, t);
				++m_finish.m_cur;
			}
			else
				m_push_back_aux(t);
		}

		void push_back() {
			if (m_finish.m_cur != m_finish.m_last - 1) {
				construct(m_finish.m_cur);
				++m_finish.m_cur;
			}
			else
				m_push_back_aux();
		}

		void push_front(const value_type& t) {
			if (m_start.m_cur != m_start.m_first) {
				construct(m_start.m_cur - 1, t);
				--m_start.m_cur;
			}
			else
				m_push_front_aux(t);
		}

		void push_front() {
			if (m_start.m_cur != m_start.m_first) {
				construct(m_start.m_cur - 1);
				--m_start.m_cur;
			}
			else
				m_push_front_aux();
		}

		void pop_back() {
			if (m_finish.m_cur != m_finish.m_first) {
				--m_finish.m_cur;
				destroy(m_finish.m_cur);
			}
			else {
				m_pop_back_aux();
			}
		}

		void pop_front() {
			if (m_start.m_cur != m_start.m_last - 1) {
				destroy(m_start.m_cur);
				++m_start.m_cur;
			}
			else
				m_pop_front_aux();
		}

	public:
		iterator insert(iterator position, const value_type& x) {
			if (position.m_cur == m_start.m_cur) {
				push_front(x);
				return m_start;
			}
			else if (position.m_cur == m_finish.m_cur) {
				push_back(x);
				iterator temp = end();
				--temp;
				return temp;
			}
			else//除了头和尾的情况
				return m_insert_aux(position, x);
		}
		iterator insert(iterator position) {
			return insert(position, value_type());
		}

		void insert(iterator pos, size_type n, const value_type& x) {
			m_fill_insert(pos, n, x);
		}
		void m_fill_insert(iterator pos, size_type n, const value_type& x);

		void insert(iterator pos, const value_type* first, const value_type* last);
		void insert(iterator pos, const_iterator first, const_iterator last);


		void resize(size_type new_size, const value_type& x) {
			const size_type len = size();
			if (len < new_size) {
				insert(m_finish, new_size - len, x);
			}
			else {
				erase(m_start + new_size, m_finish);
			}
		}
		void resize(size_type new_size) { resize(new_size, value_type()); }
	public:
		iterator erase(iterator pos) {
			iterator next = pos;
			++next;
			difference_type index = pos - m_start;
			if (size_type(index) < (this->size() >> 1)){//在前半部分
				copy_backward(m_start, pos, next);
				pop_front();
			}
			else {//在后半部分
				copy(next, m_finish, pos);
				pop_back();
			}
			return m_start + index;
		}
		iterator erase(iterator first, iterator last);
		void clear();

	protected:
		void m_fill_initialize(const value_type& value);

	protected:
		void m_push_back_aux(const value_type& x);
		void m_push_back_aux();
		void m_push_front_aux(const value_type& x);
		void m_push_front_aux();
		void m_pop_back_aux();
		void m_pop_front_aux();

	protected:
		iterator m_insert_aux(iterator pos, const value_type& x);
		iterator m_insert_aux(iterator pos);
		void m_insert_aux(iterator pos, size_type n, const value_type& x);
		void m_insert_aux(iterator pos, const value_type* first, const value_type* last, size_type n);
		void m_insert_aux(iterator pos, const_iterator first, const_iterator last, size_type n);

		iterator m_reserve_elements_at_front(size_type n) {
			size_type vacancies = m_start.m_cur - m_start.m_first;
			if (n > vacancies)
				m_new_elements_at_front(n - vacancies);
			return m_start - difference_type(n);
		}
		iterator m_reserve_elements_at_back(size_type n) {
			size_type vacancies = m_finish.m_last - m_finish.m_cur - 1;
			if (n > vacancies)
				m_new_elements_at_front(n - vacancies);
			return m_finish + difference_type(n);
		}

		void m_new_elements_at_front(size_type new_elements);
		void m_new_elements_at_back(size_type new_elements);

	protected:
		void m_reserve_map_at_back(size_type node_to_add = 1) {
			if (node_to_add > m_map_size - 1 - (m_finish.m_node - m_map);
				m_reallocate_map(node_to_add, false);
		}
		void m_reserve_map_at_front(size_type node_to_add = 1) {
			if (node_to_add > size_type(m_start.m_node - m_map)
				m_reallocate_map(node_to_add, true);
		}
		void m_reallocate_map(size_type node_to_add, bool add_at_front);
	};

	/*处理头尾插入，剩下交给m_insert_aux*/
	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_fill_insert(iterator pos, size_type n, const value_type& x) {
			if (pos.m_cur == m_start.m_cur) {
				iterator new_start = m_reserve_elements_at_front(n);
					try {
					uninitialized_fill(new_start, m_start, x);
					m_start = new_start;
				}
				catch (...) {
					m_destroy_nodes(new_start.m_node, m_start_.m_node);
				}
			}
			else if (pos.m_cur == m_finish.m_cur) {
				iterator new_finish = m_reserve_elements_at_back(n);
				try {
					uninitialized_fill(m_finish, new_finish, x);
					m_finish = new_finish;
				}
				catch (...) {
					m_destroy_node(m_finish.m_node + 1, new_finish.m_node + 1);
				}
			}
			else
				m_insert_aux(pos, n, x);
		}

	/*处理头尾插入，剩下交给m_insert_aux*/
	template<typename T, typename Alloc>
	void deque<T, Alloc>::insert(iterator position, const value_type* first, const value_type* last) {
			size_type n = last - first;
			if (pos.m_cur == m_start.m_cur) {
				iterator new_start = m_reserve_elements_at_front(n);
				try {
					uninitialized_copy(first, last, new_start);
					m_start = new_start;
				}
				catch (...) {
					m_destroy_nodes(new_start.m_node, m_start_.m_node);
				}
			}
			else if (pos.m_cur == m_finish.m_cur) {
				iterator new_finish = m_reserve_elements_at_back(n);
				try {
					uninitialized_copy(first, last, m_finish);
					m_finish = new_finish;
				}
				catch (...) {
					m_destroy_node(m_finish.m_node + 1, new_finish.m_node + 1);
				}
			}
			else
				m_insert_aux(pos, first, last, n);
		}

	template<typename T, typename Alloc>
	void deque<T, Alloc>::insert(iterator pos, const_iterator first, const_iterator last) {
			size_type n = last - first;
			if (pos.m_cur == m_start.m_cur) {
				iterator new_start = m_reserve_elements_at_front(n);
				try {
					uninitialized_copy(first, last, new_start);
					m_start = new_start;
				}
				catch (...) {
					m_destroy_nodes(new_start.m_node, m_start_.m_node);
				}
			}
			else if (pos.m_cur == m_finish.m_cur) {
				iterator new_finish = m_reserve_elements_at_back(n);
				try {
					uninitialized_copy(first, last, m_finish);
					m_finish = new_finish;
				}
				catch (...) {
					m_destroy_node(m_finish.m_node + 1, new_finish.m_node + 1);
				}
			}
			else
				m_insert_aux(pos, first, last, n);
		}

	template<typename T, typename Alloc>
	typename deque<T, Alloc>::iterator deque<T, Alloc>::erase(iterator first, iterator last) {
			if (first == m_start && last == m_finish) {
				clear();
				return m_finish;
			}
			else {
				difference_type n = last - first;
				difference_type elems_before = first - m_start;
				if (elems_before < difference_type((this->size() - n) / 2)) {//处理前半部分的情况，同时将空闲的node释放
					copy_backward(m_start, first, last);
					iterator new_start = m_start + n;
					destroy(m_start, new_start);
					m_destroy_node(new_start.m_node, m_start.m_node);
					m_start = new_start;
				}
				else {
					copy(last, m_finish, first);
					iterator new_finish = m_finish - n;
					destroy(new_finish, m_finish);
					m_destroy_nodes(new_finish.m_node + 1, m_finish.m_node + 1);
					m_finish = new_finish;
				}
				return m_start + elems_before;
			}
		}

	template<typename T, typename Alloc>
	void deque<T, Alloc>::clear() {
			for (map_pointer node = m_start.m_node; node < m_finish.m_node; ++node) {//处理整段内存
				destroy(*node, *node + _S_buffer_size());
				m_deallocate_node(*node);
			}
			if (m_start.m_node != m_finish.m_node) {//处理两端的零碎的内存
				destroy(m_start.m_cur, m_start.m_last);
				destroy(m_finish.m_first, m_finish.m_cur);
				m_deallocate_node(m_finish.m_first);
			}
			else//处理整个不到一个node的情况
				destroy(m_start.m_cur, m_finish.m_cur);
			m_finish = m_start;
		}

	//将当前元素改为value，不改变已有内存
	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_fill_initialize(const value_type& value) {
			map_pointer cur;
			try {
				for (cur = m_start.m_node; cur < m_finish.m_node; ++cur)
					uninitialized_fill(*cur, *cur + _S_buffer_size(), value);
				uninitialized_fill(m_finish.m_first, m_finish.m_cur, value);
			}
			catch (...) {
				destroy(m_start, iteator(*cur, cur));
			}
		}

	/*处理需要申请新内存的情况*/
	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_push_back_aux(const value_type& t) {
			value_type t_copy = t;
			m_reserve_map_at_back();
			*(m_finish.m_node + 1) = m_allocate_node();
			try {
				construct(m_finish.m_cur, t_copy);
				m_finish.m_set_node(m_finish.m_node + 1);
				m_finish.m_cur = m_finish.m_first;
			}
			catch (...) {
				m_deallocate_node(*(m_finish.m_node + 1));
			}
		}

	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_push_back_aux() {
			m_reserve_map_at_back();
			*(m_finish.m_node + 1) = m_allocate_node();
			try {
				construct(m_finish.m_cur);
				m_finish.m_set_node(m_finish.m_node + 1);
				m_finish.m_cur = m_finish.m_first;
			}
			catch (...) {
				m_deallocate_node(*(m_finish.m_node + 1));
			}
		}

	/*处理需要申请新内存的情况*/
	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_push_front_aux(const value_type& t) {
			value_type t_copy = t;
			m_reserve_map_at_front();
			*(m_start.m_node - 1) = m_allocate_node();
			try {
				m_start.m_set_node(m_start.m_node - 1);
				m_start.m_cur = m_start.m_last - 1;
				construct(m_start.m_cur, t_copy);
			}
			catch (...) {
				++m_start;
				m_deallocate_node(*(m_start.m_node - 1));
			}
		}

	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_push_front_aux() {
			m_reserve_map_at_front();
			*(m_start.m_node - 1) = m_allocate_node();
			try {
				m_start.m_set_node(m_start.m_node - 1);
				m_start.m_cur = m_start.m_last - 1;
				construct(m_start.m_cur);
			}
			catch (...) {
				++m_start;
				m_deallocate_node(*(m_start.m_node - 1));
			}
		}

	/*释放多余的内存*/
	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_pop_back_aux() {
		m_deallocate_node(m_finish.m_first);
		m_finish.m_set_node(m_finish.m_node - 1);
		m_finish.m_cur = m_finish.m_last - 1;
		destroy(m_finish.m_cur);
	}

	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_pop_front_aux() {
		destroy(m_start.m_cur);
		m_deallocate_node(m_start.m_first);
		m_start.m_set_node(m_start.m_node + 1);
		m_start.m_cur = m_start.m_first;
	}

	template<typename T, typename Alloc>
	typename deque<T, Alloc>::iterator
		deque<T, Alloc>::m_insert_aux(iterator pos, const value_type& x) {
			difference_type index = pos - m_start;
			value_type x_copy = x;
			if (size_type(index) < this->size() / 2) {
				push_front(front());
				iterator front1 = m_start;
				++front1;
				iterator front2 = front1;
				++front2;
				pos = m_start + index;
				iterator pos1 = pos;
				++pos1;
				copy(front2, pos1, front1);
			}
			else {
				push_back(back());
				iterator back1 = m_finish;
				--back1;
				iterator back2 = back1;
				--back2;
				pos = index + m_start;
				copy_backward(pos, back2, back1);
			}
			*pos = x_copy;
			return pos;
		}

	template<typename T, typename Alloc>
	typename deque<T, Alloc>::iterator
		deque<T, Alloc>::m_insert_aux(iterator pos) {
			difference_type index = pos - m_start;
			if (size_type(index) < this->size() / 2) {
				push_front(front());
				iterator front1 = m_start;
				++front1;
				iterator front2 = front1;
				++front2;
				pos = m_start + index;
				iterator pos1 = pos;
				++pos1;
				copy(front2, pos1, front1);
			}
			else {
				push_back(back());
				iterator back1 = m_finish;
				--back1;
				iterator back2 = back1;
				--back2;
				pos = index + m_start;
				copy_backward(pos, back2, back1);
			}
			*pos = value_type();
			return pos;
		}

	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_insert_aux(iterator pos, size_type n, const value_type& x) {
			const difference_type elems_before = pos - m_start;
			size_type length = this->size();
			value_type x_copy = x;
			if (elems_before < difference_type(length / 2)) {
				iterator new_start = m_reserve_elements_at_front(n);
				iterator old_start = m_start;
				pos = m_start + elems_before;
				try {
					if (elems_before >= difference(n)) {
						iterator start_n = m_start + differnce_type(n);
						uninitialized_copy(m_start, start_n, new_start);
						m_start = new_start;
						copy(start_n, pos, ols_start);
						fill(pos - differene_type(n), pos, x_copy);
					}
					else {
						iterator mid = uninitialized_copy(m_start, pos, new_start);
						try {
							uninitialized_fill(mid, m_start, x_copy);
						}
						catch (...) {
							destroy(new_start, mid);
						}
						m_start = new_start;
						fill(old_start, pos, x_copy);
					}
				}
				catch (...) {
					m_destroy_nodes(new_start.m_node, m_start.m_node);
				}
			}
			else {
				iterator new_finish = m_reserve_elements_at_back(n);
				iterator old_finish = m_finish;
				const difference_type elems_after = difference_type(length) - elems_before;
				pos = m_finish - elems_after;
				try {
					if (elems_after > difference_type(n)) {
						iterator finish_n = m_finish - difference_type(n);
						uninitialzed_copy(finish_n, m_finish, m_finish);
						m_finish = new_finish;
						copy_backward(pos, finish_n, old_finish);
						fill(pos, pos + difference_type(n), x_copy);
					}
					else {
						uninitialized_fill(m_finish, pos + difference_type(n), x_copy);
						try {
							return uninitialized_copy(pos, m_finish, pos + difference_type(n));
						}
						catch (...) {
							destroy(m_finish, pos + difference_type(n));
						}
					}
				}
				catch (...) {
					m_destroy_nodes(m_finish.m_node + 1, new_finish.m_node + 1);
				}

			}
		}

	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_insert_aux(iterator pos, const value_type* first, const value_type* last, size_type n) {
			const difference_type elems_before = pos - m_start;
			size_type length = this->size();
			if (elems_before < difference_type(length / 2)) {
				iterator new_start = m_reserve_elements_at_front(n);
				iterator old_start = m_start;
				pos = m_start + elems_before;
				try {
					if (elems_before >= difference(n)) {
						iterator start_n = m_start + differnce_type(n);
						uninitialized_copy(m_start, start_n, new_start);
						m_start = new_start;
						copy(start_n, pos, ols_start);
						fill(first, last, pos - difference_type(n));
					}
					else {
						const value_type* mid1 = first + (differene_type(n) - elems_before);
						iterator mid2 = uninitialized_copy(m_start, pos, new_start);
						try {
							uninitialized_copy(first, mid1, mid2);
						}
						catch (...) {
							destroy(new_start, mid2);
						}
						m_start = new_start;
						copy(mid1, last, old_start);
					}
				}
				catch (...) {
					m_destroy_nodes(new_start.m_node, m_start.m_node);
				}
			}
			else {
				iterator new_finish = m_reserve_elements_at_back(n);
				iterator old_finish = m_finish;
				const difference_type elems_after = difference_type(length) - elems_before;
				pos = m_finish - elems_after;
				try {
					if (elems_after > difference_type(n)) {
						iterator finish_n = m_finish - difference_type(n);
						uninitialzed_copy(finish_n, m_finish, m_finish);
						m_finish = new_finish;
						copy_backward(pos, finish_n, old_finish);
						copy(first, last, pos);
					}
					else {
						const value_type* mid = first + elems_after;
						__uninitialized_copy_copy(mid, last, pos, m_finish, m_finish);
						M_finish = new_finish;
						copy(first, mid, pos);
					}
				}
				catch (...) {
					m_destroy_nodes(m_finish.m_node + 1, new_finish.m_node + 1);
				}

			}
		}

	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_insert_aux(iterator pos, const_iterator first, const_iterator last, size_type n) {
			const difference_type elems_before = pos - m_start;
			size_type length = this->size();
			if (elems_before < difference_type(length / 2)) {
				iterator new_start = m_reserve_elements_at_front(n);
				iterator old_start = m_start;
				pos = m_start + elems_before;
				try {
					if (elems_before >= difference(n)) {
						iterator start_n = m_start + differnce_type(n);
						uninitialized_copy(m_start, start_n, new_start);
						m_start = new_start;
						copy(start_n, pos, ols_start);
						fill(first, last, pos - difference_type(n));
					}
					else {
						const value_type* mid1 = first + (differene_type(n) - elems_before);
						iterator mid2 = uninitialized_copy(m_start, pos, new_start);
						try {
							uninitialized_copy(first, mid1, mid2);
						}
						catch (...) {
							destroy(new_start, mid2);
						}
						m_start = new_start;
						copy(mid1, last, old_start);
					}
				}
				catch (...) {
					m_destroy_nodes(new_start.m_node, m_start.m_node);
				}
			}
			else {
				iterator new_finish = m_reserve_elements_at_back(n);
				iterator old_finish = m_finish;
				const difference_type elems_after = difference_type(length) - elems_before;
				pos = m_finish - elems_after;
				try {
					if (elems_after > difference_type(n)) {
						iterator finish_n = m_finish - difference_type(n);
						uninitialzed_copy(finish_n, m_finish, m_finish);
						m_finish = new_finish;
						copy_backward(pos, finish_n, old_finish);
						copy(first, last, pos);
					}
					else {
						const value_type* mid = first + elems_after;
						__uninitialized_copy_copy(mid, last, pos, m_finish, m_finish);
						M_finish = new_finish;
						copy(first, mid, pos);
					}
				}
				catch (...) {
					m_destroy_nodes(m_finish.m_node + 1, new_finish.m_node + 1);
				}

			}
		}

	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_new_elements_at_front(size_type new_elems) {
			size_type new_node = (new_elems + _S_buffer_size() - 1) / _S_buffer_size();
			m_reserve_map_at_front(new_node);
			size_type i;
			try {
				for (i = 1; i < new_node; ++i) {
					*(m_start.m_node - i) = m_allocate_node();
				}
			}
			catch (...) {
				for (size_type j = 1; j < i; ++j) {
					m_deallocate_node(*(m_start.m_node - j));
				}
			}
		}

	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_new_elements_at_back(size_type new_elems) {
			size_type new_node = (new_elems + _S_buffer_size() - 1) / _S_buffer_size();
			m_reserve_map_at_back(new_node);
			size_type i;
			try {
				for (i = 1; i < new_node; ++i) {
					*(m_finish.m_node + i) = m_allocate_node();
				}
			}
			catch (...) {
				for (size_type j = 1; j < i; ++j) {
					m_deallocate_node(*(m_finish.m_node + j));
				}
			}
		}

	template<typename T, typename Alloc>
	void deque<T, Alloc>::m_reallocate_map(size_type node_to_add, bool add_at_front) {
			size_type old_num_nodes = m_finish.m_node - m_start.m_node + 1;
			size_type new_num_nodes = old_num_nodes + node_to_add;
			map_pointer new_node_start;
			if (m_map_size > 2 * new_num_nodes) {//不需要申请新的map
				new_node_start = m_map + (m_map_size - new_num_nodes) / 2 +//若add_at_front，那么new_start后移留空间
					(add_at_front ? node_to_add : 0);
				if (new_node_start < m_start.m_node)
					copy(m_start.m_node, m_finish.m_node + 1, new_node_start);
				else
					copy_backward(m_start.m_node, m_finish.m_node + 1, new_start + old_num_nodes);
			}
			else {
				size_type new_map_size = m_map_size + max(m_map_size, node_to_add) + 2;
				map_pointer new_map = m_allocate_map(new_map_size);		//申请新的map
				new_node_start = new_map + (new_map_size - new_num_nodes) / 2
					+ (add_at_front ? node_to_add : 0);
				copy(m_start.m_node, m_finish.m_node + 1, new_node_start);
				m_deallocate_map(m_map, m_map_size);
				m_map = new_map;
				m_map_size = new_map_size;
			}
			m_start.m_set_node(new_node_start);
			m_finish.m_set_node(new_node_start + old_num_nodes - 1);
		}


	template <class T, class Alloc>
	inline bool operator==(const deque<T, Alloc>& x,
		const deque<T, Alloc>& y) {
			return x.size() == y.size() &&
				equal(x.begin(), x.end(), y.begin());
		}

	template <class T, class Alloc>
	inline bool operator<(const deque<T, Alloc>& x,
		const deque<T, Alloc>& y) {
		return lexicographical_compare(x.begin(), x.end(),
			y.begin(), y.end());
	}

}
#endif
