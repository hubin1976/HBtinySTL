#ifndef _HBTINYSTL_RBTREE_H_
#define _HBTINYSTL_RBTREE_H_
#include<memory.h>
#include<iterator.h>

namespace HBtinySTL {
	typedef bool rb_tree_color_type;
	const rb_tree_color_type rb_tree_red = false;
	const rb_tree_color_type rb_tree_black = true;

	class rb_tree_node_base {
		typedef rb_tree_color_type color_type;
		typedef rb_tree_color_type* base_ptr;

		color_type m_color;
		base_ptr m_parent;
		base_ptr m_left;
		base_ptr m_right;

		static base_ptr minimun(base_ptr x) {//当前节点为根节点的子树的最小值
			while (x->m_left != 0)
				x = x->left;
			return x;
		}
		static base_ptr maximun(base_ptr x) {//当前节点为根节点的子树的最大值
			while (x->m_right != 0)
				x = x->right;
			return x;
		}
	};
	template<typename Value>
	class rb_tree_node : public rb_tree_node_base {
		typedef rb_tree_node<Value>* link_type;
		Value m_value;
	};

	class rb_tree_iterator_base {
		typedef rb_tree_node_base::base_ptr base_ptr;
		typedef bidirectional_iterator_tag iterator_category;
		typedef ptrdiff_t difference_type;
		base_ptr m_node;

		/*找到后继节点*/
		void m_increment() {
			if (m_node->m_color == rb_tree_red && m_node->m_parent->m_parent == m_node)//仅有一个节点
				m_node = m_node->m->right;
			else if (m_node->m_right != 0) {//右子树的最左节点
				m_node = m_node->m_right;
				while (m_node->m_left != 0)
					m_node = m_node->m_left;
			}
			else {
				base_ptr y = m_node->m_parent;//若无右子树则找到到根节点路径上最先为左子节点的父节点的parent节点
				while (m_node == y->m_right) {
					m_node = y;
					y = y->m_parent;
				}
				if (m_node->m_right != y)
					m_node = y;
			}
		}
		/*找到前驱节点*/
		void m_decrement() {
			if (m_node->m_color == rb_tree_red && m_node->m_parent->m_parent == m_node)//仅有一个节点
				m_node = m_node->m->right;
			else if (m_node->m_left != 0) {//左子树的最右节点
				base_ptr y = m_node->m_left;
				while (y->m_right != 0)
					y = y->m_right;
				m_node = y;
			}
			else {
				base_ptr y = m_node->m_parent;//如上
				while (m_node == y->m_left) {
					m_node = y;
					y = y->m_parent;
				}
				m_node = y;
			}
		}

	};

	template <typename Value, typename Ref, typename Ptr>
	class rb_tree_iterator :public rb_tree_iterator_base {
		typedef Value value_type;
		typedef Ref reference;
		typedef Ptr pointer;
		typedef rb_tree_iterator<Value, Value&, Value*> iterator;
		typedef rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
		typedef rb_tree_iterator<Value, Ref, Ptr> self;
		typedef rb_tree_node<Value>* link_type;

		rb_tree_iterator() {}
		rb_tree_iterator(link_type x) { m_node = x; }
		rb_tree_iterator(const iterator& it) { m_node = it.m_node; }

		reference operator*() const { return link_type(m_node)->m_value; }

		self& operator++() { m_increment(); return *this; }
		self& operator++(int) { self temp = *this;  m_increment(); return temp; }

		self& operator--() { m_decrement(); return *this; }
		self& operator--(int) { self temp = *this;  m_decrement(); return temp; }

	};

	inline bool operator==(const rb_tree_base_iterator& x, const rb_tree_base_iterator& y) {
		return x.m_node == y.m_node;
	}
	inline bool operator!=(const rb_tree_base_iterator& x, const rb_tree_base_iterator& y) {
		return x.m_node != y.m_node;
	}

	/*左旋，将x的右子节点提上来*/
	inline void rb_tree_rotate_left(rb_tree_node_base* x, rb_tree_node_base*& root) {
		rb_tree_node_base* y = x->m_right;
		x->m_right = y->m_left;
		if (y->m_left)
			y->m_left->m_parent = x;
		y->m_parent = x->m_parent;
		if (root == x)
			root = y;
		else if (x == x->m_parent->m_left)
			x->m_parent->m_left = y;
		else
			x->m_parent->m_right = y;
		y->m_left = x;
		x->parent = y;
	}
	/*右旋，将x的左子节点提上来*/
	inline void rb_tree_rotate_right(rb_tree_node_base* x, rb_tree_node_base*& root) {
		rb_tree_node_base* y = x->m_left;
		x->m_left = y->m_right;
		if (y->m_right)
			y->m_right->m_parent = x;
		y->m_parent = x->m_parent;
		if (root == x)
			root = y;
		else if (x == x->m_parent->m_left)
			x->m_parent->m_left = y;
		else
			x->m_parent->m_right = y;
		y->m_right = x;
		x->parent = y;
	}

	inline void rb_tree_rebalance(rb_tree_node_base* x, rb_tree_node_base*& root) {
		x->m_color = rb_tree_red;
		while (x != root && x->m_parent->m_color == rb_tree_red) {//若x为红且父节点为黑则插入x不影响
			if (x->m_parent == x->m_parent->m_parent->m_left) {//x的父节点是左子节点
				rb_tree_node_base* y = x->m_parent->m_parent->m_right;//y为伯父节点
				if (y && y->m_color == rb_tree_red) {//若父节点及伯父节点为红，则将他们改为黑，将爷爷节点改为红，继续向上调整
					x->m_parent->m_color = rb_tree_black;
					y->m_color = rb_tree_black;
					x->m_parent->m_parent->m_color = rb_tree_red;
					x = x->m_parent->m_parent;
				}
				else {//
					if (x == x->m_parent->m_right) {//若为‘左右’，则需要先进行一次左旋
						x = x->m_parent;
						rb_tree_rotate_left(x, root);//左旋之后，x依然为最底层的子节点
					}
					x->m_parent->m_color = rb_tree_black;//此时的情况为父节点为红，爷爷节点为黑，且伯父节点不存在
					x->m_parent->m_parent->color = rb_tree_red;//令父节点为黑，爷节点为红
					rb_tree_rotate_right(x->m_parent->m_parent, root);//右旋后，则父节点为黑，爷节点变为伯父节点，颜色平衡
				}
			}
			else {//与上一种情况对称
				rb_tree_node_base* y = x->m_parent->m_parent->m_left;
				if (y && y->m_color == rb_tree_red) {
					x->m_parent->m_color = rb_tree_black;
					y->m_color = rb_tree_black;
					x->m_parent->m_parent->m_color = rb_tree_red;
					x = x->m_parent->m_parent;
				}
				else {
					if (x == x->m_parent->m_left) {
						x = x->m_parent;
						rb_tree_rotate_right(x, root);
					}
					x->m_parent->m_color = rb_tree_black;
					x->m_parent->m_parent->color = rb_tree_red;
					rb_tree_rotate_left(x->m_parent->m_parent, root);
				}
			}
		}
		root->m_color = rb_tree_black;//防止回溯到 x==root 根节点变为红色
	}
	inline rb_tree_node_base* rb_tree_rebalance_for_erase(rb_tree_node_base* z, rb_tree_node_base* root, rb_tree_node_base* leftmost, rb_tree_node_base* rightmost) {
		rb_tree_node_base* y = z;
		rb_tree_node_base* x = 0;
		rb_tree_node_base* x_parent = 0;
		//先调整树形，然后处理颜色
		if (y->m_left == 0)
			x = y->m_right;
		else
			if (y->m_right == 0)//z只有左或右子树时，y = z
				x = x->m_left;
			else {
				y = y->m_right;
				while (y->m_left != 0)//此时y是z的后继
					y = y->m_left;
				x = y->m_right;
			}
		if (y != z) {//z不只有一个子节点，将自己的后继提上来
			z->m_left->m_parent = y;
			y->m_left = z->m_left;//处理z的左子节点
			if (y != z->m_right) {//当y是z的后继却不是z的右子节点时，需要处理y的右子树和y的parent
				x_parent = y->m_parent;
				if (x) x->m_parent = y->m_parent;
				y->m_parent->m_left = x;
				y->m_right = z->m_right;
				z->m_right->m_parent = y;
			}							
			else//当y是z的后继也是z的右子节点时，y同样没有左子树，此时不需要处理y的右子树和y的parent
				x_parent = y;
			if (root == z)
				root = y;
			else if (z->m_parent->m_left == z)
				z->m_parent->m_left = y;
			else
				z->m_parent->m_right = y;
			y->m_parent->m_right = y;//处理z的parent与y的关系
			std::swap(y->m_color, z->m_color);
			y = z;
		}
		else {//z只有一个子节点，将其提上来就行
			x_parent = y->m_parent;
			if (x) x->m_parent = y->m_parent;
			if (root == z)
				root = x;
			else
				if (z->m_parent->m_left == z)
					z->m_parent->m_left = x;
				else
					z->m_parent->m_right = x;
			if (leftmost == z)
				if (z->m_right == 0)
					leftmost = z->m_parent;
				else
					leftmost = rb_tree_node_base::minimun(x);
			if (rightmost == z)
				if (z->m_left == 0)
					rightmost = z->m_parent;
				else
					rightmost = rb_tree_node_base::maximun(x);
		}
		//接下来处理红黑树的颜色问题
		if (y->m_color == rb_tree_red) return y;
		while (x != root && (x == 0 || x->m_color == rb_tree_black))
			if (x == x_parent->left) {
				rb_tree_node_base* w = x_parent->m_right;//w为伯父节点
				if (w->m_color == rb_tree_red) {//情况3，伯父节点为红色，左旋后转入情况4
					w->m_color = rb_tree_black;
					x_parent->m_color = rb_tree_red;
					rb_tree_rotate_left(x_parent, root);
					w = x_parent->m_right;
				}
				if ((w->m_left == 0 || w->m_left->m_color == rb_tree_black) && (w->m_right == 0 || w->m_right->m_color == rb_tree_black)) {//情况4中w的左右子节点均为黑
					w->m_color = rb_tree_red;
					x = x_parent;
					x_parent = x_parent->m_parent;
				}
				else {
					if (w->m_right == 0 || w->m_right->m_color == rb_tree_black) {//左子节点为红时，需要先右旋
						if (w->m_left)w->m_left->m_color = rb_tree_black;
						w->m_color = rb_tree_red;
						rb_tree_rotate_right(w, root);
						w = x_parent->m_right;
					}
					w->m_color = x_parent->m_color;
					x_parent->m_color = rb_tree_black;
					if (w->m_right) w->m_right->m_color = rb_tree_black;
					rb_tree_rotate_left(x_parent, root);
					break;
				}
			}
			else {//与上述情况对称
				rb_tree_node_base* w = x_parent->m_left;
				if (w->m_color == rb_tree_red) {
					w->m_color = rb_tree_black;
					x_parent->m_color = rb_tree_red;
					rb_tree_rotate_right(x_parent, root);
					w = x_parent->m_left;
				}
				if (w->m_left == 0 || w->m_left->m_color == rb_tree_black && w->m_right == 0 || w->m_right->m_color == rb_tree_black) {
					w->m_color = rb_tree_red;
					x = x_parent;
					x_parent = x_parent->m_parent;
				}
				else {
					if (w->m_left == 0 || w->m_left->m_color == rb_tree_black) {
						if (w->m_right)w->m_right->m_color = rb_tree_black;
						w->m_color = rb_tree_red;
						rb_tree_rotate_left(w, root);
						w = x_parent->m_left;
					}
					w->m_color = x_parent->m_color;
					x_parent->m_color = rb_tree_black;
					if (w->m_left) w->m_left->m_color = rb_tree_black;
					rb_tree_rotate_right(x_parent, root);
					break;
				}
			}
		return y;
	}

	template<typename T, typename Alloc>
	class rb_tree_base {
		typedef Alloc allocator_type;
		allocator_type get_allocator() const { return allocator_type(); }

		rb_tree_base(const allocator_type&)
			:m_header(0) {
			m_header = m_get_node();
		}
		~rb_tree_base() { m_put_node(m_header); }

	protected:
		rb_tree_node<T*> m_header;//头节点不是根节点，父节点指向根节点，左子节点指向最小节点，右子节点指向最大节点

		typedef simple_alloc<rb_tree_node<T>, Alloc> Alloc_type;
		rb_tree_node<T*> m_get_node() {
			return Alloc_type::allocate(1);
		}
		void m_put_node(rb_tree_node<T>* p) {
			Alloc_type::deallocate(p, 1);
		}
	};

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc = alloc>
	class rb_tree :protected rb_tree_base<Value, Alloc> {
		typedef rb_tree_base<Value, Alloc> Base;
	protected:
		typedef rb_tree_node_base* base_ptr;
		typedef rb_tree_node<Value> rb_tree_node;
		typedef rb_tree_color_type Color_type;
	public:
		typedef Key key_value;
		typedef Value value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type* const_reference;
		typedef rb_tree_node* link_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		typedef typename Base::allocator_type allocator_type;
		allocator_type get_allocator() const { return Base::get_allocator(); }
	protected:
		/*申请内存并且construct*/
		link_type m_create_node(const value_type& x) {
			link_type temp = m_get_node();
			try {
				construct(&temp->m_value, x);
			}
			catch (...) {
				m_put_node(temp);
			}
			return temp;
		}
		/*返回x的clone节点，已经申请内存*/
		link_type m_clone_node(link_type x) {
			link_type temp = m_create_node(x->m_value);
			temp->m_color = m->color;
			temp->m_left = 0;
			temp->m_right = 0;
			return temp;
		}
		/*析构并释放*/
		void destroy_node(link_type p) {
			destroy(&p->m_value);
			m_put_node(p);
		}
	protected:
		size_type m_node_count;//节点数量
		Compare m_key_compare;//键值的比较方式

		link_type& m_root()const {
			return m_header->m_parent;
		}
		link_type& m_leftmost() const {
			return m_head->m_left;
		}
		link_type& m_rightmost() const {
			return m_head->right;
		}

		static link_type& s_left(link_type x) {
			return x->m_left;
		}
		static link_type& s_right(link_type x) {
			return x->m_right;
		}
		static link_type& s_parent(link_type x) {
			return x->m_parent;
		}
		static link_type& s_value(link_type x) {
			return x->m_value;
		}
		static link_type& s_key(link_type x) {
			return x->Keyofvalue()(s_value(x));
		}
		static link_type& s_color(link_type x) {
			return x->m_color;
		}

		static link_type& s_left(Base_ptr x) {
			return x->m_left;
		}
		static link_type& s_right(Base_ptr x) {
			return x->m_right;
		}
		static link_type& s_parent(Base_ptr x) {
			return x->m_parent;
		}
		static link_type& s_value(Base_ptr x) {
			return x->m_value;
		}
		static link_type& s_key(Base_ptr x) {
			return x->Keyofvalue()(s_value(x));
		}
		static link_type& s_color(Base_ptr x) {
			return x->m_color;
		}

		static link_type s_minimun(link_type) {
			return rb_tree_node_base::mininum(x);
		}
		static link_type s_maximun(link_type) {
			return rb_tree_node_base::maxinum(x);
		}

	public:
		typedef rb_tree_iterator<value_type, reference, pointer> iterator;
		typedef rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;

		typedef reverse_iterator<iterator> reverse_iterator;
		typedef reverse_iterator<const_iterator> const_reverse_iterator;

	private:
		iterator m_insert(Base_ptr x, Base_ptr y, const value_type& v);
		link_type m_copy(link_type x, link_type p);
		void m_erase(link_type x);

	public:
		rb_tree() :Base(allocator_type()), m_node_count(0), m_key_compare() {
			m_empty_initialize();
		}
		rb_tree(const Compare& comp) :Base(allocator_type()), m_node_count(0), m_key_compare(comp) {
			m_empty_initialize();
		}
		rb_tree(const Compare& comp, const allocator_type& a) :Base(a), m_node_count(0), m_key_compare(comp) {
			m_empty_initialize();
		}
		rb_tree(const rb_tree<Key, Value, Keyofvalue, Compare, Alloc>& x) :Base(x.get_allocator()), m_node_count(0), m_key_compare(x.m_key_compare) {
			if (x.m_root() == 0) {
				m_empty_initialize();
			}
			else {
				s_color(m_header) = rb_tree_red;
				m_root() = m_copy(x.m_root(), m_header);
				m_leftmost() = s_minimun(m_root());//处理m_header的左右子节点
				m_rightmost() = s_maximun(m_root());
			}
			m_node_count = m_node_count;
		}
		~rb_tree() { clear(); }
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>& operator=(const rb_tree<Key, Value, Keyofvalue, Compare, Alloc>& x);
	private:
		void m_empty_inintialize() {
			s_color(m_header) = rb_tree_red;
			m_root() = 0;
			m_leftmost() = m_header;
			m_rightmost() = m_header;
		}
	public:
		Compare key_comp() const { return m_key_compare; }
		iterator begin() { return m_leftmost(); }
		const_iterator begin() const { return m_leftmost(); }
		iterator end() { return m_header; }
		const_iterator end() const { return m_header; }
		reverse_iterator rbegin() { return reverse_iterator(end()); }
		const_revsere_iterator rbegin()const { return const_reverse_iterator(end()); }
		reverse_iterator rend() { return reverse_iterator(begin()); }
		const_revsere_iterator rend()const { return const_reverse_iterator(begin()); }

		bool empty() const { return m_node_count == 0; }
		size_type size() const { return m_node_count; }
		size_type max_size() const { return size_type(-1); }

		void swap(rb_tree<Key, Value, Keyofvalue, Compare, Alloc>& t) {
			swap(m_header, t.m_header);
			swap(m_node_count, t.m_node_count);
			swap(m_key_compare, t.m_key_compare);
		}

	public:
		pair<iterator, bool> insert_unique(const value_type& v);
		iterator insert_equal(const value_type& x);
		iterator insert_unique(iterator position, const value_type& x);
		iterator insert_equal(iterator position, const value_type& x);

		void insert_unique(const_iterator first, const_iterator last);
		void insert_unique(const value_type* first, const value_type* last);
		void insert_equal(const_iterator first, const_iterator last);
		void insert_equal(const value_type* first, const value_type* last);

		void erase(iterator position);
		size_type erase(const key_type& x);
		void erase(iterator first, iterator last);
		void erase(const key_type* first, const key_type* last);
		void clear() {
			if (m_node_count != 0) {
				m_erase(m_root());//清理根节点及其子树
				m_leftmost() = m_header;
				m_root() = 0;
				m_rightmost() = m_header;//处理头节点点
				m_node_count = 0;
			}
		}
	public:
		iterator find(const key_type& x);
		const_iterator find(const key_type& x) const;
		size_type count(const key_type& x) const;
		iterator lower_bound(const key_type& x);
		iterator upper_bound(const key_type& x);
		const_iterator lower_bound(const key_type& x) const;
		const_iterator upper_bound(const key_type& x) const;
		pair<iterator, iterator> equal_range(const key_type& x);
		pair<const_iterator, const_iterator> equal_range(const key_type& x) const;
	public:
		bool rb_verify() const;
	};
	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	inline bool operator==(const rb_tree<Key, Value, Keyofvalue, Compare, Alloc>& x, const rb_tree<Key, Value, Keyofvalue, Compare, Alloc>& y) {
		return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	inline bool operator<(const rb_tree<Key, Value, Keyofvalue, Compare, Alloc>& x, const rb_tree<Key, Value, Keyofvalue, Compare, Alloc>& y) {
		return lexicographical_compare(x.begin(), x.end(),
			y.begin(), y.end());
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>&
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::operator=(const rb_tree<Key, Value, Keyofvalue, Compare, Alloc>& x) {
		if (this == &x) return *this;
		clear();
		m_key_compare = x.m_key_compare;
		if (x->m_root() != 0) {
			m_root() = m_copy(x.m_root, m_header);
			m_leftmost() = s_mininum(m_root());
			m_rightmost() = s_maxinum(m_root());
			m_node_count = x.m_node_count;
		}
		return *this;
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::iterator
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::m_insert(Base_ptr x, Base_ptr y, const value_type& v) {
		link_type z;
		if (y == m_header || x != 0 || m_key_compare(Keyofvalue()(v), s_key(yy))) {/*判断y是否为头节点，x控制是否强制v为y的左子节点*/
			z = m_create_node(v);
			s_left(y) = z;
			if (y == m_header) {
				m_root() = z;
				m_rightmost() = z;
			}
			else if (y == m_leftmost())
				m_leftmost() = z;
		}
		else {
			z = m_create_node(v);
			s_right(y) = z;
			if (y == m_rightmost())
				m_rightmost() = z;
		}
		s_parent(z) = y;
		s_left(z) = 0;
		s_right(z) = 0;
		rb_tree_rebalance(z, m_header->m_parent);
		++m_node_count;
		return iterator(z);
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::iterator
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::insert_equal(const Value& v) {
		link_type y = m_header;
		link_type x = m_root();
		while (x != 0) {//x是最后v应该插入的叶子节点
			y = x;
			x = m_key_compare(Keyofvalue()(v), s_key(x)) ? s_left(x) : s_right(x);
		}
		return m_insert(x, y, v);
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	pair<typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::iterator, bool>
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::insert_unique(const value_type& v) {
		link_type y = m_header;
		link_type x = m_root();
		bool comp = true;
		while (x != 0) {//x是最后v应该插入的叶子节点
			y = x;
			comp = m_key_compare(Keyofvalue()(v), s_key(x));
			x = comp ? s_left(x) : s_right(x);
		}
		iterator j = iterator(y);
		if (comp)//x是y的左子节点
			if (j == begin())
				return pair<iterator, bool>(m_insert(x, y, v), true);//x成为新的最小节点，没有重复
			else
				--j;//令j为y的前驱
		if (m_key_compare(s_key(j.m_node), Keyofvalue()(v))//比较j和x来判断x是否重复，若x是y的左子节点，那么j就是y的前驱，否则j就是y，因为如x与y相等，那么x是y的右子节点
			return pair<iterator, bool>(m_insert(x, y, v), true);
			return pair<iterator, bool>(j, false);
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::iterator
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::insert_equal(iterator position, const Value& v) {
		if (position.m_node == m_header->m_left) { // begin()
			if (size() > 0 &&
				!m_key_compare(s_key(position.m_node), Keyofvalue()(v)))
				return m_insert(position.m_node, position.m_node, v); 
			else
				return insert_equal(v);
		}
		else if (position.m_node == m_header) {// end()
			if (!m_key_compare(_KeyOfValue()(v), s_key(m_rightmost())))
				return m_insert(0, m_rightmost(), v);
			else
				return insert_equal(v);
		}
		else {
			iterator before = position;
			--before;
			if (!m_key_compare(Keyofvalue()(v), s_key(before.m_node))
				&& !m_key_compare(s_key(position.m_node), Keyofvalue()(v))) {
				if (s_right(before.m_node) == 0)
					return m_insert(0, before.m_node, v);
				else
					return m_insert(position.m_node, position.m_node, v); 
			}
			else
				return insert_equal(v);
		}
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::insert_equal(const Value& first, const Value& last) {
		for (; first != last; ++first)
			insert_equal(*first);
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::insert_equal(const_iterator first, const_iterator last) {
		for (; first != last; ++first)
			insert_equal(*first);
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::iterator
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::insert_unique(iterator position, const Value& v) {
		if (position.m_node == m_header->m_left) { // begin()
			if (size() > 0 &&
				m_key_compare(Keyofvalue()(v)), s_key(position.m_node))
				return m_insert(position.m_node, position.m_node, v);
			else
				return insert_unique(v).first;
		}
		else if (position.m_node == m_header) {// end()
			if (m_key_compare(s_key(m_rightmost())), _KeyOfValue()(v))
				return m_insert(0, m_rightmost(), v);
			else
				return insert_unique(v).first;
		}
		else {
			iterator before = position;
			--before;
			if (s_key(before.m_node), m_key_compare(Keyofvalue()(v))
				&& !m_key_compare(Keyofvalue()(v)), s_key(position.m_node)) {
				if (s_right(before.m_node) == 0)
					return m_insert(0, before.m_node, v);
				else
					return m_insert(position.m_node, position.m_node, v);
			}
			else
				return insert_unique(v).first;
		}
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::insert_unique(const Value& first, const Value& last) {
		for (; first != last; ++first)
			insert_unique(*first);
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::insert_unique(const_iterator first, const_iterator last) {
		for (; first != last; ++first)
			insert_unique(*first);
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	inline void rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::erase(iterator position) {
		link_type y = (link_type)rb_tree_rebalance_for_erase(position.m_node, m_header->m_parent, m_header->m_left, m_header->m_right);
		destroy_node(y);
		--m_node_count;
	}
	
	/*擦除x为键值的节点，返回其数量*/
	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::size_type
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::erase(const key_type& x) {
		pair<iterator, iterator> p = equal_range(x);
		size_type n = 0;
		distance(p.first, p.second, n);
		erase(p.first, p.second);
		return n;
	}
	
	/*摧毁以x为根节点的整棵子树，没有rebalence*/
	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::m_erase(link_type x) {
		while (x != 0) {
			m_erase(s_right(x));
			link_type y = s_left(x);
			destroy_node(x);
			x = y;
		}
	}
	
	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::erase(const key_type* first, const key_type* last) {
		while (first != last) erase(*first++);
	}
	
	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	void rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::erase(iterator first, iterator last) {
		if (first == begin() && last == end())
			clear;
		else
			while (first != last) erase(first++);
	}

	/*返回第一个大于等于k的数*/
	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::iterator
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::find(const key_type& k) {
		link_type y = m_header;
		link_type x = m_root();
		while (x != 0)
			if (!m_key_compare(s_key(x), k))//令y是第一个>=k的数
				y = x, x = s_left(x);
			else
				x = s_right(x);
		iterator j = iterator(y);
		return (j == end() || m_key_compare(k, s_key(j.m_node))) ?
			end() : j;
	}
	
	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::const_iterator
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::find(const key_type& k) const {
		link_type y = m_header;
		link_type x = m_root();
		while (x != 0)
			if (!m_key_compare(s_key(x), k))
				y = x, x = s_left(x);
			else
				x = s_right(x);
		iterator j = iterator(y);
		return (j == end() || m_key_compare(k, s_key(j.m_node))) ?
			end() : j;
	}
	
	/*记录以k为键值的数*/
	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::size_type
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::count(const key_type& k) const {
		pair<const_iterator, const_iterator> = equal_range(k);
		size_type n = 0;
		distance(p.first, p.second, n);
		return n;
	}

	/*找到第一个大于等于k的数*/
	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::iterator
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::lower_bound(const key_type& k) {
		link_type y = m_header;
		link_type x = m_root();
		while (x != 0)
			if (!m_key_compare(s_key(x), k))//令y是第一个>=k的数
				y = x, x = s_left(x);
			else
				x = s_right(x);
		return iterator(y);
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::const_iterator
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::lower_bound(const key_type& k) const {//>=k的第一个节点
		link_type y = m_header;
		link_type x = m_root();
		while (x != 0)
			if (!m_key_compare(s_key(x), k))
				y = x, x = s_left(x);
			else
				x = s_right(x);
		return iterator(y);
	}
	
	/*找到第一个大于k的数*/
	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::iterator
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::upper_bound(const key_type& k) {
		link_type y = m_header;
		link_type x = m_root();
		while (x != 0)
			if (m_key_compare(k, s_key(x)))//令y是第一个>k的数，不包含=
				y = x, x = s_left(x);
			else
				x = s_right(x);
		return iterator(y);
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::const_iterator
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::upper_bound(const key_type& k) const {//>=k的第一个节点
		link_type y = m_header;
		link_type x = m_root();
		while (x != 0)
			if (m_key_compare(k, s_key(x)))
				y = x, x = s_left(x);
			else
				x = s_right(x);
		return iterator(y);
	}

	/*找到等于键值等于k的迭代器*/
	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	inline pair<typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::iterator,
		typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::iterator>
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::equal_range(const key_type& k) {
		return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
	}

	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	inline pair<typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::const_iterator,
		typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::const_iterator>
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::equal_range(const key_type& k) const {
		return pair<iterator, iterator>(lower_bound(k), upper_bound(k));
	}

	/*自node至root路径black节点数量*/
	inline int black_count(rb_tree_node_base* node, rb_tree_node_base* root) {
		if (node == 0) {
			return 0;
		}
		else {
			int bc = node->m_color == rb_tree_black ? 1 : 0;
			if (node == root)
				return bc;
			return bc + black_count(node->m_parent, root);
		}
	}

	/*检验红黑树的合法性*/
	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	bool rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::rb_verify()const {
		if (m_node_count == 0 || begin() == end())//仅有头节点
			return m_node_count == 0 && begin() == end() && m_header->m_left == m_header && m_header->m_right == m_header;
		int len = black_count(m_leftmost(), m_root());
		for (const_iterator it = begin(); it != end(); ++it) {
			link_type x = it.m_node;
			link_type l = s_left(x);
			link_type r = s_right(x);
			if (x->m_color == rb_tree_red)
				if (l && l->m_color == rb_tree_red || r && r->m_color == rb_tree_red)//是否有连续红
					return false;
			if (l && m_key_compare(s_key(x), s_key(l)))//左子节点与父节点的比较是否合规
				return false;
			if (r && m_key_compare(s_key(r), s_key(x)))//左子节点与父节点的比较是否合规
				return false;
			if (!l && !r && black_count(x, m_root()) != len)//叶子节点时黑色节点的数量
				return false;
		}
		if (m_leftmost() != rb_tree_node_base::minimun(m_root()))
			return false;
		if (m_rightmost() != rb_tree_node_base::maximun(m_root()))
			return false;
		return true;
	}
	
	/*复制x为根的子树到p为头节点的树下*/
	template<typename Key, typename Value, typename Keyofvalue, typename Compare, typename Alloc>
	typename rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::link_type
		rb_tree<Key, Value, Keyofvalue, Compare, Alloc>::m_copy(link_type x, link_type p) {
		link_type top = m_clone_node(x);
		top->m_parent = p;
		try {
			if (x->m_right)
				top->m_right = m_copy(s_right(x), top);//递归调用m_copy，可以理解为右子树已经全部复制完毕
			p = top;
			x = s_left(x);

			while (x != 0) {
				link_type y = m_clone_node(x);//接下来复制左子树
				p->m_left = y;
				y->m_parent = p;
				if (x->m_right)
					y->m_right = m_copy(s_right(x), y);//左子树的复制同样也是先左子树的右子树
				p = top;
				x = s_left(x);
			}
		}
		catch (...) {
			m_erase(top);
		}
	}

}

#endif