#ifndef _HBTINYSTL_HASHTABLE_H_
#define _HBTINYSTL_HASHTABLE_H_
#include <algobase.h>
#include <alloc.h>
#include <construct.h>
#include <memory.h>
#include <stl_algo.h>
#include <uninitialized.h>
#include <stl_function.h>
#include <vector.h>
#include <stl_hash_fun.h>
namespace HBtinySTL {
	template<typename Value>
	class hashtable_node {
		hashtable_node* m_next;
		Value m_value;
	};
	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	class hashtable;
	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc>
	class hashtable_const_iterator;

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc>
	class hashtable_iterator {
		typedef hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc> Hashtable;
		typedef hashtable_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc> iterator;
		typedef hashtable_const_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc> const_iterator;
		typedef hashtable_node<Value> Node;
		typedef forward_iterator_tag iterator_category;
		typedef Value value_type;
		typedef ptrdiff_t difference_type;
		typedef size_t size_type;
		typedef Value& reference;
		typedef Value* pointer;

		Node* m_cur;
		Hashtable* m_ht;

		hashtable_iterator(Node* n, Hashtable* tab) :m_cur(n), m_ht(tab) {}
		hashtable_iterator() {}
		reference operator*()const { return m_cur->m_val; }
		iterator& operator++();
		iterator operator++(int);
		bool operator==(const iterator& it)const { return m_cur == it.m_cur; }
		bool operator!=(const iterator& it)const { return m_cur != it.m_cur; }
	};
	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc>
	class hashtable_const_iterator {
		typedef hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc> Hashtable;
		typedef hashtable_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc> iterator;
		typedef hashtable_const_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc> const_iterator;
		typedef hashtable_node<Value> Node;
		typedef forward_iterator_tag iterator_category;
		typedef Value value_type;
		typedef ptrdiff_t difference_type;
		typedef size_t size_type;
		typedef const Value& reference;
		typedef const Value* pointer;

		Node* m_cur;
		Hashtable* m_ht;

		hashtable_const_iterator(Node* n, Hashtable* tab) :m_cur(n), m_ht(tab) {}
		hashtable_const_iterator() {}
		hashtable_const_iterator(const iterator& it) :m_cur(it.m_cur), m_ht(it.m_ht) {}
		reference operator*()const { return m_cur->m_val; }
		iterator& operator++();
		iterator operator++(int);
		bool operator==(const iterator& it)const { return m_cur == it.m_cur; }
		bool operator!=(const iterator& it)const { return m_cur != it.m_cur; }
	};
	enum { num_primes = 28 };

	static const unsigned long prime_list[num_primes] = {
		 53ul,         97ul,         193ul,       389ul,       769ul,
	  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
	  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
	  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
	  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul,
	  1610612741ul, 3221225473ul, 4294967291ul
	};
	inline unsigned long next_prime(unsigned long n)
	{
		const unsigned long* first = prime_list;
		const unsigned long* last = prime_list + (int)num_primes;
		const unsigned long* pos = lower_bound(first, last, n);
		return pos == last ? *(last - 1) : *pos;
	}

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	class hashtable {
	public:
		typedef Key key_type;
		typedef Value value_type;
		typedef Hashfcn hasher;
		typedef Equalkey key_equal;

		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;

		hasher hash_funct()const { return m_hash; }
		key_equal key_eq()const { return m_equals; }

	private:
		typedef hashtable_node<Value> Node;
	public:
		typedef Alloc allocator_type;
		allocator_type get_allocator() const { return allocator_type(); }
	private:
		typedef simple_alloc<Node, Alloc> m_node_allocator_type;
		Node* m_get_node() { return m_node_allocator_type::allocate(1); }
		void m_put_node(Node* p) { m_node_allocator_type::deallocate(p, 1); }

	private:
		hasher m_hash;
		key_equal m_equals;
		Extractkey m_get_key;
		vector<Node*, Alloc> m_buckets;
		size_type m_num_elements;

	public:
		typedef hashtable_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc> iterator;
		typedef hashtable_const_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc> const_iterator;
		friend class hashtable_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>;
		friend class hashtable_const_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>;
	public:
		hashtable(size_type n, const Hashfcn& hf, const Equalkey& eql, const Extractkey& ext, const allocator_type& a = allocator_type())
			:m_hash(hf), m_equals(eql), m_get_key(ext), m_buckets(a), m_num_elements(0) {
			m_initialize_buckets(n);
		}
		hashtable(size_type n, const Hashfcn& hf, const Equalkey& eql, const allocator_type& a = allocator_type())
			:m_hash(hf), m_equals(eql), m_buckets(a), m_num_elements(0) {
			m_initialize_buckets(n);
		}
		hashtable(const hashtable& ht)
			:m_hash(ht.m_hash), m_equals(ht.m_equals), m_get_key(ht.m_get_key), m_buckets(ht.get_allocator()), m_num_elements(0) {
			m_copy_from(ht);
		}
		hashtable& operator=(const hashtable& ht) {
			if (this != &ht) {
				clear();
				m_hash = ht.m_hash;
				m_equals = ht.m_equals;
				m_get_key = ht.m_get_key;
				m_copy_from(ht);
			}
			return *this;
		}
		~hashtable() { clear; }

		size_type size()const { return m_num_elements; }
		size_type max_size()const { return size_type(-1); }
		bool empty()const { return size() == 0; }

		void swap(hashtable& ht) {
			std::swap(m_hash, ht.m_hash);
			std::swap(m_equals, ht.m_equals);
			std::swap(m_get_key, ht.m_get_key);
			m_buckets.swap(ht.m_buckets);
			std::swap(m_num_elements, ht.m_num_elements);
		}

		iterator begin() {
			for (size_type n = 0; n < m_buckets.size(); ++n)
				if (m_buckets[n])
					return iterator(m_buckets[n], this);
			return end();
		}
		iterator end() { return iterator(0, this); }

		const_iterator begin() const {
			for (size_type n = 0; n < m_buckets.size(); ++n)
				if (m_buckets[n])
					return const_iterator(m_buckets[n], this);
			return end();
		}
		const_iterator end() { return const_iterator(0, this); }

		size_type bucket_count()const { return m_buckets.size(); }
		size_type max_bucket_count()const { return prime_list[(int)num_primes - 1]; }

		size_type elems_in_bucket(size_type bucket) const {
			size_type result = 0;
			for (Node* cur = m_buckets[bucket]; cur; cur = cur->m_next)
				result += 1;
			return result;
		}
		pair<iterator, bool> insert_unique(const value_type& obj) {
			resize(m_num_elements + 1);
			return insert_unique_noresize(obj);
		}
		iterator insert_equal(const value_type& obj) {
			resize(m_num_elements + 1);
			return insert_equal_noresize(obj);
		}
		pair<iterator, bool> insert_unique_noresize(const value_type& obj);
		iterator insert_equal_noresize(const value_type& obj);

		void insert_unique(const value_type* f, const value_type* l) {
			size_type n = l - f;
			resize(m_num_elements + n);
			for (; n > 0; --n, ++f)
				insert_unique_noresize(*f);
		}
		void insert_equal(const value_type* f, const value_type* l) {
			size_type n = 0;
			distance(f, l, n);
			resize(m_num_elements + n);
			for (; n > 0; --n, ++f) {
				insert_equal_noresize(*f);
			}
		}
		void insert_unique(const_iterator f, const_iterator l) {
			size_type n = l - f;
			resize(m_num_elements + n);
			for (; n > 0; --n, ++f)
				insert_unique_noresize(*f);
		}
		void insert_equal(const_iterator f, const_iterator l) {
			size_type n = 0;
			distance(f, l, n);
			resize(m_num_elements + n);
			for (; n > 0; --n, ++f) {
				insert_equal_noresize(*f);
			}
		}

		reference find_or_insert(const value_type& obj);

		iterator find(const key_type& key) {
			size_type n = m_bkt_num_key(key);
			Node* first;
			for (first = m_buckets[n]; first && !m_equals(m_get_key(first->m_val), key); first = first->m_next)
			{
			}
			return iterator(first, this);
		}
		const_iterator find(const key_type& key) const {
			size_type n = m_bkt_num_key(key);
			Node* first;
			for (first = m_buckets[n]; first && !m_equals(m_get_key(first->m_val), key); first = first->m_next)
			{
			}
			return iterator(first, this);
		}

		size_type count(const key_type* key) const {
			const size_type n = m_bkt_num_key(key);
			size_type result = 0;
			for (const Node* cur = m_buckers[n]; cur; cur = cur->m_next)
				if (m_equals(m_get_key(cur->m_val), key))
					++result;
			return result;
		}

		pair<iterator, iterator> equal_range(const key_type& key);
		pair<const_iterator, const_iterator> equal_range(const key_type& key) const;

		size_type erase(const key_type& key);
		void erase(const iterator& it);
		void erase(iterator first, iterator last);

		void erase(const const_iterator& it);
		void erase(const_iterator first, const_iterator last);

		void resize(size_type num_elements_hint);
		void clear();

	private:
		size_type m_next_size(size_type n) const {
			return next_prime(n);
		}
		void m_initialize_buckets(size_type n) {
			const size_type n_buckets = m_next_size(n);
			m_buckets.reserve(n_buckets);
			m_buckets.insert(m_buckets.end(), n_buckets, (Node*)0);
			m_num_elements = 0;
		}

		size_type m_bkt_num_key(const key_type& key) const {
			return m_bkt_num_key(key, m_buckets.size());
		}
		size_type m_bkt_num(const value_type& obj) const {
			return m_bkt_num_key(m_get_key(obj));
		}
		size_type m_bkt_num_key(const key_type& key, size_t n) const {
			return m_hash(key) % n;
		}
		size_type m_bkt_num(const value_type& obj, size_t n) const {
			return m_bkt_num_key(m_get_key(obj), n);
		}

		Node* m_new_node(const value_type& obj) {
			Node* n = m_get_node();
			n->m_next = 0;
			try {
				construct(&n->m_value, obj);
				return n;
			}
			catch (...) {
				m_put_node(n);
			}
		}
		void m_delete_node(Node* n) {
			destroy(&n->m_value);
			m_put_node(n);
		}
		void m_erase_bucket(const size_type n, Node* first, Node* last);
		void m_erase_bucket(const size_type n, Node* first);

		void m_copy_from(const hashtable& ht);
	};

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	hashtable_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>&
		hashtable_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::operator++() {
		const Node* old = m_cur;
		m_cur = m_cur->m_next;
		if (!m_cur) {
			size_type bucket = m_ht->m_bkt_num(old->m_value);
			while (!m_cur && ++bucket < m_ht->m_buckets.size())
				m_cur = m_ht->elems_in_bucket[bucket];
		}
		return *this;
	}
	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	inline hashtable_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>
		hashtable_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::operator++(int) {
		iterator temp = *this;
		++(*this);
		return temp;
	}
	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	hashtable_const_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>&
		hashtable_const_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::operator++() {
		const Node* old = m_cur;
		m_cur = m_cur->m_next;
		if (!m_cur) {
			size_type bucket = m_ht->m_bkt_num(old->m_value);
			while (!m_cur && ++bucket < m_ht->m_buckets.size())
				m_cur = m_ht->elems_in_bucket[bucket];
		}
		return *this;
	}
	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	inline hashtable_const_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>
		hashtable_const_iterator<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::operator++(int) {
		iterator temp = *this;
		++(*this);
		return temp;
	}

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	bool operator==(const hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& ht1,
		const hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>& ht2) {
		typedef typename hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::Node Node;
		if (ht1.m_buckets.size() != ht2.m_buckets.size())
			return false;
		for (int n = 0; n < ht1.elems_in_bucket.size(); ++n) {
			Node* cur1 = ht1.m_buckets[n];
			Node* cur2 = ht2.m_buckets[n];
			for (; cur1 && cur2 && cur1->m_val == cur2->m_val; cur1 = cur1->m_next, cur2 = cur2->m_next)
				;
			if (cur1 || cur2)
				return false;
		}
		return true;
	}

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	pair<typename hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::iterator, bool>
		hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::insert_unique_noresize(const value_type& obj) {
		const size_type n = m_bkt_num(obj);
		Node* first = m_buckets[n];
		for (Node* cur = first; cur; cur = cur->m_next)
			if (m_equals(m_get_key(cur->m_value), m_get_key(obj)))
				return pair<iterator, bool>(iterator(cur, this), false);
		Node* temp = n_new_node(obj);
		temp->m_next = first;
		m_buckets[n] = temp;
		++m_num_elements;
		return pair<iterator, bool>(iterator(temp, this), true);
	}

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	typename hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::iterator
		hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::insert_equal_noresize(const value_type& obj) {
		const size_type n = m_bkt_num(obj);
		Node* first = m_buckets[n];
		for (Node* cur = first; cur; cur = cur->m_next)
			if (m_equals(m_get_key(cur->m_value), m_get_key(obj))) {
				Node* temp = n_new_node(obj);
				temp->m_next = cur->m_next;
				cur->m_next = temp;
				++m_num_elements;
				return iterator(temp, this);
			}
		Node* temp = n_new_node(obj);
		temp->m_next = first;
		m_buckets[n] = temp;
		++m_num_elements;
		return iterator(temp, this);
	}

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	typename hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::reference
		hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::find_or_insert(const value_type& obj) {
		resize(m_num_elements + 1);
		const size_type n = m_bkt_num(obj);
		Node* first = m_buckets[n];
		for (Node* cur = first; cur; cur = cur->m_next)
			if (m_equals(m_get_key(cur->m_value), m_get_key(obj)))
				return cur->m_value;
		Node* temp = m_new_node(obj);
		temp->m_next = first;
		m_bucket[n] = temp;
		++m_num_elements;
		return temp->m_value;
	}

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	pair<typename hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::iterator, typename hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::iterator>
		hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::equal_range(const key_type& key) {
		typedef pair<iterator, iterator> pii;
		const size_type n = m_bkt_num_key(key);
		for (Node* first = m_buckets[n]; first; first = first->m_next)
			if (m_equals(m_get_key(first->m_val), key)) {
				for (Node* cur = first->m_next; cur; cur = cur->m_next) {
					if (!m_equals(m_get_key(cur->m_value), key))
						return pii(iterator(first, this), iterator(cur, this));
				}
				for (size_type m = n + 1; m < m_buckets.size(); ++m)
					if (m_buckets[m])
						return pii(iterator(first, this), iterator(m_buckets[m], this));
				return pii(iterator(first, this), end());
			}
		return pii(end(), end());
	}

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	pair<typename hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::const_iterator, typename hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::const_iterator>
		hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::equal_range(const key_type& key) {
		typedef pair<const_iterator, const_iterator> pii;
		const size_type n = m_bkt_num_key(key);
		for (Node* first = m_buckets[n]; first; first = first->m_next)
			if (m_equals(m_get_key(first->m_val), key)) {
				for (Node* cur = first->m_next; cur; cur = cur->m_next) {
					if (!m_equals(m_get_key(cur->m_value), key))
						return pii(iterator(first, this), iterator(cur, this));
				}
				for (size_type m = n + 1; m < m_buckets.size(); ++m)
					if (m_buckets[m])
						return pii(iterator(first, this), iterator(m_buckets[m], this));
				return pii(iterator(first, this), end());
			}
		return pii(end(), end());
	}

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	typename hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::size_type
		hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::erase(const key_type& key) {
		const size_type n = m_bkt_num_key(key);
		Node* first = m_buckets[n];
		size_type erased = 0;

		if (first) {
			Node* cur = first;
			Node* next = cur->m_next;
			while (next) {
				if (m_equals(m_get_key(next->m_val), key)) {
					cur->m_next = next->m_next;
					m_delete_node(next);
					next = cur->m_next;
					++erased;
					--m_num_elements;
				}
				else {
					cur = next;
					next = cur->m_next;
				}
			}
			if (m_equals(m_get_key(first->m_val), key)) {
				m_buckets[n] = first->m_next;
				m_delete_node(first);
				++erased;
				--m_num_elements;
			}
		}
		return erased;
	}
	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	void hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::erase(const iterator& it) {
		Node* p = it.m_cur;
		if (p) {
			const size_type n = m_bkt_num(p->m_val);
			Node* cur = m_buckets[n];

			if (cur == p) {
				m_buckets[__n] = cur->m_next;
				m_delete_node(cur);
				--m_num_elements;
			}
			else {
				Node* next = cur->m_next;
				while (next) {
					if (next == p) {
						cur->m_next = next->m_next;
						m_delete_node(next);
						--m_num_elements;
						break;
					}
					else {
						cur = next;
						next = cur->m_next;
					}
				}
			}
		}
	}
	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	void hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::erase(iterator first, iterator last) {
		size_type f_bucket = first.m_cur ? m_bkt_num(first.m_cur->m_val) : m_buckets.size();
		size_type l_bucket = last.m_cur ? m_bkt_num(first.m_cur->m_val) : m_buckets.size();

		if (first.m_cur == last.m_cur) return;
		else if (f_bucket == l_bucket)
			m_erase_bucket(f_bucket, first.m_cur, last.m_cur);
		else {
			m_erase_bucket(f_bucket, first.m_cur, 0);
			for (size_type n = f_bucket + 1; n < l_bucket; ++n)
				m_erase_bucket(n, 0);
			if (l_bucket != m_buckets.size())
				m_erase_bucket(l_bucket, last.m_cur);
		}
	}

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	inline void hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::erase(const_iterator first, const_iterator last) {
		erase(iterator(const_cast<_Node*>(first.m_cur),
			const_cast<hashtable*>(first.m_ht)),
			iterator(const_cast<_Node*>(last.m_cur),
				const_cast<hashtable*>(last.m_ht)));
	}

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	inline void hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::erase(const const_iterator& it) {
		erase(iterator(const_cast<Node*>(it.m_cur), const_cast<hashtable>(it.m_ht)));
	}

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	inline void hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::resize(size_type num_element_hint) {
		const size_type old_n = m_buckets.size();
		if (num_elements_hint > old_n) {
			const size_type n = m_next_size(num_elements_hint);
			if (n > old_n) {
				vector<Node*, Alloc> temp(n, (Node*)(0), m_buckets.get_allocator());
				try {
					for (size_type bucket = 0; bucket < old_n; ++bucket) {
						Node* first = m_buckets[bucket];
						while (first) {
							size_type new_bucket = m_bkt_num(first->m_value, n);
							m_buckets[bucket] = first->m_next;
							first->m_next = temp[new_bucket];
							temp[new_bucket] = first;
							first = m_bucket[bucket];
						}

					}
					m_buckets.swap[temp];
				}
				catch (...) {
					for (size_type bucket = 0; bucket < temp.size(); ++bucket) {
						while (tmp[bucket]) {
							Node* next = temp[bucket]->m_next;
							m_delete_node(temp[bucket]);
							temp[bucket] = next;
						}
					}
					throw;
				}
			}
		}
	}
	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	void hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::m_erase_bucket(const size_type n, Node* first, Node* last) {
		Node* cur = m_bucket[n];
		if (cur == first)
			m_erase_bucket(n, last);
		else {
			Node* next;
			for (next = cur->m_next; next != first; cur = next, next = cur->m_next)
				;
			while (next != last) {
				cur->m_next = next->m_next;
				m_delete_node(next);
				next = cur->m_next;
				--m_num_elements;
			}
		}

	}
	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	void hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::m_erase_bucket(const size_type n, Node* last) {
		Node* cur = m_bucket[n];
		while (cur != last) {
			Node* next = cur->m_next;
			m_delete_node(next);
			cur = next;
			m_buckets[n] = cur;
			--m_num_elements;
		}
	}

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	void hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::clear() {
		for (size_type i = 0; i < m_buckets.size(); ++i) {
			Node* cur = m_buckets[i];
			while (cur != 0) {
				Node* next = cur->m_next;
				m_delete_node(cur);
				cur = next;
			}
			m_buckets[i] = 0;
		}
		m_num_elements = 0;
	}

	template<typename Value, typename Key, typename Hashfcn, typename Extractkey, typename Equalkey, typename Alloc = alloc>
	void hashtable<Value, Key, Hashfcn, Extractkey, Equalkey, Alloc>::m_copy_from(const hashtable& ht) {
		m_buckets.clear();
		m_buckets.reserve(ht.m_buckets.size());
		m_buckets.insert(m_buckets.end(), ht.m_buckets.size(), (Node*)0);
		try {
			for (size_type i = 0; i < ht.m_buckets.size(); ++i) {
				const Node* cur = ht.m_buckets[i];
				if (cur) {
					Node* copy = m_new_node(cur->m_value);
					m_buckets[i] = copy;

					for (Node* next = cur->m_next;
						next;
						cur = next, next = cur->m_next) {
						copy->m_next = m_new_node(next->m_value);
						copy = copy->m_next;
					}
				}
			}
			_M_num_elements = __ht._M_num_elements;
		}
	}

}



#endif
