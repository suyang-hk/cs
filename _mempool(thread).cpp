#include<iostream>
#include <cstdlib>
#include<cstring>
#include<mutex>
#include<atomic>
#include<thread>
//simple spinlock
class _Spin_lock
{
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    public:
        void lock()
        {
            //buzy wait
            int spins = 0;
            while (flag.test_and_set(std::memory_order_acquire))
            {
                if(++spins > 100)
                    std::this_thread::yield();
            }
        }

        void unlock()
        {
            flag.clear(std::memory_order_release);
        }
};

template <int inst>
class _malloc_alloc_template 
{
    public:
        static void* allocate(size_t _n)
        {
            void *_result = malloc(_n);
            //内存分配失败时等会再考虑
            return _result;
        }

        static void deallocate(void* _p, size_t /* __n */)
        {
            free(_p);
        }

        static void* reallocate(void* _p, size_t /*_old_sz*/, size_t _new_sz)
        {
            void *_result = realloc(_p, _new_sz);
            return _result;
        }
};
using malloc_alloc = _malloc_alloc_template<0>;

template <bool __threads, int __inst>
class default_alloc_template
{
    private:
        enum {_ALIGN = 8};
        enum {_MAX_BYTES = 128};
        enum {_NFREELIST = 16};
        static _Spin_lock _S_spinlock;

        struct Lock
        {
            Lock()  { if (__threads) _S_spinlock.lock(); }
            ~Lock() { if (__threads) _S_spinlock.unlock(); }
        };

        union _Obj {
            union _Obj *_Next;
            char _Client_data[1];
        };

        static char *_S_start_free;
        static char *_S_end_free;
        static size_t _S_heap_size;

        static _Obj *volatile _S_free_list[_NFREELIST];
        static size_t _S_round_up(size_t _bytes)
        {
            return (((_bytes) + (size_t) _ALIGN - 1) & ~((size_t) _ALIGN - 1));
        }
        static size_t _S_freelist_index(size_t _bytes)
        {
            return (((_bytes) + (size_t)_ALIGN - 1) / (size_t)_ALIGN - 1); 
        }

        static char* _S_chunk_alloc(size_t __size, int& __nobjs);
        static void *_S_refill(size_t _n);


    public:
        static void* allocate(size_t _n)
        {
            void* _ret = 0;

            if (_n > (size_t)_MAX_BYTES)
            {
                _ret = malloc_alloc::allocate(_n);
            }
            else
            {
                Lock lock;
                _Obj *volatile *_my_free_list = _S_free_list + _S_freelist_index(_n);
                _Obj *_result = *_my_free_list;
                if(_result == 0)
                    _ret = _S_refill(_S_round_up(_n));
                else{
                    *_my_free_list = _result->_Next;
                    _ret = _result;
                }
            }
            return _ret;
        }
        static void deallocate(void* _p, size_t _n)
        {
            if (_n > (size_t)_MAX_BYTES)
                malloc_alloc::deallocate(_p, _n);
            else {
                Lock lock;  
                _Obj *volatile *_my_free_list = _S_free_list + _S_freelist_index(_n);
                _Obj *_q = (_Obj *)_p;
                //don't think about thread
                _q->_Next = *_my_free_list;
                *_my_free_list = _q;
            }
        }
        static void* reallocate(void* _p, size_t _old_sz, size_t _new_sz){
            void *_result;
            size_t _copy_sz;

            if(_old_sz > (size_t)_MAX_BYTES && _new_sz > (size_t)_MAX_BYTES)
            {
                return realloc(_p, _new_sz);
            }
            if (_S_round_up(_old_sz) == _S_round_up(_new_sz))
                return _p;
            _result = allocate(_new_sz);
            _copy_sz = _new_sz > _old_sz ? _old_sz : _new_sz;
            memcpy(_result, _p, _copy_sz);
            deallocate(_p, _old_sz);
            return (_result);
        }
    };

//初始化    
template <bool __threads, int __inst>
char* default_alloc_template<__threads, __inst>::_S_start_free = 0;

template <bool __threads, int __inst>
char* default_alloc_template<__threads, __inst>::_S_end_free = 0;

template <bool __threads, int __inst>
size_t default_alloc_template<__threads, __inst>::_S_heap_size = 0;

template <bool __threads, int __inst>
typename default_alloc_template<__threads, __inst>::_Obj * volatile
default_alloc_template<__threads, __inst>::_S_free_list[_NFREELIST] = {0};

template <bool __threads, int __inst>
_Spin_lock default_alloc_template<__threads, __inst>::_S_spinlock;



int main()
{
    //testing
    using alloc = default_alloc_template<false, 0>;

    // 申请 16 字节
    void* p1 = alloc::allocate(16);
    std::cout << "p1 = " << p1 << std::endl;

    // 释放
    alloc::deallocate(p1, 16);

    // 申请 64 字节
    void* p2 = alloc::allocate(64);
    std::cout << "p2 = " << p2 << std::endl;

    // 重新分配
    void* p3 = alloc::reallocate(p2, 64, 100);
    std::cout << "p3 = " << p3 << std::endl;

    alloc::deallocate(p3, 100);

    return 0;
}

template <bool _threads, int _inst>
        char*default_alloc_template<_threads, _inst>::_S_chunk_alloc(size_t _size, int& _nobjs)
        {
            char *_result;
            size_t _total_bytes = _size * _nobjs;
            size_t _bytes_left = _S_end_free - _S_start_free;

            if(_bytes_left >= _total_bytes){
                _result = _S_start_free;
                _S_start_free += _total_bytes;  
                return (_result);
            }else if(_bytes_left >= _size)  
            {
                _nobjs = (int)(_bytes_left / _size);
                _total_bytes = _size * _nobjs;
                _result = _S_start_free;
                _S_start_free += _total_bytes;
                return (_result);
            } else {
                size_t _bytes_to_get = 2 * _total_bytes + _S_round_up(_S_heap_size >> 4);
                if(_bytes_left > 0){
                    _Obj *volatile *_my_free_list = _S_free_list + _S_freelist_index(_bytes_left);
                    ((_Obj *)_S_start_free)->_Next = *_my_free_list;
                    *_my_free_list = (_Obj *)_S_start_free;
                }

                if(__threads)
                    _S_spinlock.unlock();
                
                //_S_start_free = (char *)malloc(_bytes_to_get);
                char *_new_start = static_cast<char *>(std::malloc(_bytes_to_get));

                if(__threads)
                    _S_spinlock.lock();

                if (0 == _new_start){
                    size_t _i;
                    _Obj *volatile *_my_free_list;
                    _Obj *_p;
                    for (_i = _size; _i <= (size_t)_MAX_BYTES; _i += (size_t)_ALIGN)
                    {
                        _my_free_list = _S_free_list + _S_freelist_index(_i);
                        _p = *_my_free_list;
                        if(0 != _p){
                            *_my_free_list = _p->_Next;
                            _S_start_free = (char *)_p;
                            _S_end_free = _S_start_free + _i;
                            return (_S_chunk_alloc(_size, _nobjs));
                        }
                    }
                    _S_end_free = 0;
                    _S_start_free =  static_cast<char*>(malloc_alloc::allocate(_bytes_to_get));
                    if (_S_start_free == nullptr) 
                    {
                    // 最后依赖系统分配失败，返回 nullptr
                        return nullptr;
                    }
                }else{
                        _S_start_free = _new_start;
                    }
                _S_heap_size += _bytes_to_get;
                _S_end_free = _S_start_free + _bytes_to_get;
                return (_S_chunk_alloc(_size, _nobjs));
            }
        }


template <bool _threads, int _inst>
        void*default_alloc_template<_threads, _inst>::_S_refill(size_t _n)
        {
            int _nobjs = 20;
            char *_chunk = _S_chunk_alloc(_n, _nobjs);
            _Obj *volatile *_my_free_list;
            _Obj *_result;
            _Obj *_current_obj;
            _Obj *_next_obj;
            int _i;

            if(_nobjs == 1)
                return (_chunk);
            _my_free_list = _S_free_list + _S_freelist_index(_n);
            _result = (_Obj *)_chunk;
            *_my_free_list = _next_obj = (_Obj *)(_chunk + _n);
            for (_i = 1;; ++_i){
                _current_obj = _next_obj;
                _next_obj = (_Obj *)((char *)_next_obj + _n);
                if (_nobjs - 1 == _i){
                    _current_obj->_Next = 0;
                    break;
                } else{
                    _current_obj->_Next = _next_obj;
                }
            }
            return _result;
        }
