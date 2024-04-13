#pragma once
#include <cstddef>
#include <initializer_list>
#include <cassert>

template <typename Type>
class SingleLinkedList {
	// Узел списка
	struct Node {
		Node() = default;
		Node(const Type& val, Node* next)
			: value(val)
			, next_node(next) {
		}
		Type value;
		Node* next_node = nullptr;
	};

	template <typename ValueType>
	class BasicIterator {
		friend class SingleLinkedList;

		explicit BasicIterator(Node* node) : node_(node) { }
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = Type;
		using difference_type = std::ptrdiff_t;
		using pointer = ValueType*;
		using reference = ValueType&;

		BasicIterator() = default;
		BasicIterator(const BasicIterator<Type>& other) noexcept : node_(other.node_) { }
		BasicIterator& operator=(const BasicIterator& rhs) = default;

		[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
			return this->node_ == rhs.node_;
		}
		[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
			return !(this->node_ == rhs.node_);
		}
		[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
			return this->node_ == rhs.node_;
		}
		[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
			return !(this->node_ == rhs.node_);
		}

		BasicIterator& operator++() noexcept {
			node_ = node_->next_node;
			return *this;
		}
		BasicIterator operator++(int) noexcept {
			auto old_value(*this);
			++(*this);
			return old_value;
		}

		[[nodiscard]] reference operator*() const noexcept {
			return node_->value;
		}
		[[nodiscard]] pointer operator->() const noexcept {
			return &node_->value;
		}
	private:
		Node* node_ = nullptr;
	};
public:
	using value_type = Type;
	using reference = value_type&;
	using const_reference = const value_type&;

	using Iterator = BasicIterator<Type>;
	using ConstIterator = BasicIterator<const Type>;

	SingleLinkedList() {
		head_.next_node = nullptr;
	}
	SingleLinkedList(const SingleLinkedList& other) {
		assert(size_ == 0 && head_.next_node == nullptr);
		SingleLinkedList tmp;
		for (auto obj : other) {
			tmp.PushBack(obj);
		}
		swap(tmp);
	}
	SingleLinkedList(std::initializer_list<Type> values) {
		for (Type value : values) {
			this->PushBack(value);
		}
	}
	SingleLinkedList& operator=(const SingleLinkedList& rhs) {
		SingleLinkedList tmp = rhs;
		this->swap(tmp);
		return *this;
	}
	~SingleLinkedList() {
		Clear();
	}

	[[nodiscard]] Iterator before_begin() noexcept {
		return Iterator{ &head_ };
	}
	[[nodiscard]] ConstIterator before_begin() const noexcept {
		return ConstIterator{ const_cast<Node*>(&head_) };
	}
	[[nodiscard]] ConstIterator cbefore_begin() const noexcept {
		return before_begin();
	}

	[[nodiscard]] Iterator begin() noexcept {
		return Iterator{ head_.next_node };
	}
	[[nodiscard]] ConstIterator begin() const noexcept {
		return ConstIterator{ head_.next_node };
	}
	[[nodiscard]] ConstIterator cbegin() const noexcept {
		return begin();
	}

	[[nodiscard]] Iterator end() noexcept {
		return Iterator{ nullptr };
	}
	[[nodiscard]] ConstIterator end() const noexcept {
		return ConstIterator{ nullptr };
	}
	[[nodiscard]] ConstIterator cend() const noexcept {
		return end();
	}

	void PushFront(const Type& value) {
		head_.next_node = new Node(value, head_.next_node);
		++size_;
	}

	void PushBack(const Type& value) {
		Node* newNode = new Node(value, nullptr);
		if (head_.next_node == nullptr) {
			head_.next_node = newNode;
		}
		else {
			Node* lastNode = head_.next_node;
			while (lastNode->next_node != nullptr) {
				lastNode = lastNode->next_node;
			}
			lastNode->next_node = newNode;
		}
		++size_;
	}

	void PopFront() noexcept {
		auto next_node = head_.next_node->next_node;
		delete head_.next_node;
		head_.next_node = next_node;
		--size_;
	}

	Iterator InsertAfter(ConstIterator pos, const Type& value) {
		assert(pos.node_ != nullptr);

		pos.node_->next_node = new Node(value, pos.node_->next_node);
		++size_;
		return Iterator{ pos.node_->next_node };
	}

	Iterator EraseAfter(ConstIterator pos) noexcept {
		assert(!IsEmpty());
		assert(pos.node_ != nullptr);

		auto next_node = pos.node_->next_node->next_node;
		delete pos.node_->next_node;
		pos.node_->next_node = next_node;
		--size_;
		return Iterator{ pos.node_->next_node };
	}

	void swap(SingleLinkedList& other) noexcept {
		std::swap(other.head_.next_node, this->head_.next_node);
		std::swap(other.size_, this->size_);
	}

	void Clear() noexcept {
		while (head_.next_node != nullptr) {
			Node* next_node = head_.next_node->next_node;
			delete head_.next_node;
			head_.next_node = next_node;
			--size_;
		}
	}

	[[nodiscard]] size_t GetSize() const noexcept {
		return size_;
	}
	[[nodiscard]] bool IsEmpty() const noexcept {
		return size_ == 0;
	}

private:
	Node head_;
	size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
	lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return (lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()));
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return rhs < lhs;
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs > rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs < rhs);
}