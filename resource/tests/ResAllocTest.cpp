#include "../ResAlloc.h"
#include "../ResMemory.h"
#include <gtest/gtest.h>
#include <iostream>
using namespace std;

#define A16 16

struct alignas(A16) sA16{
    sA16(int aa):a(aa),b(0){}
    int a;
    char b;
};


TEST(ResAllocTest,vectorStack){
    using allocPolicy = ArenaStack<alignof(sA16)*3,alignof(sA16)>;
    using Allocator = ResAlloc<sA16,alignof(sA16),allocPolicy>;
    allocPolicy arena;
    Allocator alloc(arena);

    using SmallVector = std::vector<sA16,Allocator>;
    SmallVector v{alloc};
    v.reserve(3);

    // ensure first memory addr aligned by 16
    EXPECT_TRUE( (ptrdiff_t)arena.ptr() % A16 == 0 );

    v.push_back(1);
    v.emplace_back(1);
    v.push_back(1);

    //cout<<hex<<(ptrdiff_t)arena.ptr()<<endl;
    //cout<<hex<<(ptrdiff_t)&v[0]<<endl;

    // ensre element memory addr aligned by 16
    EXPECT_TRUE( (ptrdiff_t)&v[0] % A16 == 0);
    EXPECT_TRUE( (ptrdiff_t)&v[1] % A16 == 0);
    EXPECT_TRUE( (ptrdiff_t)&v[2] % A16 == 0);

    // arena first addr equal to vector first element addr
    EXPECT_TRUE( (ptrdiff_t)arena.ptr() == (ptrdiff_t)&v[0] );

    // every vector element addr aligned by 16
    EXPECT_TRUE( ((ptrdiff_t)&v[0] + A16 ) == (ptrdiff_t)&v[1]);
    EXPECT_TRUE( ((ptrdiff_t)&v[1] + A16 ) == (ptrdiff_t)&v[2]);
}

TEST(ResAllocTest,vectorHeap){
    using allocPolicy = ArenaHeap<alignof(sA16)>;
    using Allocator = ResAlloc<sA16,alignof(sA16),allocPolicy>;
    allocPolicy arena;
    Allocator alloc(arena);

    using SmallVector = std::vector<sA16,Allocator>;
    SmallVector v{alloc};
    v.reserve(4);

    // ensure first memory addr aligned by 16
    EXPECT_TRUE( (ptrdiff_t)arena.ptr() % A16 == 0 );

    v.push_back(1);
    v.emplace_back(1);
    v.push_back(1);

    //cout<<hex<<(ptrdiff_t)arena.ptr()<<endl;
    //cout<<hex<<(ptrdiff_t)&v[0]<<endl;

    // ensre element memory addr aligned by 16
    EXPECT_TRUE( (ptrdiff_t)&v[0] % A16 == 0);
    EXPECT_TRUE( (ptrdiff_t)&v[1] % A16 == 0);
    EXPECT_TRUE( (ptrdiff_t)&v[2] % A16 == 0);

    // arena first addr equal to vector first element addr
    //cout<<( (ptrdiff_t)arena.ptr() == (ptrdiff_t)&v[0] ) <<endl;

    // every vector element addr aligned by 16
    EXPECT_TRUE( ((ptrdiff_t)&v[0] + A16 ) == (ptrdiff_t)&v[1]);
    EXPECT_TRUE( ((ptrdiff_t)&v[1] + A16 ) == (ptrdiff_t)&v[2]);
}

TEST(ResAllocTest,floatArrayAligned16){
    SFloat16Pool pool(3,33);
    //cout<<hex<<(ptrdiff_t)&(pool.vec_[0])<<endl;
    //cout<<hex<<(ptrdiff_t)&(pool.vec_[1])<<endl;
    //cout<<hex<<(ptrdiff_t)pool.indexOf(1)<<endl;
    //cout<<dec<<"elesize:"<<pool.alloctor_.elementSize()<<endl;
    EXPECT_TRUE( (ptrdiff_t)pool.indexOf(0) % A16 == 0);
    EXPECT_TRUE( (ptrdiff_t)pool.indexOf(1) % A16 == 0);

}

static void deadfun(){
  using allocPolicy = ArenaHeap<alignof(sA16)>;
    using Allocator = ResAlloc<sA16,alignof(sA16),allocPolicy>;
    allocPolicy arena;
    Allocator alloc(arena);

    using SmallVector = std::vector<sA16,Allocator>;
    SmallVector v{alloc};
    v.reserve(20);

    SmallVector v2{alloc};
    v2.push_back(2);
    v2.push_back(3);
}

TEST(ResAllocTest,shareAlloc){
//    EXPECT_DEATH(deadfun(),"");
    deadfun();
}



