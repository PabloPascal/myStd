
#include <cstdlib>

/*
	Allocators
*/


#include <scoped_allocator>

template <typename Alloc>
struct scoped_allocator_adaptor {

	Alloc alloc;

	template <typename T, typename ...Args>
	void construct(T& ptr, Args&&... args) {
		if constexpr (std::uses_allocator_v<T, Alloc>) {
			using inner_alloc = typename T::allocator_type;
			alloc.construct(ptr, std::forward<Args>(args)..., inner_alloc(alloc));
		}
		else {
			alloc.construct(ptr, std::forward<Args>(args)...);

		}
	}

};




template <typename Alloc>
struct AllocatorTraits {

	Alloc _alloc;

	template <typename U, typename ...Args>
	static void construct(Alloc& alloc, U* ptr, Args&... args) {
		//if constexpr (/*allocate has method construct*/) {
		//	alloc.construct(ptr, args...);
		//}
		new (ptr) U(args...);
	}

};


template <typename T>
struct allocator {

	T* allocate(size_t count) {
		return operator new(count * sizeof(T));
		//return reinterpret_cast<T*>(new char[count * sizeof(T)]);
	}

	void deallocate(T* ptr, size_t) {

		operator delete[](ptr);

	}

	template <typename U, typename ...Args>
	void construct(U* ptr, Args&& ...args) {

		new(ptr) U(std::forward<Args>(args)...);

	}

	void destroy(T* ptr) {
		ptr->~T();
	}
};





template <typename T, size_t N>
class StackAllocator
{
public:
	using value_type = T;

	StackAllocator() noexcept
	 : buffer(reinterpret_cast<T*>(storage)), current(buffer)
	{}

	StackAllocator(const StackAllocator& other) noexcept
	 : buffer(reinterpret_cast<T*>(storage)), current(buffer + (other.current - other.buffer))
	{}
	

	size_t max_size() const noexcept
	{
		return N;
	}

	template <typename U>
	struct rebind
	{
		using other = StackAllocator<U, N>;
	};

	T* allocate(size_t n)
	{
		if(current + n > buffer + N ) {
			throw std::bad_alloc();
		}

		T* ptr = current;
		current += n;

		return ptr;
	}


	void deallocate(T* buffer, size_t n){

	}

private:
	alignas(alignof(T)) char storage[N * sizeof(T)];
	T* buffer;
	T* current;


};

