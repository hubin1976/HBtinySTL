#ifndef _HBTINYSTL_QUEUE_H_
#define _HBTINYSTL_QUEUE_H_
namespace HBtinySTL {
    template <class T, class Sequence>
    class queue {

        typedef typename Sequence::value_type Sequence_value_type;

        friend bool operator==(const queue&, const queue&);
        friend bool operator<(const queue&, const queue&);

    public:
        typedef typename Sequence::value_type      value_type;
        typedef typename Sequence::size_type       size_type;
        typedef          Sequence                  container_type;

        typedef typename Sequence::reference       reference;
        typedef typename Sequence::const_reference const_reference;
    protected:
        Sequence c;
    public:
        queue() : c() {}
        explicit queue(const Sequence& __c) : c(__c) {}

        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }
        reference front() { return c.front(); }
        const_reference front() const { return c.front(); }
        reference back() { return c.back(); }
        const_reference back() const { return c.back(); }
        void push(const value_type& __x) { c.push_back(__x); }
        void pop() { c.pop_front(); }
    };

    template <typename T, typename Sequence>
    bool
        operator==(const queue<T, Sequence>& __x, const queue<T, Sequence>& __y)
    {
        return __x.c == __y.c;
    }

    template <typename T, typename Sequence>
    bool
        operator<(const queue<T, Sequence>& __x, const queue<T, Sequence>& __y)
    {
        return __x.c < __y.c;
    }


    template<typename T, typename Sequence = vector<T>, typename Compare = less<typename Sequence::value_type>>
    class priority_queue {
    public:
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference referrnce;
        typedef typename Sequence::const_reference const_reference;
    protected:
        Sequence c;
        Compare comp;
    public:
        priority_queue() :c() {}
        explicit priority_queue(const Compare& x) :c(), comp(x) {}

        priority_queue(const _Compare& x, const _Sequence& s)
            : c(s), comp(x)
        {
            make_heap(c.begin(), c.end(), comp);
        }

        priority_queue(const value_type* first, const value_type* last)
            : c(first, last) {
            make_heap(c.begin(), c.end(), comp);
        }

        priority_queue(const value_type* first, const value_type* last,
            const Compare& x)
            : c(first, last), comp(x)
        {
            make_heap(c.begin(), c.end(), comp);
        }

        priority_queue(const value_type* __first, const value_type* __last,
            const Compare& x, const Sequence& c)
            : c(c), comp(x)
        {
            c.insert(c.end(), __first, __last);
            make_heap(c.begin(), c.end(), comp);
        }

        bool empty() const { return c.empty() };
        size_type size() const { return c.size(); }
        const_reference top() const { return c.front(); }
        void push(const value_type& x) {
            try {
                c.push_back(x);
                push_heap(c.begin(), c.end(), comp);
            }
            catch (...) {
                c.clear();
            }
        }
        void pop() {
            try {
                pop_heap(c.begin(), c.end(), comp);
                c.pop_back();
            }
            catch (...) {
                c.clear();
            }
        }


    };



}

#endif