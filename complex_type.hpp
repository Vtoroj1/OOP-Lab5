#pragma once

#include <string>
#include <iostream>
#include <memory_resource>

struct ComplexType {
    int id;
    std::string name;
    double value;
    std::pmr::string description;
    
    ComplexType(int i, const char* n, double v, const char* d)
        : id(i), name(n), value(v), description(d) {}
    
    friend std::ostream& operator<<(std::ostream& os, const ComplexType& ct) {
        os << "ComplexType{id=" << ct.id 
           << ", имя='" << ct.name 
           << "', значение=" << ct.value 
           << ", описание='" << ct.description 
           << "'}";
        return os;
    }
};
