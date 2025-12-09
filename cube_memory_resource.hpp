#pragma once

#include <memory_resource>
#include <vector>
#include <cstddef>
#include <algorithm>

class cube_memory_resource : public std::pmr::memory_resource {
private:
    struct block_info {
        void* ptr;
        std::size_t size;
        std::size_t alignment;
        bool used;

        block_info(void* p, std::size_t sz, std::size_t align, bool u) : ptr(p), size(sz), alignment(align), used(u) {}
    };
    
    static constexpr std::size_t CUBE_SIZE = 64;
    
    std::vector<block_info> blocks;
    std::pmr::memory_resource* upstream;
    
public:
    explicit cube_memory_resource(std::pmr::memory_resource* upstream_resource = std::pmr::get_default_resource()) : upstream(upstream_resource) {}
    
    cube_memory_resource(const cube_memory_resource&) = delete;
    cube_memory_resource& operator=(const cube_memory_resource&) = delete;
    
    ~cube_memory_resource() {
        for (const auto& block : blocks) {
            if (block.ptr) {
                upstream->deallocate(block.ptr, block.size, block.alignment);
            }
        }
        blocks.clear();
    }
    
    std::size_t allocated_blocks() const { 
        return std::count_if(blocks.begin(), blocks.end(), [](const block_info& info) { return info.used; });
    }
    
    std::size_t total_blocks() const { 
        return blocks.size(); 
    }
    
    std::size_t total_memory() const {
        std::size_t total = 0;
        for (const auto& block : blocks) {
            total += block.size;
        }
        return total;
    }
    
protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        std::size_t actual_size = ((bytes + CUBE_SIZE - 1) / CUBE_SIZE) * CUBE_SIZE;
        
        for (auto& block : blocks) {
            if (!block.used && block.size >= bytes) {
                block.used = true;
                return block.ptr;
            }
        }
        
        void* ptr = upstream->allocate(actual_size, alignment);
        blocks.push_back(block_info(ptr, actual_size, alignment, true));
        
        return ptr;
    }
    
    void do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) override {
        auto it = std::find_if(blocks.begin(), blocks.end(),
            [ptr](const block_info& info) { return info.ptr == ptr; });
        
        if (it != blocks.end()) {
            it->used = false;
        } else {
            upstream->deallocate(ptr, bytes, alignment);
        }
    }
    
    bool do_is_equal(const memory_resource& other) const noexcept override {
        return this == &other;
    }
};
