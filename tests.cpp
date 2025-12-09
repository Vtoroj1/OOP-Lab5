#include <gtest/gtest.h>
#include <memory_resource>
#include "pmr_queue.hpp"
#include "cube_memory_resource.hpp"
#include "complex_type.hpp"
#include <vector>
#include <algorithm>
#include <string>

// ==================== Базовые тесты для очереди ====================

TEST(PMRQueueTest, BasicIntOperations) {
    cube_memory_resource mr;
    pmr_queue<int> queue(&mr);

    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);

    queue.push(1);
    queue.push(2);
    queue.push(3);

    EXPECT_FALSE(queue.empty());
    EXPECT_EQ(queue.size(), 3);
    EXPECT_EQ(queue.front(), 1);
    EXPECT_EQ(queue.back(), 3);

    queue.pop();
    EXPECT_EQ(queue.front(), 2);
    EXPECT_EQ(queue.size(), 2);

    queue.pop();
    queue.pop();
    EXPECT_TRUE(queue.empty());
}

TEST(PMRQueueTest, ComplexTypeOperations) {
    cube_memory_resource mr;

    pmr_queue<ComplexType> queue(&mr);

    queue.push(ComplexType(1, "First", 1.1, "Description 1"));
    queue.push(ComplexType(2, "Second", 2.2, "Description 2"));
    queue.push(ComplexType(3, "Third", 3.3, "Description 3"));

    EXPECT_EQ(queue.size(), 3);
    EXPECT_EQ(queue.front().name, "First");
    EXPECT_EQ(queue.front().value, 1.1);
    EXPECT_EQ(queue.back().name, "Third");
    EXPECT_EQ(queue.back().value, 3.3);

    queue.pop();
    EXPECT_EQ(queue.front().name, "Second");
    EXPECT_EQ(queue.front().value, 2.2);
}

TEST(PMRQueueTest, IteratorTest) {
    cube_memory_resource mr;
    pmr_queue<int> queue(&mr);

    queue.push(10);
    queue.push(20);
    queue.push(30);

    // Test forward iteration
    auto it = queue.begin();
    EXPECT_EQ(*it, 10);
    ++it;
    EXPECT_EQ(*it, 20);
    it++;
    EXPECT_EQ(*it, 30);
    ++it;
    EXPECT_EQ(it, queue.end());

    // Test range-based for loop
    std::vector<int> values;
    for (const auto& val : queue) {
        values.push_back(val);
    }
    EXPECT_EQ(values, std::vector<int>({10, 20, 30}));

    // Test const iterators
    const auto& const_queue = queue;
    std::vector<int> const_values;
    for (auto cit = const_queue.cbegin(); cit != const_queue.cend(); ++cit) {
        const_values.push_back(*cit);
    }
    EXPECT_EQ(const_values, std::vector<int>({10, 20, 30}));
}

TEST(PMRQueueTest, CopyAndMoveSemantics) {
    cube_memory_resource mr;
    
    pmr_queue<int> original(&mr);
    original.push(1);
    original.push(2);
    original.push(3);

    // Test copy constructor
    pmr_queue<int> copy(original);
    EXPECT_EQ(copy.size(), 3);
    EXPECT_EQ(copy.front(), 1);
    EXPECT_EQ(copy.back(), 3);
    
    // Modify copy, original should remain unchanged
    copy.pop();
    EXPECT_EQ(copy.size(), 2);
    EXPECT_EQ(original.size(), 3);

    // Test move constructor
    pmr_queue<int> moved(std::move(original));
    EXPECT_EQ(moved.size(), 3);
    EXPECT_TRUE(original.empty());

    // Test move assignment
    pmr_queue<int> another(&mr);
    another.push(99);
    another = std::move(moved);
    EXPECT_EQ(another.size(), 3);
    EXPECT_TRUE(moved.empty());
}

TEST(PMRQueueTest, MemoryReuseWithCubeResource) {
    cube_memory_resource mr;
    pmr_queue<std::string> queue(&mr);

    // Check initial memory state
    EXPECT_EQ(mr.allocated_blocks(), 0);

    // Allocate
    queue.push("first");
    queue.push("second");
    
    EXPECT_EQ(mr.allocated_blocks(), 2);

    // Deallocate first element
    queue.pop(); // "first" is deallocated, but block stays in pool
    
    EXPECT_EQ(mr.allocated_blocks(), 1);
    EXPECT_EQ(mr.total_blocks(), 2); // Block remains in pool

    // Should reuse memory from "first"
    queue.push("third");
    
    EXPECT_EQ(mr.allocated_blocks(), 2);
    EXPECT_EQ(mr.total_blocks(), 2); // No new block allocated!

    EXPECT_EQ(queue.front(), "second");
    EXPECT_EQ(queue.back(), "third");
}

TEST(PMRQueueTest, ClearTest) {
    cube_memory_resource mr;
    pmr_queue<int> queue(&mr);

    queue.push(1);
    queue.push(2);
    queue.push(3);

    EXPECT_EQ(queue.size(), 3);
    EXPECT_EQ(mr.allocated_blocks(), 3);
    
    queue.clear();
    
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(queue.size(), 0);
    EXPECT_EQ(mr.allocated_blocks(), 0);
    EXPECT_GE(mr.total_blocks(), 3); // Blocks remain in pool for reuse
}

TEST(PMRQueueTest, ExceptionSafety) {
    cube_memory_resource mr;
    pmr_queue<int> queue(&mr);
    
    // Basic exception safety test
    EXPECT_NO_THROW({
        queue.push(1);
        queue.push(2);
        queue.pop();
        queue.push(3);
    });

    // Test front/back on empty queue
    pmr_queue<int> empty_queue(&mr);
    EXPECT_THROW(empty_queue.front(), std::out_of_range);
    EXPECT_THROW(empty_queue.back(), std::out_of_range);
    EXPECT_THROW(empty_queue.pop(), std::out_of_range);
}
