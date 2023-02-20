#ifndef _HBTINYSTL_SET_H_
#define _HBTINYSTL_SET_H_

#include<stl_function.h>

namespace HBtinySTL {
	template<typename Key, typename Compare, typename Alloc>
	class set {
	public:
		typedef Key key_type;
		typedef Key value_type;
		typedef Compare key_compare;
		typedef Compare value_compare;

	private:
		typedef rb_tree<key_type, value_type, _Identity<value_type>, key_compare, Alloc> rep_type;
		rep_type m_t;

	public:
		typedef typename rep_type::const_pointer			pointer;
		typedef typename rep_type::const_pointer			const_pointer;
		typedef typename rep_type::const_reference			reference;
		typedef typename rep_type::const_reference			const_reference;
		typedef typename rep_type::const_iterator			iterator;
		typedef typename rep_type::const_iterator			const_iterator;
		typedef typename rep_type::const_reverse_iterator	reverse_iterator;
		typedef typename rep_type::const_reverse_iterator	const_reverse_iterator;
		typedef typename rep_type::size_type				size_type;
		typedef typename rep_type::difference_type			difference_type;
		typedef typename rep_type::allocator_type			allocator_type;

		set() :m_t(Compare(), allocator_type()) {}
		explicit set(const Compare& comp, const allocator_type& a = allocator_type())
			:m_t(comp, a) {}

		set(const value_type* first, const value_type* last)
			:m_t(Compare(), allocator_type()) {
			m_t.insert_unique(first, last);
		}
		set(const value_type* first, const value_type* last, const Compare& comp, const allocator_type& a = allocator_type())
			:m_t(comp, a) {
			m_t.insert_unique(first, last);
		}
		set(const_iterator first, const_iterator last) :m_t(comp, allocator_type()) {
			m_t.insert_unique(first, last);
		}
		set(const_iterator first, const_iterator last, const Compare& comp, const allocator_type& a = allocator_type())
			:m_t(comp, a) {
			m_t.insert_unique(first, last);
		}
		set(const set<Key, Compare, Alloc>& x) :m_t(x.m_t) {}
		set<Key, Compare, Alloc>& operator=(const set<Key, Compare, Alloc>& x) {
			m_t = x.m_t;
			return *this;
		}


		key_compare key_comp() const { return m_t.key_comp()£» }
		value_compare value_comp() const { return m_t.key_comp(); }
		allocator_type get_allocator() const { return m_t.get_allocator(); }

		iterator begin() const { return m_t.begin(); }
		iterator end() const { return m_t.end(); }
		reverse_iterator rbegin() const { return m_t.rbegin(); }
		reverse_iterator rend() const { return m_t.rend(); }
		bool empty() const { return m_t.empty(); }
		size_type size() const { return m_t.size(); }
		size_type max_size() const { return m_t.max_size(); }
		void swap(set<_Key, _Compare, _Alloc>& __x) { m_t.swap(__x.m_t); }


		pair<iterator, bool> insert(const value_type& x) {
			pair<typename rep_type::iterator, bool> p = m_t.insert_unique(x);
			return pair<iterator, bool>(p.first, p.second);
		}

		iterator insert(iterator position, const value_type& x) {
			typedef typename rep_type::iterator rep_iterator;
			return m_t.insert_unique(rep_iterator&)position, x);
		}

		void insert(const_iterator first, const_iterator last) {
			m_t.insert_unique(first, last);
		}
		void insert(const value_type* first, const value_type* last) {
			m_t.insert_unique(first, last);
		}

		void erase(iterator position) {
			typedef typename rep_type::iterator rep_iterator;
			m_t.erase((rep_iterator&)position);
		}
		size_type erase(const key_type& __x) {
			return m_t.erase(__x);
		}
		void erase(iterator __first, iterator __last) {
			typedef typename _Rep_type::iterator _Rep_iterator;
			m_t.erase((_Rep_iterator&)__first, (_Rep_iterator&)__last);
		}
		void clear() { m_t.clear(); }



		iterator find(const key_type& __x) const { return m_t.find(__x); }
		size_type count(const key_type& __x) const {
			return m_t.find(__x) == m_t.end() ? 0 : 1;
		}
		iterator lower_bound(const key_type& __x) const {
			return m_t.lower_bound(__x);
		}
		iterator upper_bound(const key_type& __x) const {
			return m_t.upper_bound(__x);
		}
		pair<iterator, iterator> equal_range(const key_type& __x) const {
			return m_t.equal_range(__x);
		}







	};

}

#endif
