//
// Created by ori294 on 1/14/20.
//

#ifndef EX4__SOLVERTOSEARCHERADAPTER_H_
#define EX4__SOLVERTOSEARCHERADAPTER_H_

#include <SearchAlgorithms/Searcher.h>
#include "Solver.h"

template<class Solution, class Problem, class T>
class SolverToSearcherAdapter : public Solver<Solution, Problem> {
  Searcher<Solution, T>* searcher_;
 public:
  explicit SolverToSearcherAdapter(Searcher<Solution, T>* searcher) {
    this->searcher_ = searcher;
  }

  Solution solve(Problem searchAble) override {
    return searcher_->search(searchAble);
  }
};

#endif //EX4__SOLVERTOSEARCHERADAPTER_H_