/**
 * @author Tim Luchterhand
 * @date 26.11.24
 * @brief
 */

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>

#include "Clause.hpp"
#include "basic_structures.hpp"

namespace sat {
// TODO implementation here

Clause::Clause(std::vector<Literal> literals) {
    this->literals = literals;
    watchers[0] = 0;
    if (literals.size() >= 2) {
       watchers[1] = 1;
    } else {
        watchers[1] = 0;
    }
}

short Clause::getRank(Literal l) const {
    if (literals[watchers[0]] == l) {
        return 0;
    } else if (literals[watchers[1]] == l) {
        return 1;
    } else {
        return -1;
    }
}

std::size_t Clause::getIndex(short rank) const {
    if (rank < 0 || rank > 1) {
        throw -1;
    }
    return watchers[rank];
}

bool Clause::setWatcher(Literal l, short watcherNo) {
    if (watcherNo < 0 || watcherNo > 1) {
        throw -1;
    }
    for (unsigned i = 0; i < literals.size(); i++) {
        if (literals[i] == l) {
            watchers[watcherNo] = i;
            return true;
        }
    }
    return false;
}

auto Clause::begin() const -> std::vector<Literal>::const_iterator {
    return literals.begin();
}

auto Clause::end() const -> std::vector<Literal>::const_iterator {
    return literals.end();
}

bool Clause::isEmpty() const { return literals.size() == 0; }

Literal Clause::operator[](std::size_t index) const { return literals[index]; }

std::size_t Clause::size() const { return literals.size(); }

Literal Clause::getWatcherByRank(short rank) const {
    if (rank < 0 || rank > 1) {
        throw -1;
    }
    return literals[watchers[rank]];
}

bool Clause::sameLiterals(const Clause &other) const {
    auto literals_sorted = literals;
    std::sort(
        literals_sorted.begin(), literals_sorted.end(),
        [](const Literal &a, const Literal &b) { return a.get() < b.get(); });
    auto other_sorted = other.literals;
    std::sort(
        other_sorted.begin(), other_sorted.end(),
        [](const Literal &a, const Literal &b) { return a.get() < b.get(); });
    return literals_sorted == other_sorted;
}

} // namespace sat
