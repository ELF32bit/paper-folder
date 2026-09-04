#ifndef FOLD_GRAPH_ATTRIBUTES_H
#define FOLD_GRAPH_ATTRIBUTES_H

#include "fold_specification/graph.h"

bool fold_graph_is_abstract(const FoldGraph* graph);
bool fold_graph_is_2D(const FoldGraph* graph);
bool fold_graph_is_3D(const FoldGraph* graph);
bool fold_graph_is_manifold(const FoldGraph* graph);

Error fold_graph_is_orientable(const FoldGraph* graph, bool* is);
Error fold_graph_is_self_touching(const FoldGraph* graph, bool* is);
Error fold_graph_is_self_intersecting(const FoldGraph* graph, bool* is);

bool fold_graph_has_cuts(const FoldGraph* graph);
bool fold_graph_has_joins(const FoldGraph* graph);
bool fold_graph_has_concave_faces(const FoldGraph* graph);

#endif /* FOLD_GRAPH_ATTRIBUTES_H */
