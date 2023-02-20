#ifndef _HBTINYSTL_ALLOC_H_
#define _HBTINYSTL_ALLOC_H_

#include<stddef.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>

namespace HBtinySTL {
	template<int inst>
	class __malloc_alloc_template {
	private:
		static void* oom_malloc(size_t);
		static void* oom_realloc(void*, size_t);
		static void (*__malloc_alloc_oom_handler)();
	public:
		static void* allocate(size_t n) {
			void* result = malloc(n);
			if (0 == result) result = oom_malloc(n);
			return result;
		}
		static void deallocate(void* p, size_t /* n */) {
			free(p);
		}
		static void reallocate(void* p, size_t /*old_sz*/, size_t new_sz) {
			void* result = realloc(p, new_sz);
			if (0 == result) result = oom_realloc(p, new_sz);
			return result;
		}

		static void (*set_malloc_handler(void (*f)()))() {
			void (*old)() = __malloc_alloc_oom_handler;
			__malloc_alloc_oom_handler = f;
			return old;
		}
	};

	template <int inst>
	void* __malloc_alloc_template<inst>::oom_malloc(size_t n)
	{
		void (*my_malloc_handler)();
		void* result;
		for (;;) {
			my_malloc_handler = malloc_alloc_oom_handler;
			if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; }
			(*my_malloc_handler)();
			result = malloc(n);
			if (result) return result;
		}
	}
	template <int inst>
	void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n) {
		void (*my_realloc_handler)();
		void* result;
		for (;;) {
			my_realloc_handler = __malloc_alloc_oom_handler;
			if (0 == my_realloc_handler) { __THROW_BAD_ALLOC };
			(*my_realloc_handler)();
			result = realloc(p, n);
			if (result) return result;
		}
	}


	typedef __malloc_alloc_template<0> malloc_alloc;

	template<class T, class Alloc>
	class simple_alloc {
	public:
		static T* allocate(size_t n) {
			return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
		}
		static T* allocate(void) {
			return (T*)Alloc::allocate(sizeof(T));
		}
		static void deallocate(T* p, size_t n) {
			if (0 != n) Alloc::deallocate(p, n * sizeof(T));
		}
		static void deallocate(T* p) {
			Alloc::deallocate(p, sizeof(T));
		}
	};

#ifdef __USE_FIRST_ALLOC
	typedef malloc_alloc alloc;
	typedef malloc_alloc single_client_alloc;
#else

	enum { _ALIGN = 8 };
	enum { _MAX_BYTES = 128 };
	enum { _NFREELISTS = _MAX_BYTES / _ALIGN };

	template<bool threads, int inst>
	class __default_alloc_template {
	private:
		static size_t ROUND_UP(size_t bytes) {
			return ((bytes)+_ALIGN - 1) & ~(_ALIGN - 1);
		}
	private:
		union obj {
			union obj* free_list_link;
			char client_data[1];
		};
	private:
		static obj* free_list[_NFREELISTS];
		static size_t FREELIST_INDEX(size_t bytes) {
			return ((bytes)+_ALIGN - 1) / _ALIGN - 1;
		}

		static void* refill(size_t n);
		static char* chunk_alloc(size_t size, int& nobjs);

		static char* start_free;
		static char* end_free;
		static size_t heap_size;
	public:
		static void* allocate(size_t n);
		static void deallocate(void* p, size_t n);
		static void* reallocate(void* p, size_t old_sz, size_t new_sz);

	};

	template <bool threads, int inst>
	char* __default_alloc_template<threads, inst>::start_free = 0;

	template<bool threadss, int inst>
	char* __default_alloc_template<threads, inst>::end_free = 0;

	template<bool threads, int inst>
	char* __default_alloc_template<threads, inst>::heap_size = 0;

	template<bool threads, int inst>
	__default_alloc_template<threads, inst>::obj* volatile
		__default_alloc_template<threads, inst>::free_list[_NFREELISTS] =
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	template<bool threads, int inst>
	void* __default_alloc_template<threads, inst>::allocate(size_t n) {
		obj* volatile* my_free_list;
		obj* result;
		if (n > (size_t)_MAX_BYTES) {
			return (malloc_alloc::allocate(n));
		}
		my_free_list = free_list + FREELIST_INDEX(n);
		result = *my_free_list;
		if (0 == result) {
			void* r = refill(ROUND_UP(n));
			return r;
		}
		*my_free_list = result->free_list_link;
		return result;
	}
	template<bool threads, int inst>
	void* __default_alloc_template<threads, inst>::deallocate(void* p, size_t n) {
		obj* q = (obj*)p;
		obj* volatile* my_free_list;
		if (n > (size_t)_MAX_BYTES) {
			malloc_alloc::deallocate(p, n);
			return;
		}
		my_free_list = free_list + FREELIST_INDEX(n);

		q->free_list_link = my_free_list;
		*my_free_list = q;
	}
	template<bool threads, int inst>
	void* __default_alloc_template<threads, inst>::reallocate(void* p, size_t old_sz, size_t new_sz) {
		void* result;
		size_t copy_len;
		if (old_sz > (size_t)_MAX_BYTES && new_sz > (size_t)_MAX_BYTES) {
			return malloc_alloc::reallocate(p, old_sz, new_sz);
		}
		if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) return p;
		result = allocate(new_sz);
		copy_len = old_sz > new_sz ? new_sz : old_sz;
		memcpy(result, p, copy_len);
		deallocate(p, old_sz);
		return result;
	}

	template<bool threads, int inst>
	void* __default_alloc_template<threads, inst>::refill(size_t n) {
		int nobjs = 20;
		char* chunk = chunk_alloc(n, nobjs);
		obj* volatile* my_free_list;
		obj* result;
		obj* current_obj, * next_obj;
		int i;
		if (1 == nobjs) return (chunk);
		my_free_list = free_list + FREELIST_INDEX(n);
		result = (obj*)chunk;
		*my_free_list = next_obj = (obj*)(chunk + n);
		for (i = 1; ; i++) {
			current_obj = next_obj;
			next_obj = (obj*)((char*)next_obj + n);
			if (nobjs - 1 == i) {
				current_obj->free_list_link = 0;
				break;
			}
			else {
				current_obj->free_list_link = next_obj;
			}
		}
		return result;
	}

	template<bool threads, int inst>
	char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size, int& nobjs) {
		char* result;
		size_t total_bytes = size * nobjs;
		size_t bytes_left = end_free - start_free;

		if (bytes_left >= total_bytes) {
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else if (bytes_left >= size) {
			nobjs = bytes_left / size;
			total_bytes = nobjs * size;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else {
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			if (bytes_left > 0) {
				obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);
				((obj*)start_free)->free_list_link = my_free_list;
				*my_free_list = (obj*)start_free;
			}
			start_free = (char*)malloc(bytes_to_get);
			if (0 == start_free) {
				int i;
				obj* volatile* my_free_list, * p;
				for (int i = size; i <= _MAX_BYTES; i += _ALIGN) {
					my_free_list = free_list + FREELIST_INDEX[i];
					p = *my_free_list;
					if (0 != p) {
						*my_free_list = p->free_list_link;
						start_free = (char* p);
						end_free = start_free + i;
						return chunk_alloc(size, nobjs);
					}
				}
				end_free = 0;
				start_free = (char*)malloc_alloc::allocate(bytes_to_get);
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return chunk_alloc(size, nobjs);
		}

	}

	typedef __default_alloc_template<false, 0> alloc;
	typedef __default_alloc_template<false, 0> single_client_alloc;
}

#endif


#endif

