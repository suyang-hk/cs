#include "iostream"
#include "ctime"
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

    T& operator[](int index){
        if ( index < 0 || index >= size())
            throw "the index is avalid";
            return _first[index];
    }

    class iterator {
    public:
        iterator(T* p = nullptr) : _ptr(p){};
        bool operator!=(const iterator& it) const {
            return _ptr != it._ptr;
        }

        iterator &operator++(){
            ++_ptr;
            return *this;
        }

        T& operator*(){
            return *_ptr;
        }

        const T& operator*() const {
            return *_ptr;
        }
    private:
        T* _ptr;

    };

    iterator begin(){ return iterator(_first); }
    iterator end(){ return  iterator(_last); }
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
    srand(time(0));
   vector<int> a;

    for (int i = 0; i < 20; ++i) {
        a.push_back(rand()%100 + 1);
    }

   vector<int>::iterator it = a.begin();
    int size = a.size();
    for( int i = 0; i < size; ++i){
        cout << a[i] << ".";
    }
    cout << endl;
    cout << "----------------------" << endl;

    for(; it != a.end(); ++it){
        cout << *it << '.';
    }

    cout << endl;



    return 0;
}
