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
        verify(_last -1 ,_last);
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
        friend class vector<T,Alloc>;
        iterator(vector<T,Alloc>* _pvec = nullptr ,T* p = nullptr)
        : _ptr(p), _pvec(_pvec){
            Iterator_Base *itb = new Iterator_Base(this , _pvec->_head.next);
            _pvec->_head.next = itb;
        };

        bool operator!=(const iterator& it) const {
            if(_pvec == nullptr || _pvec != it._pvec){
                throw "iterator incompatable!";
            }
            return _ptr != it._ptr;
        }

        iterator &operator++(){
            if(_pvec == nullptr){
                throw "iterator invalid!";
            }
            ++_ptr;
            return *this;
        }

        T& operator*(){
            if(_pvec == nullptr){
                throw "iterator invalid!";
            }
            return *_ptr;
        }

        const T& operator*() const {
            return *_ptr;
        }
    private:
        T* _ptr;
        vector<T,Alloc> *_pvec;
    };

    iterator begin(){ return iterator(this , _first); }
    iterator end(){ return  iterator(this , _last); }

    iterator insert(iterator it , const T &val){
        /*
         * 不考虑扩容
         * 不考虑空指针 it._ptr 的合法性
         */
        verify(it._ptr - 1, _last);
        T* p = _last;
        while(p > it._ptr ){
            _allocater.construct(p,*(p -1 ));
            _allocater.destory(p - 1);
            --p;
        }
        _allocater.construct(p,val);
        _last++;
        return iterator(this , p);
    }

    iterator erase( iterator it ){
        verify(it._ptr - 1 , _last);
        T* p = it._ptr;
        while (p != _last - 1){
            _allocater.destory(p);
            _allocater.construct(p ,*(p + 1));
            ++p;
        }
        _allocater.destory(p);
        _last--;
        return iterator(this , it._ptr);
    }
private:
    T* _first;
    T* _last;
    T* _end;
    Alloc _allocater;

        struct Iterator_Base
        {
            Iterator_Base(iterator* c = nullptr, Iterator_Base *n = nullptr)
            : _cur(c) , next(n){}
            iterator *_cur;
            Iterator_Base* next;
        };
        Iterator_Base _head;

        //检查迭代器失效
        void verify(T *first , T *last) {
            Iterator_Base *pre = &this->_head;
            Iterator_Base *it = this->_head.next;
            while ( it != nullptr){
                if(it->_cur->_ptr > first &&  it->_cur->_ptr  <= last){
                    it->_cur->_pvec = nullptr ;
                    pre->next = it->next;
                    delete it;
                    it = pre->next;
                } else {
                    pre = it;
                    it = it->next;
                }
            }
        }


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
   vector<int> a(200);

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
        it = a.insert(it , *it - 1);
        ++it;
    }

    for(int v : a){
        cout << v << ' ';
    }

    cout << endl;



    return 0;
}
