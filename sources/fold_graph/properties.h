#ifndef FOLD_GRAPH_PROPERTIES_H
#define FOLD_GRAPH_PROPERTIES_H

#include "fold_specification/graph.h"

bool fold_graph_is_maekawa_compliant(const FoldGraph* graph);
bool fold_graph_is_kawasaki_compliant(const FoldGraph* graph, real error);
bool fold_graph_is_kawasaki_compatible(const FoldGraph* graph, real error);

#endif /* FOLD_GRAPH_PROPERTIES_H */