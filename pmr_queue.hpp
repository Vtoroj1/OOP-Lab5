#pragma once
#include <memory_resource>
#include <utility>
#include <memory>
#include <iterator>
#include <type_traits>
#include <stdexcept>

template<typename T>
class pmr_queue {
private:
    struct Node {
        T data;
        Node* next;
        
        template<typename... Args>
        Node(Args&&... args) : data(std::forward<Args>(args)...), next(nullptr) {}
    };
    
    Node* m_head;
    Node* m_tail;
    std::size_t m_size;
    std::pmr::polymorphic_allocator<Node> m_allocator;

public:
    template<typename U>
    class queue_iterator {
    private:
        using node_ptr = std::conditional_t<
            std::is_const_v<U>, 
            const Node*, 
            Node*>;
        node_ptr m_current;
        
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = U;
        using difference_type = std::ptrdiff_t;
        using pointer = U*;
        using reference = U&;
        
        queue_iterator(node_ptr current = nullptr) : m_current(current) {}
        
        reference operator*() const { 
            return m_current->data; 
        }
        
        pointer operator->() const { 
            return &(m_current->data); 
        }
        
        queue_iterator& operator++() {
            if (m_current) {
                m_current = m_current->next;
            }
            return *this;
        }

        queue_iterator operator++(int) {
            queue_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        bool operator==(const queue_iterator& other) const { 
            return m_current == other.m_current; 
        }
        
        bool operator!=(const queue_iterator& other) const { 
            return m_current != other.m_current; 
        }
    };

    using iterator = queue_iterator<T>;
    using const_iterator = queue_iterator<const T>;

    explicit pmr_queue(std::pmr::memory_resource* mr = std::pmr::get_default_resource())
        : m_head(nullptr), m_tail(nullptr), m_size(0), m_allocator(mr) {}
    
    ~pmr_queue() {
        clear();
    }
    
    pmr_queue(const pmr_queue& other) 
        : m_allocator(std::allocator_traits<
            decltype(m_allocator)>::select_on_container_copy_construction(
                other.m_allocator)),
          m_head(nullptr), m_tail(nullptr), m_size(0) {
        Node* current = other.m_head;
        while (current) {
            push(current->data);
            current = current->next;
        }
    }
    
    pmr_queue& operator=(const pmr_queue& other) {
        if (this != &other) {
            if constexpr (std::allocator_traits<
                decltype(m_allocator)>::propagate_on_container_copy_assignment::value) {
                m_allocator = other.m_allocator;
            }
            
            clear();
            Node* current = other.m_head;
            while (current) {
                push(current->data);
                current = current->next;
            }
        }
        return *this;
    }
    
    pmr_queue(pmr_queue&& other) noexcept 
        : m_head(other.m_head), m_tail(other.m_tail), m_size(other.m_size), 
          m_allocator(std::move(other.m_allocator)) {
        other.m_head = nullptr;
        other.m_tail = nullptr;
        other.m_size = 0;
    }
    
    pmr_queue& operator=(pmr_queue&& other) noexcept {
        if (this != &other) {
            clear();
            m_head = other.m_head;
            m_tail = other.m_tail;
            m_size = other.m_size;
            
            if constexpr (std::allocator_traits<
                decltype(m_allocator)>::propagate_on_container_move_assignment::value) {
                m_allocator = std::move(other.m_allocator);
            }
            
            other.m_head = nullptr;
            other.m_tail = nullptr;
            other.m_size = 0;
        }
        return *this;
    }
    
    template<typename U>
    void push(U&& value) {
        Node* new_node = std::allocator_traits<decltype(m_allocator)>::allocate(m_allocator, 1);
        try {
            std::allocator_traits<decltype(m_allocator)>::construct(
                m_allocator, new_node, std::forward<U>(value));
        } catch (...) {
            std::allocator_traits<decltype(m_allocator)>::deallocate(m_allocator, new_node, 1);
            throw;
        }
        
        if (m_tail) {
            m_tail->next = new_node;
        } else {
            m_head = new_node;
        }
        m_tail = new_node;
        ++m_size;
    }
    
    void pop() {
        if (empty()) {
            throw std::out_of_range("pop from empty queue");
        }
        
        Node* old_head = m_head;
        m_head = m_head->next;
        if (!m_head) {
            m_tail = nullptr;
        }
        
        std::allocator_traits<decltype(m_allocator)>::destroy(m_allocator, old_head);
        std::allocator_traits<decltype(m_allocator)>::deallocate(m_allocator, old_head, 1);
        --m_size;
    }
    
    T& front() {
        if (empty()) {
            throw std::out_of_range("front on empty queue");
        }
        return m_head->data;
    }
    
    const T& front() const {
        if (empty()) {
            throw std::out_of_range("front on empty queue");
        }
        return m_head->data;
    }
    
    T& back() {
        if (empty()) {
            throw std::out_of_range("back on empty queue");
        }
        return m_tail->data;
    }
    
    const T& back() const {
        if (empty()) {
            throw std::out_of_range("back on empty queue");
        }
        return m_tail->data;
    }
    
    bool empty() const { return m_size == 0; }
    std::size_t size() const { return m_size; }
    
    void clear() {
        while (!empty()) {
            pop();
        }
    }
    
    iterator begin() { return iterator(m_head); }
    iterator end() { return iterator(nullptr); }
    
    const_iterator begin() const { return const_iterator(m_head); }
    const_iterator end() const { return const_iterator(nullptr); }
    
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }
};