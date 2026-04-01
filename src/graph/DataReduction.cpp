
Copy

//
// DataReduction.cpp
//
 
#include "graph/DataReduction.h"
#include "graph/Graph.h"
 
bool DataReduction::is_simplicial(Graph& h, uint32_t v) {
    const auto neighbors = h.get_neighbors(v);
    for (size_t i = 0; i < neighbors.size(); i++)
        for (size_t j = i + 1; j < neighbors.size(); j++)
            if (!h.edge_exists(neighbors[i], neighbors[j]))
                return false;
    return true;
}
 
bool DataReduction::is_almost_simplicial(Graph& h, uint32_t v) {
    const auto neighbors = h.get_neighbors(v);
    int missing = 0;
    for (size_t i = 0; i < neighbors.size(); i++)
        for (size_t j = i + 1; j < neighbors.size(); j++)
            if (!h.edge_exists(neighbors[i], neighbors[j]))
                if (++missing > 1) return false;
    return true;
}
 
void DataReduction::eliminate_simple(Graph& h, uint32_t v, std::vector<uint32_t>& ordering) {
    const auto neighbors = h.get_neighbors(v);
    for (size_t i = 0; i < neighbors.size(); i++) {
        for (size_t j = i + 1; j < neighbors.size(); j++) {
            uint32_t u = neighbors[i], w = neighbors[j];
            if (!h.edge_exists(u, w)) {
                uint32_t weight = h.get_edge_weight(u, v) + h.get_edge_weight(v, w);
                h.dr_add_neighbor(u, w, weight);
                h.dr_add_neighbor(w, u, weight);
                h.add_edge_cache(u, w, weight);
                h.add_edge_cache(w, u, weight);
            }
        }
    }
    h.dr_record_bag(v, neighbors);
    h.dr_remove_vertex(v, neighbors);
    ordering.push_back(v);
}
 
void DataReduction::apply(Graph& h, std::vector<uint32_t>& ordering) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto it = h.dr_adj_begin(); it != h.dr_adj_end(); ) {
            uint32_t v = it->first;
            ++it;
            if (!h.dr_has_vertex(v)) continue;
            uint32_t deg = h.dr_degree(v);
            if (deg == 0) {
                h.dr_remove_isolated(v, ordering);
                changed = true;
            } else if (deg == 1) {
                eliminate_simple(h, v, ordering);
                changed = true;
            } else if (is_simplicial(h, v)) {
                eliminate_simple(h, v, ordering);
                changed = true;
            } else if (deg <= 4 && is_almost_simplicial(h, v)) {
                eliminate_simple(h, v, ordering);
                changed = true;
            }
        }
    }
}
