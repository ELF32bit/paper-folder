#include "faces.h"
#include "edges.h"

Error fold_graph_FE_from_FV(FoldGraph* graph, const Map* EV_map) {
	ASSERT(NOT(graph->FE.is_view));

	Map _EV_map;
	if (EV_map == NULL) {
		TRY(fold_graph_get_EV_map(graph, &_EV_map));
		EV_map = &_EV_map;
	}

	TRY_OR_ELSE(array_resize(&graph->FE.data, graph->FV.data.size),
		if (EV_map == &_EV_map) map_destroy(&_EV_map));
	TRY_OR_ELSE(array_resize(&graph->FE.offsets, graph->FV.offsets.size),
		if (EV_map == &_EV_map) map_destroy(&_EV_map);
		array2_recreate(&graph->FE));

	ARRAY2_ITERATE(&graph->FV, fv) {
		ARRAY_FOR_EACH_IN_RANGE(&graph->FV.data,
			i, usize*, a, fv.start, fv.end)
		{
			usize* b = array_get(&graph->FV.data,
				wrap_index_in_range(i, +1, fv.start, fv.end));

			usize ab = hash_usize_mix2(*a, *b);
			usize* fei = map_get(EV_map, &ab, NULL);
			ASSERT(fei != NULL);

			array_set(&graph->FE.data, i, fei);
		}
		array_set(&graph->FE.offsets, fv.index, &fv.end);
	}

	if (EV_map == &_EV_map) map_destroy(&_EV_map);
	return OK;
}

Error fold_graph_FF_from_FV(FoldGraph* graph) {
	(void)graph; // TODO
	return OK;
}

Error fold_graph_FV_triangulate(FoldGraph* graph) {
	(void)graph; // TODO
	return OK;
}

Error fold_graph_FV_flip(FoldGraph* graph) {
	(void)graph; // TODO
	return OK;
}