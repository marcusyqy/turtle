#pragma once
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include "defs.hpp"

namespace mini {

template<size_t Stack_Size>
struct Stack_Allocator {
    static_assert(mini_is_power_of_two(Stack_Size), "Stack pages must be in power of two.");
    static constexpr size_t page_size = Stack_Size;

    uint8_t* allocate(size_t size, size_t alignment) {
        assert(mini_is_power_of_two(alignment));
    }

private:
    struct Stack {
        uint8_t buffer[Stack_Size];
        size_t size;
    };

    struct Stack_Node {
        Stack stack;
        Stack_Node* next;
    };

private:
    Stack_Node head;
};
}
