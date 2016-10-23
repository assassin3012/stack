#pragma once
#include <iostream>

template <typename T>
class allocator
{
protected:
    allocator(size_t size = 0);
    ~allocator();
    auto swap(allocator & other) -> void;
    
    allocator(allocator const &) = delete;
    auto operator =(allocator const &) -> allocator & = delete;
    
    T * ptr_;
    size_t size_;
    size_t count_;
};

//-----------------------------------------------------------------------------------------------------------------------------------------

template <typename T>
class stack : private allocator<T>
{
public:
	stack(size_t size = 0); /*noexcept*/
	stack(const stack<T> & st); /*strong*/
	~stack(); /*noexcept*/
	size_t count() const noexcept; /*noexcept*/
	bool empty() const noexcept; /*noexcept*/
	void push(T const & el); /*strong*/
	stack<T> & operator = (stack<T> & st); /*strong*/
	void pop(); /*strong*/
	const T& top() const; /*strong*/
};

//-----------------------------------------------------------------------------------------------------------------------------------------

template <typename T1, typename T2>
void construct(T1 * ptr, T2 const & value) {
    new(ptr) T1 (value);
}

template <typename T>
void destroy(T * ptr) noexcept
{
    ptr->~T();
}

template <typename FwdIter>
void destroy(FwdIter first, FwdIter last) noexcept
{
    for (; first != last; ++first) {
        destroy(&*first);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------
	
template <typename T>
allocator<T>::allocator(size_t s) : ptr_(static_cast<T *>(s != 0 ? operator new(s * sizeof(T)) : nullptr)),
	size_(0), count_(s) {};

template<typename T>
allocator<T>::~allocator() { operator delete(ptr_); }

template <typename T>
auto allocator<T>::swap(allocator& s) -> void {
	std::swap(s.ptr_, ptr_);
	std::swap(s.count_, count_);
	std::swap(s.size_, size_);
};

//--------------------------------------------------------------------------------------------------------------------------------------

template<typename T>
stack<T>::stack(size_t s) : allocator<T>(s) {}

template<typename T> 
stack<T>::~stack() { destroy(this->ptr_, this->ptr_ + this->count_); }

template <typename T>
bool stack<T>::empty() const noexcept { return (this->count_ == 0); }

template <typename T>
stack<T>::stack(const stack& st) : allocator<T>(st.size_) {
	if (st.count_ != 0) {
		stack<T> ar(st.size_);
		for (size_t t = 0; t < st.count_; ++t) construct(ar.ptr_ + t, st.ptr_[t]);
		std::swap(ar.ptr_, this->ptr_);
	}
	this->count_ = st.count_;
};

template <typename T> /*noexcept*/
size_t stack<T>::count() const noexcept { return this->count_; }

template <typename T> /*strong*/
void stack<T>::push(T const & el) {
	if (this->size_ == this->count_) {
		if (this->size_ == 0) { this->size_ = 1; }
		size_t size = this->size_ * 2;
		stack<T> ar(size);
		for (size_t t = 0; t < this->count_; ++t) {
			construct(ar.ptr_ + t, this->ptr_[t]);
		}
		std::swap(ar.ptr_, this->ptr_);
		this->size_ = size;
	}
	construct(this->ptr_ + this->count_, el);
        ++this->count_;
}

template <typename T> /*strong*/
stack<T> & stack<T>::operator = (stack<T> & st) {
	if (this != &st) {
		(stack(st)).swap(*this);
	}
	return *this;
}

template <typename T> /*strong*/
void stack<T>::pop() {
	if (this->count_ != 0) {
		--this->count_;
	}
	else throw;
}

template <typename T> /*strong*/
const T& stack<T>::top() const {
	if (this->count_ != 0) {
		return this->ptr_this->count_ - 1];
	}
	else throw;
}
