
// DataReduction.h
// Preprocessing rules applied before min-fill to eliminate zero/near-zero
// fill vertices, reducing graph size and improving TD construction time.
//
// Rules applied exhaustively:
//   1. Degree-0: isolated vertices (free)
//   2. Degree-1: pendant vertices (no fill ever)
//   3. Simplicial: neighbors already form a clique (zero fill)
//   4. Almost-simplicial, degree <= 4: at most 1 fill edge
//
 
#ifndef GROUP7_DATAREDUCTION_H
#define GROUP7_DATAREDUCTION_H
 
#include <vector>
#include <cstdint>
 
class Graph; // forward declaration
 
class DataReduction {
public:
    static void apply(Graph& h, std::vector<uint32_t>& ordering);
 
private:
    static bool is_simplicial(Graph& h, uint32_t v);
    static bool is_almost_simplicial(Graph& h, uint32_t v);
    static void eliminate_simple(Graph& h, uint32_t v, std::vector<uint32_t>& ordering);
};
 
#endif //GROUP7_DATAREDUCTION_H
 
