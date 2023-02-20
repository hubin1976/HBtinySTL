#ifndef _HBTINYSTL_MEMORY_H_
#define _HBTINYSTL_MEMORY_H_

#include<algobase.h>
#include<alloc.h>
#include<construct.h>
#include<uninitialized.h>

namespace HBtinySTL {
	template<class T>
	pair<T*, ptrdiff_t> _get_temporary_buffer(ptrdiff_t len, T*) {
		if (len > ptrdiffer_t(INT_MAX / sizeof(T)))
			len = INT_MAX / sizeof(T);
		while (len > 0) {
			T* temp = (T*)malloc(sizeof(T) * (size_t)len);
			if (temp != 0)
				return pair<T*, ptrdiff_t>(temp, len);
			len /= 2;
		}
		return pair<T*, ptrdiff_t>((T*)0, 0);
	}
	template<class T>
	inline pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len, T*) {
		return _get_temporary_buffer(len, (T*)0);
	}
	template<class T>
	void return_temporary_buffer(T* p) {
		free(p);
	}
	template<typename ForwardIterator, class T>
	class temporary_buffer {
	private:
		ptrdiff_t	m_original_len;
		ptrdiff_t	m_len;
		T* m_buffer;
		void m_allocate_buffer() {
			m_original_len = m_len;
			m_buffer = 0;
			if (m_len > (ptrdiff_t)(INT_MAX / sizeof(T)))
				m_len = INT_MAX / sizeof(T);
			while (m_len > 0) {
				m_buffer = (T*)malloc(sizeof(T) * m_len);
				if (m_buffer)
					break;
				m_len /= 2;
			}
		}
		void m_initialize_buffer(const T&, __true_type) {}
		void m_initialize_buffer(const T& val, __false_type) {
			uninitialized_fill_n(m_buffer, m_len, val);
		}
	public:
		ptrdiff_t size()const { return m_len; }
		ptrdiff_t requested_size()const { return m_original_len; }
		T* begin() { return m_buffer; }
		T* end() { return m_buffer + m_len; }

		temporary_buffer(ForwardIterator first, ForwardIterator last) {
			typedef typename __type_traits<T>::has_trivial_default_constructor
				Trivial;
			try {
				m_len = 0;
				distance(first, last, m_len);
				m_allocate_buffer();
				if (m_len > 0)
					m_initialize_buffer(*first, Trivial());
			}
			catch (...) {
				free(m_buffer);
				m_buffer = 0;
				m_len = 0;
			}
		}
		~temporary_buffer() {
			destroy(m_buffer, m_buffer + m_len);
			free(m_buffer);
		}
	private:
		temporary_buffer(const temporary_buffer&) {}
		void operator=(const temporary_buffer&) {}
	};
	template<typename ForwardIterator, typename T>
	class Temporary_buffer :public temporary_buffer<ForwardIterator, T> {
		Temporary_buffer(ForwardIterator first, ForwardIterator last)
			:temporary<ForwardIterator, T>(first, last) {}
		~Temporary_buffer() {}
	};














	template<typename T>
	class auto_ptr {
	private:
		T* m_ptr;
	public:
		typedef T element_type;
		explicit auto_ptr(T* p = 0) : m_ptr(p) {}
		auto_ptr(auto_ptr& x) : m_ptr(x.release()) {}
		auto_ptr& operator=(auto_ptr& x) {
			if (&x != this) {
				delete m_ptr;
				m_ptr = x.release();
			}
			return *this;
		}
		~auto_ptr() { delete m_ptr; }
		T& operator*()const {
			return *m_ptr;
		}
		T* operator->() const {
			return m_ptr;
		}
		T* get()const {
			return m_ptr;
		}
		T* release() {
			T* temp = m_ptr;
			m_ptr = 0;
			return temp;
		}
		void reset(T* p = 0) {
			if (p != m_ptr) {
				delete m_ptr;
				m_ptr = p;
			}
		}
	};

}
#endif