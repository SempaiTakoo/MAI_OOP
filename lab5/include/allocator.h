#pragma once

#include <chrono>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <utility>

namespace allocators {
template <class T, size_t BLOCK_SIZE>
class Allocator {
   private:
    std::list<T *> _used_blocks;
    T **_free_blocks;
    uint64_t _free_count;
   public:
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using size_type = std::size_t;

    Allocator() {
        _used_blocks.push_back(
            static_cast<T *>(malloc(sizeof(T) * BLOCK_SIZE)));
        _free_blocks = new T *[BLOCK_SIZE];

        for (uint64_t i = 0; i < BLOCK_SIZE; i++) {
            _free_blocks[i] = _used_blocks.back() + i;
        }
        _free_count = BLOCK_SIZE;
    }

    ~Allocator() {
        for (auto block : _used_blocks) {
            free(block);
        }
        delete[] _free_blocks;
    }

    T *allocate(size_t n) {
        if (n > 1) {
            throw std::bad_alloc();
        }

        T *result = nullptr;
        if (_free_count > 0) {
            result = _free_blocks[--_free_count];
        } else {
            throw std::bad_alloc();
            ;
        }
        return result;
    }

    void deallocate(T *pointer, size_t) {}

    template <class U>
    struct rebind {
        using other = Allocator<U, BLOCK_SIZE>;
    };

    template <typename U, typename... Args>
    void construct(U *p, Args &&...args) {
        new (p) U(std::forward<Args>(args)...);
    }

    void destroy(pointer p) { p->~T(); }
};

template <class T, class U, size_t BLOCK_SIZE>
constexpr bool operator==(const Allocator<T, BLOCK_SIZE> &lhs,
                          const Allocator<U, BLOCK_SIZE> &rhs) {
    return true;
}

template <typename T, typename U, size_t BLOCK_SIZE>
constexpr bool operator!=(const Allocator<T, BLOCK_SIZE> &lhs,
                          const Allocator<U, BLOCK_SIZE> &rhs) {
    return false;
}

}
