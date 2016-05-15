#pragma once
// The MIT License (MIT)
//
// Copyright (c) 2015 Howard Hinnant
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cstddef> // max_align_t
#include <assert.h>
//#include <iostream> //cout
#include <type_traits>
template <std::size_t alignment>
class ArenaHeap{
public:
    static auto constexpr alignv = alignment;
    ~ArenaHeap() { delete []ptr_; }
    ArenaHeap() noexcept : ptr_(nullptr) {}
    ArenaHeap(const ArenaHeap&) = delete;
    ArenaHeap& operator=(const ArenaHeap&) = delete;

    char* allocate(std::size_t n){
        ptr_ = new char[n + alignment-1];
        return (char*)(align_up((size_t)ptr_));
    }

    void deallocate(char* p, std::size_t ) noexcept{
        delete []ptr_;
        ptr_ = nullptr;
    }

    void* ptr(){return ptr_;}

    static std::size_t align_up(std::size_t n) noexcept
    {return (n + (alignment-1)) & ~(alignment-1);}

private:
    char *ptr_;
};


template <std::size_t N, std::size_t alignment>
class ArenaStack{
    alignas(alignment) char buf_[N];
    char* ptr_;
    ArenaHeap<alignment> ah_;
public:
    static auto constexpr alignv = alignment;
    void* ptr(){return buf_;}
    ~ArenaStack() {ptr_ = nullptr;}
    ArenaStack() noexcept : ptr_(buf_) {}
    ArenaStack(const ArenaStack&) = delete;
    ArenaStack& operator=(const ArenaStack&) = delete;

    char* allocate(std::size_t n);
    void deallocate(char* p, std::size_t n) noexcept;

    static constexpr std::size_t size() noexcept {return N;}
    std::size_t used() const noexcept {return static_cast<std::size_t>(ptr_ - buf_);}
    void reset() noexcept {ptr_ = buf_;}

private:

    bool pointer_in_buffer(char* p) noexcept
    {return buf_ <= p && p <= buf_ + N;}
};

template <std::size_t N, std::size_t alignment>
char* ArenaStack<N, alignment>::allocate(std::size_t n){
    assert(pointer_in_buffer(ptr_) && "ResAlloc has outlived ArenaStack");
    auto const aligned_n = ah_.align_up(n);
    if (static_cast<decltype(aligned_n)>(buf_ + N - ptr_) >= aligned_n){
        char* r = ptr_;
        ptr_ += aligned_n;
        return r;
    }

    return ah_.allocate(n);
}

template <std::size_t N, std::size_t alignment>
void ArenaStack<N, alignment>::deallocate(char* p, std::size_t n) noexcept{
    assert(pointer_in_buffer(ptr_) && "ResAlloc has outlived ArenaStack");
    if (pointer_in_buffer(p)){
        n = ah_.align_up(n);
        if (p + n == ptr_)
            ptr_ = p;
    }
    else
        ah_.deallocate(p,n);
}


template <class T,size_t alignN,
          class ArenaType = ArenaHeap<alignN> >
class ResAlloc{
public:
    using value_type = T;
   // using data_type = typename std::aligned_storage<alignN,alignN >::type;

private:
	size_t     elemSize_;
    size_t     extraSize_;
    ArenaType& a_;

public:
    ResAlloc(const ResAlloc&) = default;
    ResAlloc& operator=(const ResAlloc&) = delete;

    ResAlloc(ArenaType& a,size_t s=sizeof(T),size_t ex=0) noexcept
    : elemSize_(ArenaHeap<alignN>::align_up(s)),extraSize_(ex),a_(a)
    {}

    size_t elementSize(){return elemSize_;}

    template <class U>
    ResAlloc(const ResAlloc<U,alignN,ArenaType>& a) noexcept
        : elemSize_(ArenaHeap<alignN>::align_up(a.elemSize_)),
          extraSize_(a.extraSize_),
          a_(a.a_) {}

    template <class _Up> struct rebind {using other = ResAlloc<_Up,alignN,ArenaType>;};

    T* allocate(std::size_t n)
    {return reinterpret_cast<T*>(a_.allocate(n*elemSize_ + extraSize_));}

    void deallocate(T* p, std::size_t n) noexcept
    {a_.deallocate(reinterpret_cast<char*>(p), n*elemSize_);}

    template <class U,size_t alignk,class A> friend class ResAlloc;

    template <class AlT,
              class T1,
              class U ,
			  size_t alignK>
    friend bool operator==(const ResAlloc<T1,alignK,AlT>& x, const ResAlloc<U,alignK,AlT>& y) noexcept;
};

template <class AlT,class T,class U,size_t alignN>
inline bool
operator==(const ResAlloc<T,alignN,AlT>& x, const ResAlloc<U,alignN,AlT>& y) noexcept
{return  &x.a_ == &y.a_;}

template <class AlT,class T, class U,size_t alignN>
inline bool
operator!=(const ResAlloc<T,alignN,AlT>& x, const ResAlloc<U,alignN,AlT>& y) noexcept
{return !(x == y);}

