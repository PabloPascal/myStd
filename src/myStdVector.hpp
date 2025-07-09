#ifndef MYSTDVECTOR_hpp
#define MYSTDVECTOR_hpp

#include <iostream>
#include <cstdint>
#include <cstdlib>
/*
	vector
*/

namespace MY_STD{

template <typename T, typename Alloc = std::allocator<T>>
class vector{
private:

	T* _arr;
	size_t _size;
	size_t _cap;

	Alloc _alloc;
	using AllocTraits = std::allocator_traits<Alloc>;  

private:

	template <bool isConst>
	class base_iterator {
	public:
		using pointer_type = std::conditional_t<isConst, const T*, T*>;
		using reference_type = std::conditional_t<isConst, const T&, T&>;
		using value_type = T;

	private:
		friend class vector;
		pointer_type _ptr;
		base_iterator(T* ptr) : _ptr(ptr) {}

	public:
		base_iterator(const base_iterator&) = default;
		base_iterator& operator=(const base_iterator&) = default;

		reference_type operator*() const { return *_ptr; }
		pointer_type operator->() const { return _ptr; }

		base_iterator& operator++() {
			_ptr++;
			return *this;
		}

		base_iterator& operator++(int) {
			base_iterator copy = *this;
			++_ptr;
			return copy;
		}

		base_iterator& operator+(size_t k) {
			_ptr = _ptr + k;
			return *this;
		}

		bool operator!=(const base_iterator& other) {
			if (_ptr != other._ptr) return true;
			return false;
		}

		operator base_iterator<true>() const {
			return { _ptr };
		}

	};

public:

	using iterator = base_iterator<false>;
	using const_iterator = base_iterator<true>;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	iterator begin() {
		return { _arr };
	}

	iterator end() {
		return { _arr + _size };
	}

	const_iterator begin() const {
		return { _arr };
	}

	const_iterator end() const {
		return { _arr + _size };
	}

	const_iterator cbegin() const {
		return { _arr };
	}

	const_iterator cend() const {
		return { _arr + _size };
	}


	vector(): _size(0),_cap(0), _arr(nullptr) {}

	vector(std::initializer_list<T>& list) {
		_size = list.size();
		_cap = 2 * _size;
		_arr = reinterpret_cast<T*>(new char[_cap * sizeof(T)]);

		int k = 0;
		auto it = list.begin();
		for (; it != list.end(); it++) {
			new(_arr + k) T(*it);
			k++;
		}

	}
	vector(std::initializer_list<T>&& list) {
		_size = list.size();
		_cap = 2 * _size;
		_arr = reinterpret_cast<T*>(new char[_cap * sizeof(T)]);

		int k = 0;
		auto it = list.begin();
		for (; it != list.end(); it++) {
			new(_arr + k) T(*it);
			k++;
		}

	}

	vector(size_t sz) : _cap(sz), _size(0)
	{
		_arr = reinterpret_cast<T*>(new char[_cap * sizeof(T)]);

	}

	vector(const vector& other) {
		_cap = other._cap;
		_size = other._size;

		_arr = reinterpret_cast<T*>(new char[_cap * sizeof(T)]);

		for (size_t i = 0; i < _size; i++) {
			new(_arr + i) T(other._arr[i]);
		}

	}


	vector(vector&& other) noexcept
	{
		_cap = other._cap;
		_size = other._size;

		_arr = other._arr;
		other._arr = nullptr;
		other._cap = 0;
		other._size = 0;
	}


	void reserve(size_t new_cap) {
		if (new_cap < _cap) return;

		//T* new_arr = reinterpret_cast<T*>(new char[new_cap * sizeof(T)]);
		T* new_arr = AllocTraits::allocate(_alloc, new_cap);

		size_t i = 0;
		try {
			for (; i < _size; i++) {
				AllocTraits::construct(_alloc, new_arr + i, std::move_if_noexcept(_arr[i]));
			}
		}
		catch (...) {
			for (size_t j = 0; j < i; j++) {
				AllocTraits::destroy(_alloc, new_arr + j);
			}
			AllocTraits::deallocate(_alloc, new_arr, 0);
			throw;
		}
		for (size_t k = 0; k < _size; k++) {
			AllocTraits::destroy(_alloc, _arr + i);
		}

		AllocTraits::deallocate(_alloc, _arr, 0);

		_cap = new_cap;
		_arr = new_arr;
	}


	void push_back(T&& elem) {
		
		emplace_back(std::move(elem));

	}

	void push_back(const T& elem) {

		emplace_back(elem);

	}

	template <typename ...Args>
	void emplace_back(Args&&... args) {

		if (_size == _cap) {
			reserve(2 * _cap + 1);
		}

		AllocTraits::construct(_alloc, _arr + _size, std::forward<Args>(args)...);
		++_size;
	}


	T& operator[](size_t index) {
		return _arr[index];
	}

	size_t size() {
		return _size;
	}

	vector& operator=(const vector& other) {

		Alloc new_alloc = AllocTraits::propagate_on_container_copy_assignment::value
			? other._alloc : _alloc;


		T* new_arr = AllocTraits::allocate(new_alloc, other._cap);
		
		size_t i = 0;
		try {

			for (; i < other._size; i++) {
				AllocTraits::construct(new_alloc, new_arr + i, other._arr[i]);
			}

		}
		catch (...) {

			for (size_t j = 0; j < i; j++) {
				AllocTraits::destroy(new_alloc, new_arr + j);
			}

			AllocTraits::deallocate(new_alloc, new_arr, other._cap);

			throw;
		}

		for (size_t j = 0; j < _size; j++) {
			AllocTraits::destroy(new_alloc, _arr + j);
		}

		AllocTraits::deallocate(new_alloc, _arr, _cap);

		_alloc = other._alloc;
		_arr = new_arr;
		_cap = other._cap;
		_size = other._size;

		return *this;
	}


	vector& operator=(vector&& other) {

		Alloc new_alloc = AllocTraits::propagate_on_container_copy_assignment::value
			? other._alloc : _alloc;


		T* new_arr = AllocTraits::allocate(new_alloc, other._cap);

		size_t i = 0;
		try {

			for (; i < other._size; i++) {
				AllocTraits::construct(new_alloc, new_arr + i, std::move(other._arr[i]));
			}

		}
		catch (...) {

			for (size_t j = 0; j < i; j++) {
				AllocTraits::destroy(new_alloc, new_arr + j);
			}

			AllocTraits::deallocate(new_alloc, new_arr, other._cap);

			throw;
		}

		for (size_t j = 0; j < _size; j++) {
			AllocTraits::destroy(new_alloc, _arr + j);
		}

		AllocTraits::deallocate(new_alloc, _arr, _cap);

		_alloc = other._alloc;
		_arr = new_arr;
		_cap = other._cap;
		_size = other._size;

		other._arr = nullptr;
		other._size = 0;
		other._cap = 0;

		return *this;

	}




	T& at(size_t index) {
		if (index > _size || index < 0) {
			throw "index out of range";
		}
		else {
			return _arr[index];
		}

	}

	void clear() {

		for (size_t i = 0; i < _size; i++) {
			AllocTraits::destroy(_alloc, _arr + i);
		}

		AllocTraits::deallocate(_alloc,_arr, _cap);

		_size = 0;
		_cap = 0;
		_arr = nullptr;
	}


	T& back() {
		return _arr[_size - 1];
	}

	size_t capacity() {
		return _cap;
	}


	void insert(iterator& iter, std::initializer_list<T> lst) {
		size_t new_size = _size + lst.size();

		T* new_arr = AllocTraits::allocate(_alloc, new_size);

		size_t k = 0;
		try {

			iterator it = begin();
			
			for (; it != iter; it++) {
				AllocTraits::construct(_alloc, new_arr + k, *it);
				k++;
			}
			
			auto list_it = lst.begin();
			for (; list_it != lst.end(); list_it++) {
				AllocTraits::construct(_alloc, new_arr + k, *list_it);
				k++;
			}

			for (; k < new_size; k++) {
				AllocTraits::construct(_alloc, new_arr + k, *it);
				it++;
			}



		}
		catch (...) {
			
			for (size_t j = 0; j < k; j++) {
				AllocTraits::destroy(_alloc, new_arr + j);
			}

			AllocTraits::deallocate(_alloc, new_arr, new_size);

			throw;
		}

		

		clear();

		_arr = new_arr;
		_size = new_size;
		_cap = new_size;
	}




	/*
		destructor
	*/

	~vector() {

		if (_arr != nullptr) {

			for (size_t i = 0; i < _size; i++) {
				/*(_arr + i)->~T();*/
				AllocTraits::destroy(_alloc, _arr + i);
			}

			//delete[] reinterpret_cast<char*>(_arr);
			AllocTraits::deallocate(_alloc, _arr, _cap);
		}
	}
};


template <>
class vector<bool> {
	char* _arr;
	size_t _size;
	size_t _cap;

	struct bitReference {
		char* _cell;
		uint8_t _index;

		bitReference(char* cell, uint8_t index) : _cell(cell), _index(index) {}

		void operator=(bool b) {
			if (b) {
				*_cell |= (1 << _index);
			}
			else {
				*_cell &= ~(1 << _index);
			}
		}

		operator bool() const {
			return *_cell & (1 << _index);
		}
	};



public:

	vector(std::initializer_list<bool> list) {

		_size = list.size();
		_cap = 2 * _size;
		_arr = new char[_cap];


		auto it = list.begin();
		size_t k = 0;
		for (it; it != list.end(); it++) {
			new(_arr + k) char(*it);
			k++;
		}

	}

	bitReference operator[](size_t index) {
		return bitReference{ _arr + index / 8, index % 8 };

	};

	size_t size() {
		return _size;
	}

};

}

#endif
