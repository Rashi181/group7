#ifndef METIS_ND_PERMUTATION_STRATEGY_H
#define METIS_ND_PERMUTATION_STRATEGY_H

#include "PermutationStrategy.h"
#include "Graph.h"
#include <vector>

class MetisNDPermutationStrategy : public PermutationStrategy {
public:
    std::vector<int> computePermutation(const Graph& graph) override {
        // Step 1: Convert graph → METIS format
        // Step 2: Call METIS partition
        // Step 3: Recursively build ordering
        // Step 4: Return permutation

        std::vector<int> ordering;

        // TODO: implement

        return ordering;
    }
};

#endif
