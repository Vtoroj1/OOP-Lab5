#include "cube_memory_resource.hpp"
#include "pmr_queue.hpp"
#include "complex_type.hpp"
#include <iostream>

void demonstrate_with_int() {
    std::cout << "=== Демонстрация работы очереди с типом int ===" << std::endl;
    cube_memory_resource cube_mr;
    pmr_queue<int> int_queue(&cube_mr);
    for (int i = 1; i <= 10; ++i) {
        int_queue.push(i);
    }
    
    std::cout << "Размер очереди: " << int_queue.size() << std::endl;
    std::cout << "Первый элемент (front): " << int_queue.front() << std::endl;
    std::cout << "Последний элемент (back): " << int_queue.back() << std::endl;
    
    std::cout << "Итерация с помощью итераторов: ";
    for (auto it = int_queue.begin(); it != int_queue.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Извлекаем элементы (FIFO порядок): ";
    while (!int_queue.empty()) {
        std::cout << int_queue.front() << " ";
        int_queue.pop();
    }
    std::cout << std::endl;
    
    std::cout << "Использовано блоков памяти: " << cube_mr.allocated_blocks() 
              << " из " << cube_mr.total_blocks() 
              << " (всего памяти: " << cube_mr.total_memory() << " байт)" 
              << std::endl;
    std::cout << std::endl;
}

void demonstrate_with_complex_type() {
    std::cout << "=== Демонстрация работы очереди с ComplexType ===" << std::endl;
    cube_memory_resource cube_mr;
    pmr_queue<ComplexType> complex_queue(&cube_mr);
    complex_queue.push(ComplexType(1, "First", 1.1, "Description 1"));
    
    complex_queue.push(ComplexType(2, "Second", 2.2, "Description 2"));
    complex_queue.push(ComplexType(3, "Third", 3.3, "Description 3"));
    complex_queue.push(ComplexType(4, "Fourth", 4.4, "Description 4"));
    
    std::cout << "Размер очереди ComplexType: " << complex_queue.size() << std::endl;
    
    std::cout << "Итерация с помощью итераторов:" << std::endl;
    for (auto it = complex_queue.begin(); it != complex_queue.end(); ++it) {
        std::cout << *it << std::endl;
    }
    
    std::cout << "Извлекаем элементы (FIFO):" << std::endl;
    while (!complex_queue.empty()) {
        std::cout << complex_queue.front() << std::endl;
        complex_queue.pop();
    }
    
    std::cout << "Использовано блоков памяти: " << cube_mr.allocated_blocks() 
              << " из " << cube_mr.total_blocks() 
              << " (всего памяти: " << cube_mr.total_memory() << " байт)" 
              << std::endl;
    std::cout << std::endl;
}

void demonstrate_memory_reuse() {
    std::cout << "=== Демонстрация повторного использования памяти ===" << std::endl;
    
    cube_memory_resource cube_mr;
    
    pmr_queue<int> queue(&cube_mr);
    
    std::cout << "Изначально блоков: " << cube_mr.total_blocks() << std::endl;
    
    for (int i = 0; i < 5; ++i) {
        queue.push(i);
    }
    
    std::cout << "После добавления 5 элементов: " << cube_mr.total_blocks() << " блоков" << std::endl;
    
    for (int i = 0; i < 3; ++i) {
        queue.pop();
    }
    
    std::cout << "После удаления 3 элементов: " << cube_mr.total_blocks() << " блоков" << std::endl;
    std::cout << "Свободных блоков: " 
              << (cube_mr.total_blocks() - cube_mr.allocated_blocks()) 
              << std::endl;
    
    for (int i = 10; i < 15; ++i) {
        queue.push(i);
    }
    
    std::cout << "После добавления еще 5 элементов: " << cube_mr.total_blocks() << " блоков" << std::endl;
    
    while (!queue.empty()) {
        queue.pop();
    }
    
    std::cout << "После полной очистки: " << cube_mr.total_blocks() 
              << " блоков (память не возвращена upstream для повторного использования)" << std::endl;
    std::cout << std::endl;
}

void demonstrate_copy_and_move() {
    std::cout << "=== Демонстрация копирования и перемещения ===" << std::endl;
    
    cube_memory_resource cube_mr;
    
    pmr_queue<int> original_queue(&cube_mr);
    for (int i = 1; i <= 3; ++i) {
        original_queue.push(i * 10);
    }
    
    pmr_queue<int> copied_queue(original_queue);
    std::cout << "После копирования:" << std::endl;
    std::cout << "Оригинал (размер " << original_queue.size() << "): ";
    for (const auto& item : original_queue) std::cout << item << " ";
    std::cout << std::endl;
    std::cout << "Копия (размер " << copied_queue.size() << "): ";
    for (const auto& item : copied_queue) std::cout << item << " ";
    std::cout << std::endl;
    
    pmr_queue<int> moved_queue(std::move(original_queue));
    std::cout << "После перемещения:" << std::endl;
    std::cout << "Оригинал (размер " << original_queue.size() << "): пуст" << std::endl;
    std::cout << "Перемещенная (размер " << moved_queue.size() << "): ";
    for (const auto& item : moved_queue) std::cout << item << " ";
    std::cout << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "ДЕМОНСТРАЦИЯ РАБОТЫ КОНТЕЙНЕРА ОЧЕРЕДИ\n";
    std::cout << "==================================================================\n\n";
    
    demonstrate_with_int();
    demonstrate_with_complex_type();
    demonstrate_memory_reuse();
    demonstrate_copy_and_move();
    
    std::cout << "==================================================================\n";
    std::cout << "Все демонстрации успешно завершены!" << std::endl;
    
    return 0;
}