#ifndef MY_MEMORY_H
#define MY_MEMORY_H

#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <utility>

namespace MY_STD{


template <typename T>
class shared_ptr{
private:    
    T* _ptr = nullptr;

    struct ControlBlock{
        mutable size_t reference_count;
    };

    ControlBlock* ctrl_block;

    // template <typename U, typename ...Args>
    // friend shared_ptr<U> make_shared(Argss... args);

private:

    // shared_ptr(ControlBlock* control_block) : ctrl_block(control_block)
    // {}

public:

    shared_ptr() noexcept = default;
    //constexpr shared_ptr(nullptr_t) noexcept {};
    shared_ptr(T* ptr) : _ptr(ptr){
        ctrl_block = new ControlBlock{1};
    }
    
    shared_ptr(const shared_ptr& s_ptr){

        s_ptr.ctrl_block->reference_count++;
        ctrl_block = s_ptr.ctrl_block;

        _ptr = s_ptr._ptr;
        
    }

    size_t use_count() const noexcept {return ctrl_block->reference_count;}

    T& operator*() const noexcept { return *_ptr;}

    T* operator->() const noexcept {return _ptr;}

    shared_ptr& operator=(const shared_ptr& _other){

        delete _ptr;
        delete ctrl_block;
        
        _ptr = _other._ptr;
        _other.ctrl_block->reference_count++;
        ctrl_block = _other.ctrl_block;

        return *this;
    }

    ~shared_ptr(){
        std::cout << "~shader_ptr"<< std::endl;
        
        if(ctrl_block->reference_count==0 && _ptr != nullptr){
            delete _ptr;
            delete ctrl_block;
        }else{
            ctrl_block->reference_count--;
            
        }


    }

};


template<typename T, typename ...Args>
    shared_ptr<T> make_shared(Args&&... args){
        return shared_ptr<T>(new T(std::forward<Args>(args)...) );
}




template <typename T>
class unique_ptr{

    T* _ptr;

public:
    unique_ptr(): _ptr(nullptr) {}

    unique_ptr(T* ptr) : _ptr(ptr){}

    unique_ptr(const unique_ptr&) = delete;

    unique_ptr& operator=(const unique_ptr&) = delete;

    T& operator*(){ return *_ptr;}

    T* operator->() {return _ptr;}

    ~unique_ptr(){
        delete _ptr;
    }

};


template<typename T, typename ...Args>
    shared_ptr<T> make_unique(Args&&... args){
        return unique_ptr<T>(new T(std::forward<Args>(args)...) );
}


}


#endif
