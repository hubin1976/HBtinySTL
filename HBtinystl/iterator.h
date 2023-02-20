#ifndef _HBTINYSTL_ITERATOR_H_
#define _HBTINYSTL_ITERATOR_H_

#include<cstddef>
#include<type_trait>

namespace HBtinySTL{

	class input_iterator_tag{};
	class output_iterator_tag{};
	class forward_iterator_tag: public input_iterator_tag{};
	class bidirectional_iterator_tag: public forward_iterator_tag {};
	class random_access_iterator_tag: public bidirectional_iterator_tag {};

	template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
	class iterator {
		typedef Category	iterator_category;
		typedef T			value_type;
		typedef Distance	difference_type;
		typedef Pointer		pointer;
		typedef Reference	reference;
	};
	
	template<class Iterator>
	class iterator_traits {
		typedef typename Iterator::iterator_category	iterator_category;
		typedef typename Iterator::value_type			value_type;
		typedef typename Iterator::difference_type		difference_type;
		typedef typename Iterator::pointer				pointer;
		typedef typename Iterator::reference			reference;
	};
	template<class T>
	class iterator_traits<T*> {
		typedef random_access_iterator_tag	iterator_category;
		typedef T							value_type;
		typedef ptrdiff_t					difference_type;
		typedef T*							pointer;
		typedef T&							reference;
	};
	template<class T>
	class iterator_traits<const T*> {
		typedef random_access_iterator_tag	iterator_category;
		typedef T							value_type;
		typedef ptrdiff_t					difference_type;
		typedef const T*					pointer;
		typedef const T&					reference;
	};

	template<class Iterator>
	inline typename iterator_traits<Iterator>::iterator_category
	iterator_category(const Iterator&){
		typedef typename iterator_traits<Iterator>::iterator_category Category;
		return Category();
	}

	template<class Iterator>
	inline typename iterator_traits<Iterator>::difference_type*
	difference_type(const Iterator&) {
		return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
	}

	template<class Iterator>
	inline typename iterator_traits<Iterator>::value_type*
	value_type(const Iterator&) {
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	}


	template<class InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type
	__distance(InputIterator first, InputIterator last, input_iterator_tag) {
		typename iterator_traits<InputIterator>::difference_type n = 0;
		while (first != last)
		{
			++first;
			++n;
		}
		return n;
	}
	template<class RandomAccessIterator>
	inline typename iterator_traits<RandomAccessIterator>::difference_type
	__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
		return last - first;
	}

	template<class Iterator>
	inline typename iterator_traits<Iterator>::difference_type
	distance(Iterator first, Iterator last) {
		typedef typename iterator_traits<Iterator>::iterator_category Category;
		return __distance(first, last, Category();
	}

	template<class InputIterator, class Distance>
	inline void __advance(InputIterator i, Distance n, input_iterator_tag) {
		while (n--) ++i;
	}

	template<class BiderectionalIterator, class Distance>
	inline void __advance(BidirectionalIterator i, Diatance n, bidirectional_iterator_tag) {
		if (n >= 0)
			while (n--) ++i;
		else
			while (n++) --i;
	}

	template<class RandomAccessIterator, class Distance>
	inline void  __advance(RandomAccessIterator i, Distance n, random_access_iterator_tag) {
		i += n;
	}

	template<class InputIterator, class Distance>
	inline void advance(InputIterator& i, Distance n) {
		typedef iterator_traits<InputIterator>::iterator_category category;
		return __advance(i, n, category());
	}
	/*--------------------------------------------------------------------------------------------------------------------*/
	/*--------------------------------------------------------------------------------------------------------------------*/
	template<class Container>
	class back_insert_iterator {
	protected:
		Container* container;
	public:
		typedef Container			container_type;
		typedef output_iterator_tag	iterator_category;
		typedef	void				value_type;
		typedef void				defference_type;
		typedef void				pointer;
		typedef void				reference;

		explicit back_insert_iterator(Container& x) :container(&x) {}
		back_insert_iterator<Container>& operator=(const typename Container::value_type& value) {
			container->push_back(value);
			return *this;
		}
		back_insert_iterator<Container>& operator*() { return *this; }
		back_insert_iterator<Container>& operator++() { return *this; }
		back_insert_iterator<Container>& operator++(int) { return *this; }
	};
	template<class Container>
	inline back_insert_iterator<Container> back_inserter(Container& x) {
		return back_insert_iterator<Container>(x);
	}

	template<class Container>
	class front_insert_iterator {
	protected:
		Contianer container;
	public:
		typedef Container			container_type;
		typedef output_iterator_tag	iterator_category;
		typedef void				value_type;
		typedef void				difference_type;
		typedef void				pointer;
		typedef void				reference;
		explicit front_insert_iterator(Container& x): container(&x) {}
		front_insert_iterator<Container>& operator=(const typename Container::value_type& value) {
			container->push_front(value);
			return *this;
		}
		front_insert_iterator<Container>& operator*() { return *this; }
		front_insert_iterator<Container>& operator++() { return *this; }
		front_insert_iterator<Container>& operator++(int) { return *this; }
	};
	template<class Container>
	inline front_insert_iterator<Container> front_insert(Container& x) {
		return front_insert_iterator<Container>(x);
	}

	template<class Container>
	class insert_iterator {
	protected:
		Container container;
		typename Container::iterator iter;
	public:
		typedef Container				container_type;
		typedef output_iterator_tag		iterator_category;
		typedef void					value_type;
		typedef void					difference_type;
		typedef void					pointer;
		typedef	void					reference;
		explicit insert_iterator(Container& x, typename Container::iterator& ite):contaienr(x), iter(ite) {}
		insert_iterator<Container>& operator=(typename Container::value_type& value){
			iter = container->insert(iter, value);
			++iter;
			return *this;
		}
		insert_iterator<Container>& operator*() { return *this; }
		insert_iterator<Container>& operator++() { return *this; }
		insert_iterator<Container>& operator++(int) { return *this; }

	};
	template<typename Container, typename Iterator>
	inline insert_iterator<Container> insert(Container& x，Iterator i) {
		typedef typename Container::iterator ite;
		return insert_iterator<Container>(x, ite(i));
	}



	template<typename Iterator>
	class reverse_iterator {
	protected:
		Iterator current;
	public:
		typedef typename iterator_traits<Iterator>::iterator_category	iterator_category;
		typedef typename iterator_traits<Iterator>::value_type			value_type;
		typedef typename iterator_traits<Iterator>::difference_type		difference_type;
		typedef typename iterator_traits<Iterator>::pointer				pointer;
		typedef typename iterator_traits<Iterator>::reference			reference;
		typedef Iterator iterator_type;
		typedef reverse_iterator<Iterator> self;
	public:
		reverse_iterator() {}
		explicit reverse_iterator(iterator_type x) :current(x) {}
		reverse_iterator(const self& x) :current(x.current) {}
		iterator_type base()const { return current; }
		reference operator*()const {
			iterator_type temp(current);
			return *(--temp);
		}
		pointer operator->()const {
			return &(operator*());
		}
		self& operator++() {
			--current;
			return *this;
		}
		self& operator++(int) {
			self temp = *this;
			--current;
			return *this;
		}
		self& operator--() {
			++current;
			return *this;
		}
		self& operator--(int) {
			self temp = *this;
			++current;
			return *this;
		}
		self operator+(difference_type n)const {
			return self(current - n);
		}
		self& operator+=(difference_type n) {
			current -= n;
			return *this;
		}
		self operator-(difference_type n) const {
			return self(current + n);
		}
		self& operator-=(difference_type n) {
			current += n;
			return *this;
		}
		reference operator[](difference_type n)const{
			return *(*this + n);
		}
	};
	template<typename Iterator>
	inline bool operator==(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
		return x.base() == y.base();
	}
	template<typename Iterator>
	inline bool operator!=(const reverse_iterator<Iterator>& x，const reverse_iterator<Iterator>& y) {
		return !(x == y);
	}
	template<typename Iterator>
	inline bool operator>(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {/*都使用< 减少对iterator的要求*/
		return y < x;
	}
	template<typename Iterator>
	inline bool operator>=(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
		return !(x < y);
	}
	template<typename Iterator>
	inline bool operator<(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
		return x.base() < y.base();
	}
	template<typename Iterator>
	inline bool operator<=(const reverse_iterator<Iterator>& x，const reverse_iterator<Iterator>& y) {
		return !(y < x);
	}
	template<typename Iterator>
	inline typename iterator_traits<Iterator>::differnce_type
	operator-(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
		return y.base() - x.base();
	}
	template<typename Iterator>
	inline reverse_iterator<Iterator>
	operator+(const reverse_iterator<Iterator>& x, const iterator_traits<Iterator>::difference_type n) {
		return reverse_iterator<x.base - n>;
	}





}

#endif