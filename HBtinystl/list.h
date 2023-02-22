#ifndef _HBTINYSTL_LIST_H_
#define _HBTINYSTL_LIST_H_

#include<iterator.h>
#include<memory.h>
namespace HBtinySTL {
	template<typename T>
	class list_node {
	private:
		list_node* m_prev;
		list_node* m_next;
		T m_data;
	};

	/*list�ĵ���������ʹ����ͨ��ָ�룬listռ�ݵ��ڴ�ռ䲢��������������Ҫ����*,->,++,--�������ȷ���书��*/
	template<typename T, typename Ref, typename Ptr>
	class list_iterator {
	private:
		typedef size_t						size_type;
		typedef ptrdiff_t					difference_type;
		typedef bidirectional_iterator_tag	iterator_category;
		typedef T							value_type;
		typedef Ptr							pointer;
		typedef Ref							reference;

		typedef list_node<T>							Node;
		typedef list_iterator<T, Ref, Ptr>				self;
		typedef list_iterator<T, T&, T*>				iterator;
		typedef list_iteartor<T, const T&, const T*>	const_iterator;

		Node* m_node;		//ָ��iteratorʵ�ʰ����Ľڵ�
		void m_incr() { m_node = m_node->m_next; }
		void m_decr() { m_node = m_node->m_prev; }

		list_iterator(Node* x) : m_node(x) {}
		list_iterator() {}
		list_iterator(const iterator& x) : m_node(x.m_node) {}

		bool operator==(const list_iterator& x) {
			return m_node == x.m_node;
		}
		bool operator!=(const list_iterator& x) {
			return m_node != x.m_node;
		}
		reference operator*() const { return m_node->m_data; }
		pointer operator->()const { return &(operator*()); }
		self& operator++() {
			this->m_incr();
			return *this;
		}
		self& operator++(int) {
			self temp = *this;
			this->m_incr();
			return temp;
		}
		self& operator--() {
			this->m_decr();
			return *this;
		}
		self& operator--(int) {
			self temp = *this;
			this->m_decr();
			return temp;
		}
	};

	template<typename T, typename Alloc>
	class list_base {
	protected:
		list_node<T>* m_node;	//��β���ڵ�֮���һ���հ׽ڵ㣬����ʵ������

	protected:
		typedef simple_alloc<list_node<T>, Alloc> Alloc_type;
		list_node* m_get_node() { return Alloc_type::allocate(1); }//����һ���ڵ���ڴ�
		void m_put_node(list_node* p) { Alloc_type::deallocate(p, 1); }//�ͷ�һ���ڵ���ڴ�
	public:
		typedef Alloc allocator_type;
		allocator_type get_allocator() { return allocator_type(); }

		list_base(const allocator_type&) {
			m_node = m_get_node();
			m_node->m_next = m_node;
			m_node->m_prev = m_node;
		}
		~list_base() {
			clear();
			m_put_node(m_node);
		}
		/*�������ͷų�����֮������нڵ�*/
		void clear() {
			list_node<T>* cur = m_node->next;
			while (cur != m_node) {
				list_node<T>* temp = cur;
				cur = cur->m_next;
				destroy(&(temp->m_data));
				m_put_node(temp);
			}
			m_node->m_next = m_node;
			m_node->m_prev = m_node;
		}
	};

	template<typename T, typename Alloc = alloc>
	class list :protected list_base<T, Alloc> {
	protected:
		typedef void* void_pointer;

	public:
		typedef T					value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef size_t				size_type;
		typedef ptrdiff_t			difference_type;

		typedef list_node<T>		node;
		typedef list_base<T, Alloc>	Base;
		typedef typename Base::allocator_type allocator_type;
		allocator_type get_allocator() { return Base::get_allocator(); }

		typedef list_iterator<T, T&, T*>				iterator;
		typedef lsit_iterator<T, const T&, const T*>	const_iterator;
		typedef reverse_iterator<iterator>				reverse_iterator;
		typedef reverse_iterator<const_iterator>		const_reverse_itearor;

	protected:
		/*����m_get_node�����ڴ沢��constuct*/
		node* m_create_node(const T& x) {
			node* p = m_get_node();
			try {
				construct(&p->m_data, x);
			}
			catch (...) {
				m_put_node(p);
			}
			return p;
		}
		node* m_create_node() {
			node* p = m_get_node();
			try {
				construct(&p->m_data);
			}
			catch (...) {
				m_put_node(p);
			}
			return p;
		}

	public:
		explicit list(const allocator_type& a = allocator_type()) : Base(a) {}


		iterator begin() { return m_node->m_next; }
		const_iterator begin() const { return m_node->m_next; }
		iterator end() { return m_node; }//m_node��ָ��β�ڵ�֮��Ŀհ׽ڵ㣬ֱ�ӷ���m_node����
		const_iterator end() const { return m_node; }

		reverse_iterator rbegin() {
			return reverse_iterator(end());
		}
		const_reverse_iterator rbegin() {
			return const_reverse_iterator(end());
		}
		reverse_iterator rend() {
			return reverse_iterator(begin());
		}
		const_reverse_iterator rend() {
			return const_reverse_iterator(begin());
		}
		bool empty()const { return m_node->m_next == m_node; }

		size_type size() const {
			size_type result = 0;
			distance(begin(), end(), result);
			return result;
		}

		size_type max_size() const { return size_type(-1); }

		reference front() { return *begin(); }
		const_reference front()const { return *begin(); }
		reference back() { return *(--end()); }//end()������m_node��ʼ������ʱ���������������Լ�����Ӱ��list�е�����
		const_reference back() const { return *(--end()); }

		void swap(list<T, Alloc>& x) { std::swap(m_node, x.m_node); }//��vector��������ͬ�����������ݣ�������ַ

		/*��position֮ǰ���룬��x��ʼ���������½ڵ��ַ*/
		iterator insert(iterator position, const T& x) {
			node* temp = m_create_node(x);
			temp->m_next = position.m_node;
			temp->m_prev = position.m_prev;
			position.m_node->m_prev->next = temp;
			position.m_node->m_prev = temp;
			return temp;
		}
		iterator insert(iterator position) { return insert(position, T()); }

		void insert(iterator position, const T* first, const T* last) {
			for (; first != last; ++first) {
				insert(position, *first);
			}
		}
		void insert(iterator position, iterator first, iterator last) {
			for (; first != last; ++first) {
				insert(position, *first);
			}
		}
		void insert(iterator position, size_type n, const T& x) {
			m_fill_insert(position, n, x);
		}
		void m_fill_insert(iterator position, size_type n, const T& x) {
			for (; n > 0; --n) {
				insert(position, x);
			}
		}

		void push_front(const T& x) { insert(begin(), x); }
		void push_front() { insert(begin()); }

		void push_back(const T& x) { insert(end(), x); }
		void push_back() { insert(end()); }

		/*�Ƴ�position�ϵ�Ԫ�أ��ͷ��ڴ棬���ز������next�ڵ������*/
		iterator erase(iterator position) {
			list_node* next_node = position.m_node->m_next;
			list_node* prev_node = position.m_node->m_prev;
			node* n = position.m_node;
			next_node->m_prev = prev_node;
			prev_node->m_next = next_node;
			destroy(&(n->m_data));
			m_put_node(n);
			return iterator(next_node);
		}
		iterator erase(iterator first, iterator last) {
			for (; fisrt != last; ++first)
				erase(first);
			return last;
		}

		void clear() { Base::clear(); }

		/*�ı�����Ĵ�СΪnew_size��x���Ը���Ԫ�ظ�ֵ*/
		void resize(size_type new_size, const T& x) {
			iterator temp = begin();
			size_type len = 0;
			for (; temp != end() && len < new_size; ++len, ++temp)//�ҵ�new_size��Ӧ�Ľڵ㣬ɾ������ڵ����������ڵ�
				;
			if (len == new_size)
				erase(temp, end());
			else
				insert(end(), new_size - len, x);
		}
		void resize(size_type new_size) { this->resize(new_size, T()); }

		void pop_front() { erase(begin()); }
		void pop_back() { iterator temp = end(); erase(--temp); }

		list(size_type n, const T& value, const allocator_type& a = allocator_type()) :Base(a) {
			insert(begin(), n, value);
		}
		explicit list(size_type n) :Base(allocator_type()) { insert(begin(), n, T())�� }

		list(const T* first, const T* last, const allocator_type& a = allocator_type())
			:Base(a) {
			this->insert(begin(), first, last);
		}
		list(const_iterator first, const_iterator last, const allocator_type& a = allocator_type())
			:Base(a) {
			this->insert(begin(), first, last);
		}
		list(list<T, Alloc>& x) :Base(x.get_allocator()) {
			insert(begin(), x.begin(), x.end());
		}

		~list() {}

		list<T, Alloc>& operator=(const list<T, Alloc>& x);

	public:
		/*ָ��������Ϊn������Ϊval*/
		void assign(size_type n, const T& val) { m_fill_assign(n, val); }
		void m_fill_assign(size_type n, const T& val);

	protected:
		/*��һ��list��[first, last)��Ԫ���ƶ���position֮ǰ*/
		void transfer(iterator position, iterator first, iterator last) {
			if (position != last) {
				//��[first, last)�Ӿ�list�а���
				last.m_node->m_prev->m_next = position.m_node;
				first.m_node->m_prev->m_next = last.m_node;
				position.m_node->m_prev->m_next = first.m_node;
				//�����µ�list
				list_node* temp = position.m_node->m_prev;
				position.m_node->m_prev = last.m_node->m_prev;
				last.m_node->m_prev = first.m_node->m_prev;
				first.m_node->m_prev = temp;
			}
		}
	public:
		/*���������е�Ԫ���ƶ���position֮ǰ*/
		void splice(iterator position, list& x) {
			if (!x.empty()) {
				this->transfer(position, x.begin(), x.end());
			}
		}
		/*��i��Ԫ���ƶ���position֮ǰ*/
		void splice(iterator position, list&, iterator i) {
			iterator j = i;
			++j;
			if (position == i || position == j) return;
			this->transfer(position, i, j);
		}
		/*��[first, last)�е�Ԫ���ƶ���position֮ǰ*/
		void splice(iteator position, list&, iterator first, iterator last) {
			if (first != last) {
				this->transfer(position, first, last);
			}
		}


		/*���������ֵΪvalue�Ľڵ�*/
		void remove(const T& value) {
			iterator first = begin(), last = end();
			while (first != last) {
				iterator next = ++first;
				if (*first == value)
					erase(first);
				first = next;
			}
		}

		/*��������е��ظ�����Ԫ�أ�������һ��*/
		void unique() {
			if (begin() == end()) return;
			iterator first = begin(), last = end();
			iterator next = first;
			while (++next != last) {
				if (*first == *next) {
					erase(next);
				}
				else {
					first = next;
				}
				next = first;
			}
		}

		/*�鲢���������б�Ϊһ�������б�*/
		void merge(list& x) {
			iterator first1 = begin(), first2 = x.begin();
			iterator last1 = end(), last2 = x.end();
			while (first1 != last1 && first2 != last2) {
				if (*first2 < *first1) {
					iterator next = first2;
					transfer(first1, first2, ++next);
				}
				else {
					++first;
				}
			}
			if (first2 != last2)
				transfer(first1, first2, last2);
		}

		/*��ת����Ϊ����*/
		void reverse() {
			list_node* temp = m_node;
			do {
				std::swap(temp->m_next, temp->m_prev);
				temp = temp->m_prev;
			} while (temp != m_node);
		}


		void sort() {
			if (m_node->m_next == m_node || m_node->m_next->m_next == m_node)//����0��1���ڵ�
				return;
			list<T, Alloc> carry;
			list<T, Alloc> counter[64];
			int fill = 0;
			while (!empty()) {
				carry.splice(carry.begin(), *this, begin());//�㷨˵���ɼ�https://blog.csdn.net/qq_31720329/article/details/85535787
				int i = 0;
				while (i < fill && !counter[i].empty()) {
					counter[i].merge(carry);
					carry.swap(counter[i++]);
				}
				carry.swap(counter[i]);
				if (i == fill) ++fill;
			}
			for (int i = 1; i < fill; ++i) {
				counter[i].merge(counter[i - 1]);
			}
			swap(counter[fill - 1]);
		}
	};

	template<typename T, typename Alloc>
	inline bool operator==(const list<T, Alloc>& x, const list<T, Alloc>& y) {
		typedef typename list<T, Alloc>::const_iterator const_iterator;
		const_iterator f1 = x.begin(), f2 = y.begin(), l1 = x.end(), l2 = y.end();
		while (f1 != l1 && f2 != l2 && *f1 == *f2) {
			++f1;
			++f2;
		}
		return f1 == l1 && f2 == l2;
	}

	template<typename T, typename Alloc>
	inline bool operator<(const list<T, Alloc>& x, const list<T, Alloc>& y) {
		return lexicographical_compare(x.begin(), x.end(),
			y.begin(), y.end());
	}

	template<typename T, typename Alloc>
	list<T, Alloc>::operator=(const list<T, Alloc>& x) {
		if (&x == this) return *this;
		iterator first1 = begin(), last1 = end();
		const_iterator first2 = x.begin(), last2 = x.end();
		while (first1 != last1 && first2 != last2)
			*first1++ = *first2++;
		if (first2 == last2)//��x�϶̣����������Ԫ��
			erase(first1, last1);
		else//��x�ϳ�����Ϊlist1�����½ڵ�
			insert(last1, first2, last2);
		return *this;
	}

	/*ָ��������Ϊn������������val���*/
	template<typename T, typename Alloc>
	void list<T, Alloc>::m_fill_assign(size_type n, const T& val) {
		iterator temp = begin();
		for (; n > 0 && temp != end(); ++temp, --n) {//�ҵ�n��Ӧ�Ľڵ�򵽴������β����Ȼ���������ڵ����벻��ڵ�
			*temp = val;
		}
		if (n > 0) {
			insert(end(), n, val);
		}
		else {
			erase(temp, end());
		}
	}








}

#endif



