/**
 * @author Tim Luchterhand
 * @date 27.11.24
 * @brief
 */

#include "Solver.hpp"
#include "Clause.hpp"
#include "basic_structures.hpp"
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include <algorithm>

namespace sat {

Assignments::Assignments(unsigned numVariables) {
    assignments = std::vector<TruthValue>(numVariables, TruthValue::Undefined);
}

TruthValue Assignments::operator[](Variable var) const {
    return assignments[size_t(var.get())];
}

void Assignments::set(Variable var, TruthValue value) {
    assignments[size_t(var.get())] = value;
}

Solver::Solver(unsigned numVariables) : assignments(numVariables) {}

bool Solver::addClause(Clause clause) {
    if (clause.isEmpty()) {
        return false;
    } else if (clause.size() == 1) {
        auto literal = clause[0];
        if (falsified(literal)) {
            return false;
        }
        unitLiterals.push_back(clause[0]);
    } else {
        watchLists[clause.getWatcherByRank(0).get()].push_back(std::shared_ptr<Clause>(&clause));
        watchLists[clause.getWatcherByRank(1).get()].push_back(std::shared_ptr<Clause>(&clause));
        clauses.push_back(clause);
    }
    return true;
}

/**
 * Here you have a possible implementation of the rebase-method. It should work
 * out of the box. To use it, remove the throw-expression and un-comment the
 * code below. The implementation requires that your solver has some sort of
 * container of pointer types to clause called 'clauses' (e.g.
 * std::vector<ClausePointer>). Additionally, your solver needs to have a
 * container of unit literals called 'unitLiterals'.
 */
auto Solver::rebase() const -> std::vector<Clause> {
    std::vector<Clause> reducedClauses;
    // We check all clauses in the solver. If the clause is SAT (at least one
    // literal is satisfied), we don't include it. Additionally, we remove all
    // falsified literals from the clauses since we only care about unassigned
    // literals.
    for (const auto &c : clauses) {
        bool sat = false;
        // We're creating a new (possibly smaller clause from the current clause
        std::vector<Literal> newLits;
        newLits.reserve(c.size());
        // For each literal l in the clause we check if l is satisfied or
        // falsified
        for (auto l : c) {
            if (satisfied(l)) {
                // in this case, the whole clause is satisfied and can be
                // removed entirely from the solver
                sat = true;
                break;
            }

            if (!falsified(l)) {
                // only unassigned literals are added to the reduced clause
                newLits.emplace_back(l);
            }
        }

        if (!sat) {
            // create the new clause (move all the literals inside the
            // Clause-class)
            Clause newClause(std::move(newLits));
            // Check if we already added an equivalent clause
            auto res = std::ranges::find_if(
                reducedClauses, [&newClause](const auto &clause) {
                    return clause.sameLiterals(newClause);
                });
            if (res == reducedClauses.end()) {
                // The new clause is not yet contained => add it
                reducedClauses.emplace_back(std::move(newClause));
            }
        }
    }

    // Finally, we need to add all the unit literals as well
    for (Literal l : unitLiterals) {
        reducedClauses.emplace_back(std::vector{l});
    }

    return reducedClauses;
}

TruthValue Solver::val(Variable x) const { return assignments[x]; }

bool Solver::satisfied(Literal l) const {
    return l.sign() == (short)assignments[var(l)];
}

bool Solver::falsified(Literal l) const {
    return -1 * l.sign() == (short)assignments[var(l)];
}

bool Solver::assign(Literal l) {
    if (falsified(l)) {
        return false;
    }
    unitLiterals.push_back(l);
    assignments.set(var(l), (TruthValue)l.sign());
    return true;
}

bool Solver::unitPropagate() {
    size_t i = 0;
    while (i < unitLiterals.size()) {
        auto l = unitLiterals[i].negate();
        if (!unitPropagate(l)) {
            return false;
        }
        i++;
    }
    return true;
}

bool Solver::unitPropagate(Literal l) {
    for (auto c: watchLists[l.get()]) {
        auto rank = c->getRank(l);
        auto start = c->getIndex(rank);
        auto i = start;
        auto p = c->getWatcherByRank(1-rank);
        if (!satisfied(p)) {
            while (true) {
                i++;
                if (i == c->size()) {
                    i = 0;
                }
                if (i == start) {
                    break;
                }
                auto ci = (*c)[i];
                if (ci != p) {
                    if (!falsified(ci)) {
                        auto &watchList = watchLists[l.get()];
                        watchList.erase(std::remove(watchList.begin(), watchList.end(), c), watchList.end());
                        watchLists[ci.get()].push_back(c);
                        c->setWatcher(ci, rank);
                        break;
                    }
                }
            }
            if (i == start) {
                if (!assign(p)) {
                    return false;
                }
            }
        }
    }
    return true;
}

} // namespace sat
