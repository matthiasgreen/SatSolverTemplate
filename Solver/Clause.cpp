/**
 * @author Tim Luchterhand
 * @date 26.11.24
 * @brief
 */

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <optional>
#include <vector>

#include "Clause.hpp"
#include "basic_structures.hpp"
#include "util/exception.hpp"

namespace sat {
// TODO implementation here

Clause::Clause(std::vector<Literal> literals) { this->literals = literals; }

short Clause::getRank(Literal l) const {
    if (watchers[0].has_value() && literals[watchers[0].value()] == l) {
        return 0;
    } else if (watchers[1].has_value() && literals[watchers[1].value()] == l) {
        return 1;
    } else {
        return -1;
    }
}

std::size_t Clause::getIndex(short rank) const {
    if (rank < 0 || rank > 1) {
        throw -1;
    }
    std::optional<size_t> watcher = watchers[rank];
    if (watcher.has_value()) {
        return watcher.value();
    } else {
        throw -1;
    }
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
    if (watchers[rank].has_value()) {
        return literals[watchers[rank].value()];
    }
    throw -1;
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
