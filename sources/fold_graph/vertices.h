#ifndef FOLD_GRAPH_VERTICES_H
#define FOLD_GRAPH_VERTICES_H

#include "fold_specification/graph.h"

#include "math/aabb2.h"
#include "math/aabb3.h"
#include "collections/map.h"

usize fold_graph_get_abstract_size(const FoldGraph* graph);
AABB2 fold_graph_get_aabb2(const FoldGraph* graph);
AABB3 fold_graph_get_aabb3(const FoldGraph* graph);

Error fold_graph_VV_from_EV(FoldGraph* graph, bool sorted);
Error fold_graph_VV_from_FV(FoldGraph* graph, bool sorted);
Error fold_graph_VE_from_VV(FoldGraph* graph, const Map* EV_map);
Error fold_graph_VF_from_VV(FoldGraph* graph);

#endif /* FOLD_GRAPH_VERTICES_H */