#pragma once
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include "defs.hpp"
#include <iostream>

namespace mini {

template<size_t Stack_Size>
struct Stack_Allocator {
    static_assert(mini_is_power_of_two(Stack_Size), "Stack pages must be in power of two.");
    static constexpr size_t page_size = Stack_Size;

    // this needs to be specific for T
    uint8_t* push(size_t size, size_t alignment) {
        assert(mini_is_power_of_two(alignment));
        Stack_Node* tmp = &head; 
        Stack_Node** node = &tmp;

        // go through all the pages.
        while(*node) { 
            auto& n = *node;
            auto buf = (uintptr_t)n->stack.buffer;
            auto p = buf + (uintptr_t)n->stack.current;
            auto a = (uintptr_t)alignment;
            auto mod = p & (a - 1);
            if(mod != 0) {
                p += a - mod;
            }
            auto result = p + (uintptr_t)size - buf;
            if(result < page_size) {
                assert(result >=0);
                n->stack.current = (size_t)result;
                std::cout << std::hex << p << std::endl;
                return (uint8_t*)p;
            }
            node = &n->next;
        }

        // allocate new page.
        std::cout << "allocating new page" << std::endl;

        *node = new Stack_Node;
        auto& n = *node;
        auto buf = (uintptr_t)n->stack.buffer;
        auto p = buf + (uintptr_t)n->stack.current;
        auto a = (uintptr_t)alignment;
        auto mod = p & (a - 1);
        if(mod != 0) {
            p += a - mod;
        }
        auto result = p + (uintptr_t)size - buf;
        assert(result < page_size);
        assert(result >=0);
        n->stack.current = (size_t)result;
        std::cout << std::hex << p << std::endl;
        return (uint8_t*)p;
    }

    // need to call destructor for some T
    void clear() {
        auto tmp = &head;
        while(tmp) {
            tmp->stack.current = 0;
            tmp = tmp->next;
        }
    }

private:
    struct Stack {
        uint8_t buffer[Stack_Size];
        size_t current = {};
    };

    struct Stack_Node {
        Stack stack;
        Stack_Node* next = nullptr;
    };

private:
    Stack_Node head;
};

using Default_Stack_Allocator = Stack_Allocator<1u<<10>;

}
