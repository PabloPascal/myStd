#include <iostream>


template <typename T, typename Alloc = std::allocator<T>>
class list {

	struct BaseNode {
		BaseNode* next = nullptr;
		BaseNode* prev = nullptr;

		BaseNode() = default;
		BaseNode(BaseNode* next, BaseNode* prev) : next(next), prev(prev) {}

	};

	struct Node : BaseNode {
		T value;
		Node() = default;
		Node(const T& data) {
			value = data;
		}
		Node(const T& data, BaseNode* next, BaseNode* prev) : BaseNode(next, prev) {}
	};

	Node* head;
	BaseNode fakeNode;
	size_t _size;

private:

	template <bool isConst>
	class BaseIterator {
	public:

		using ptr_type = std::conditional_t<isConst, const Node*, Node*>;
		using reference_type = std::conditional_t<isConst, const Node&, Node&>;


	private:
		friend class list;
		ptr_type _ptr;

		BaseIterator(ptr_type node_ptr) : _ptr(node_ptr)
		{
		}

	public:
		BaseIterator(const BaseIterator&) = default;
		BaseIterator& operator=(const BaseIterator&) = default;

		T& operator*() {
			return _ptr->value;
		}

		bool operator!=(const BaseIterator& it) {
			if (_ptr != it._ptr) {
				return true;
			}
			else
				return false;
		}

		BaseIterator& operator++() {
			_ptr = static_cast<ptr_type>(_ptr->next);
			return *this;
		}

		BaseIterator& operator--() {
			_ptr = static_cast<ptr_type>(_ptr->prev);
			return *this;
		}

	};


public:
	using iterator = BaseIterator<false>;
	using const_iterator = BaseIterator<true>;

	list() :fakeNode{ &fakeNode, &fakeNode }, _size(0), head(nullptr) {}

public: //iterators

	iterator begin() {
		return { head };
	}

	iterator end() {
		return { static_cast<Node*>(&fakeNode) };
	}

public: //methods

	void push_front(const T& data) {
		_size++;

		if (head == nullptr) {
			head = new Node(data);
			head->next = static_cast<Node*>(&fakeNode);
			fakeNode.prev = head;
			return;
		}
		
		Node* newNode = new Node(head->value);
		newNode->prev = head;
		newNode->next = head->next;
		head->prev = newNode;
		head->next = newNode;
		head->value = data;
	}


	~list() {
		while (head != &fakeNode) {
			delete_head();
		}

	}

	void delete_head() {
		Node* tmp_head = head;
		head = static_cast<Node*>(head->next);

		delete tmp_head;
	}

	size_t size() {
		return _size;
	}

	Node* getHead() {
		return head;
	}


private:

};
