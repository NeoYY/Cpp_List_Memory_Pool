#include <iostream>   // std::cout, std::endl
#include <cassert>    // assert()
#include <ctime>      // clock()
#include <vector>     // std::vector

#include "MemoryPool.hpp"  // MemoryPool<T>
#include "StackAlloc.hpp"  // StackAlloc<T, Alloc>

// Number of insert elements
#define ELEMS 10000000
// Repeated times
#define REPS 100

int main()
{
    clock_t start;
/*
    // use STL default allocator
    StackAlloc<int, std::allocator<int> > stackDefault;
    start = clock();
    for (int j = 0; j < REPS; j++) {
        assert(stackDefault.empty());
        for (int i = 0; i < ELEMS; i++)
          stackDefault.push(i);
        for (int i = 0; i < ELEMS; i++)
          stackDefault.pop();
    }
    std::cout << "Default Allocator Time: ";
    std::cout << (((double)clock() - start) / CLOCKS_PER_SEC) << "\n\n";

// 比较内存池和 std::vector 之间的性能
    std::vector<int> stackVector;
    start = clock();
    for (int j = 0; j < REPS; j++) {
        assert(stackVector.empty());
        for (int i = 0; i < ELEMS; i++)
          stackVector.push_back(i);
        for (int i = 0; i < ELEMS; i++)
          stackVector.pop_back();
    }
    std::cout << "Vector Time: ";
    std::cout << (((double)clock() - start) / CLOCKS_PER_SEC) << "\n\n";
*/
    // Use Memory Pool
    StackAlloc<int, MemoryPool<int> > stackPool;
    start = clock();
    for (int j = 0; j < REPS; j++) {
        assert(stackPool.empty());
        for (int i = 0; i < ELEMS; i++)
          stackPool.push(i);
        for (int i = 0; i < ELEMS; i++)
          stackPool.pop();
    }
    std::cout << "MemoryPool Allocator Time: ";
    std::cout << (((double)clock() - start) / CLOCKS_PER_SEC) << "\n\n";

    return 0;
}