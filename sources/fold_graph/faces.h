#ifndef FOLD_GRAPH_FACES_H
#define FOLD_GRAPH_FACES_H

#include "fold_specification/graph.h"

#include "collections/map.h"

Error fold_graph_FE_from_FV(FoldGraph* graph, const Map* EV_map);
Error fold_graph_FF_from_FV(FoldGraph* graph);

Error fold_graph_FV_triangulate(FoldGraph* graph);
Error fold_graph_FV_flip(FoldGraph* graph);

#endif /* FOLD_GRAPH_FACES_H */