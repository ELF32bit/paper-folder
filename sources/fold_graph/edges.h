#ifndef FOLD_GRAPH_EDGES_H
#define FOLD_GRAPH_EDGES_H

#include "fold_specification/graph.h"

#include "collections/map.h"

Error fold_graph_get_EV_map(const FoldGraph* graph, Map* map);

Error fold_graph_EF_from_FV(FoldGraph* graph, bool sorted);
Error fold_graph_EA_from_EF(FoldGraph* graph);
Error fold_graph_EA_from_EFA(FoldGraph* graph);
Error fold_graph_EFA_from_EA(FoldGraph* graph);
Error fold_graph_EL_from_EVC(FoldGraph* graph);

#endif /* FOLD_GRAPH_EDGES_H */