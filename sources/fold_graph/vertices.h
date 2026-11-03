#ifndef FOLD_GRAPH_VERTICES_H
#define FOLD_GRAPH_VERTICES_H

#include "fold_specification/graph.h"

#include "collections/map.h"

Error fold_graph_VV_from_EV(FoldGraph* graph, bool sorted);
Error fold_graph_VV_from_FV(FoldGraph* graph, bool sorted);

Error fold_graph_VE_from_VV(FoldGraph* graph,
	const Map* NULLABLE(EV_map));

Error fold_graph_VF_from_VV(FoldGraph* graph);

#endif /* FOLD_GRAPH_VERTICES_H */