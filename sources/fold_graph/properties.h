#ifndef FOLD_GRAPH_PROPERTIES_H
#define FOLD_GRAPH_PROPERTIES_H

#include "fold_specification/graph.h"

#include "math/aabb2.h"
#include "math/aabb3.h"

usize fold_graph_get_abstract_size(const FoldGraph* graph);
AABB2 fold_graph_get_aabb2(const FoldGraph* graph);
AABB3 fold_graph_get_aabb3(const FoldGraph* graph);

bool fold_graph_is_maekawa_compliant(const FoldGraph* graph);
bool fold_graph_is_kawasaki_compliant(const FoldGraph* graph, real error);

static inline
bool fold_graph_is_locally_flat_foldable(const FoldGraph* graph, real error) {
	return fold_graph_is_maekawa_compliant(graph) &&
		fold_graph_is_kawasaki_compliant(graph, error);
}

bool fold_graph_has_unassigned_vertices(const FoldGraph* graph);
bool fold_graph_is_triangulated(const FoldGraph* graph);

#endif /* FOLD_GRAPH_PROPERTIES_H */