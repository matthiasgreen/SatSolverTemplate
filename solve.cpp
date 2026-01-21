#include "Solver/inout.hpp"
#include <iostream>
int main() {
    auto problem = sat::inout::read_from_dimacs(std::cin);
}
