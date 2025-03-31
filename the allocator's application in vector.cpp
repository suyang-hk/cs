#include "iostream"
using namespace std;

template<typename T>
// 空间配置器
class Allocater {
public:
    T* allocate(size_t size){
        return (T* ) malloc(size*sizeof(T));
    }

    void deAllocate(void *p) {
        free(p);
    }

    void construct(T *p, const T &val){
        new (p) T(val);
    }

    void destory(T *p){
        p->~T();
    }

};

template<typename T , typename Alloc = Allocater<T>>
class vector{
public:

    vector(int size = 10 ){
        _first = _allocater.allocate(size);
        _last = _first;
        _end = _first + size;
    }

    ~vector(){
        for(T* p = _first ; p != _last ; ++p){
            _allocater.destory(p);
        }
        _allocater.deAllocate(_first);
        _first = _last = _end = nullptr;
    }

    vector(const vector<T> &rhs){
        int size = rhs._end - rhs._first;
        int len = rhs._last - rhs._first;
        _first = _allocater.allocate(size);
        _last = _first + len;
        _end = _first + size;
        for( int i = 0; i < len ; ++i){
            _allocater.construct(_first + i , std::move(rhs._first[i]));
        }
    }

    vector<T>& operator=(const vector<T> &rhs){
        if(this == &rhs){
            return *this;
        }
        for(T* p = _first ; p != _last ; ++p){
            _allocater.destory(p);
        }
        _allocater.deAllocate(_first);
        int size = rhs._end - rhs._first;
        int len = rhs._last - rhs._first;
        _first = _allocater.allocate(size);
        _last = _first + len;
        _end = _first + size;
        for( int i = 0; i < len ; ++i){
            _allocater.construct(_first + i , std::move(rhs._first[i]));
        }
        return *this;
    }

    void push_back(const T& rhs){
        if(full()){
            expand();
        }
        _allocater.construct(_last, rhs);
        _last++;
    }

    void pop_back(){

        if(empty()){
            return ;
        }

        _last--;
        _allocater.destory(_last);

    }

    T back() const{
        return *(_last - 1);
    }

    bool full() const{
        return _last == _end;
    }

    bool empty() const{
        return _first == _last;
    }

    int size() const{
        return _last - _first;
    }

private:
    T* _first;
    T* _last;
    T* _end;
    Alloc _allocater;
    void expand() {
        int size = _last - _first;
        T* ptmp = _allocater.allocate(2*size);
        for( int i = 0; i < size ; ++i){
            _allocater.construct(ptmp + i,_first[i]);
        }

        for(T* p = _first ; p != _last; ++p){
            _allocater.destory(p);
        }
        _allocater.deAllocate(_first);
        _first = ptmp;
        _last = _first + size;
        _end = _first + 2*size;
    }
};

int main(){
   vector<int> a;
    for(int i = 0;i < 10 ; ++i){
        a.push_back(i);
    }
    for(int i = 0;i < 10 ; ++i){
        std::cout << a.back() << std::endl;
        a.pop_back();
    }


    return 0;
}
