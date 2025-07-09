#ifndef MYSTRING_H
#define MYSTRING_H
#include <scoped_allocator>
#include <cstdint>
#include <cstdlib>

namespace MY_STD{

template <class Alloc = std::allocator<char>> 
class string{
private:

    void getSize(const char* str){
        for(size_t i = 0; str[i] != '\0'; i++){
            _size++;
        }
    }

    void copyStr(const char* str){
        size_t i = 0;

        try{
            for(; i < _size; i++){
                AllocTraits::construct(_alloc, _str + i, str[i]);
            }
            AllocTraits::construct(_alloc, _str + _size, '\0');

        }catch(...){
            for(size_t k = 0; k < i; k++)
                AllocTraits::destroy(_alloc, _str + k);
            
        }

    }
private:

    template <bool isConst>
    class base_iterator{
        private:
        using pointer_type = std::conditional_t<isConst, const char*, char*>;
        using reference_type = std::conditional_t<isConst, const char&, char&>;

        pointer_type _ptr;

        friend class string;

        public:
        base_iterator(char* str) : _ptr(str){} 

        reference_type operator*() const{return *(_ptr); }

        pointer_type operator->() const {return _ptr;}

        base_iterator& operator++(){
            _ptr++;
            return *this;
        }

        bool operator==(base_iterator& iter) const {
            return _ptr == iter._ptr; 
        }

        bool operator!=(base_iterator iter) const {
            return _ptr != iter._ptr;
        }

    };


public:

    using iterator = base_iterator<false> ;  
    using const_iterator = base_iterator<true>;  
    
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;



    string(){
        _str = nullptr;
        _size = 0;
        _cap = 0;
    }

    string(const char* str){
        _size = 0;
        _cap = 0;
        getSize(str);
        _cap =  2 * _size;
        
        _str = AllocTraits::allocate(_alloc, _cap);
        

        copyStr(str);
    }

    string(const string& other_str){
        _size = other_str._size;
        _cap = other_str._cap;

        _str = AllocTraits::allocate(_alloc, _cap);
        copyStr(other_str._str);
        
    }




    inline constexpr char* data() const {
        return _str;
    }

    inline constexpr size_t size() const noexcept{
        return _size;
    }

    inline constexpr size_t capacity() const noexcept {
        return _cap;
    }

    ~string(){
        if(_cap != 0){
            AllocTraits::deallocate(_alloc, _str, _cap);

        }
    }


    iterator begin(){return {_str};}

    iterator end(){ return {_str + _size};}

    const_iterator cbegin() const { return {_str};}

    const_iterator cend() const { return {_str + _size};}

private:
    char* _str;
    size_t _size;
    size_t _cap;

    Alloc _alloc;
    using AllocTraits = std::allocator_traits<Alloc>; 
};






}


#endif //MYSTRING_H
