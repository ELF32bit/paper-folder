#include "edges.h"
#include "attributes.h"

#include "math/vector2.h"
#include "math/vector3.h"

#include <stdlib.h>

struct SortPrimitive { usize index; real weight; };

static inline
int _sort_primitives_ascending(const void* a, const void* b) {
	real w1 = ((const struct SortPrimitive*)a)->weight;
	real w2 = ((const struct SortPrimitive*)b)->weight;
	if (w1 < w2) return -1;
	if (w1 > w2) return 1;
	return 0;
}

Error fold_graph_get_EV_map(const FoldGraph* graph, Map* map) {
	TRY_MULTIPLY(graph->EV.size, 2);

	MAP_CREATE(EV_map, usize, usize);
	TRY_OR_ELSE(map_reserve(&EV_map, graph->EV.size * 2),
		map_destroy(&EV_map));

	EV_map.hash = usize_hash_identity;
	ARRAY_FOR_EACH(&graph->EV, ei, FoldGraphEdge*, ev) {
		usize ab = hash_usize_mix2(ev->a, ev->b);
		TRY_OR_ELSE(map_add(&EV_map, &ab, &ei, NULL),
			map_destroy(&EV_map));

		usize ba = hash_usize_mix2(ev->b, ev->a);
		TRY_OR_ELSE(map_add(&EV_map, &ba, &ei, NULL),
			map_destroy(&EV_map));
	}

	*map = EV_map;
	return OK;
}

static inline
Error fold_graph_EF_sort2D(FoldGraph* graph) {
	(void)graph;
	return OK; //TODO
}

static inline
Error fold_graph_EF_sort3D(FoldGraph* graph) {
	(void)graph;
	return OK; //TODO
}

Error fold_graph_EF_from_FV(FoldGraph* graph, bool sorted) {
	ASSERT(NOT(graph->EF.is_view));
	(void)graph; // TODO
	if NOT(sorted) return OK;
	if (fold_graph_is_abstract(graph)) {
		return OK;
	} else if (fold_graph_is_2D(graph)) {
		TRY(fold_graph_EF_sort2D(graph));
	} else if (fold_graph_is_3D(graph)) {
		TRY(fold_graph_EF_sort3D(graph));
	}
	return OK;
}

Error fold_graph_EA_from_EF(FoldGraph* graph) {
	ASSERT(NOT(graph->EA.is_view));
	ASSERT(graph->EA.size == graph->EF.size);
	ARRAY2_ITERATE(&graph->EF, ef) {
		usize ef_size = 0;
		ARRAY_FOR_EACH_IN_RANGE(&graph->EF.data,
			_, usize*, efi, ef.start, ef.end) {
			if (*efi != FOLD_GRAPH_NULL) ef_size++;
		}
		if (ef_size == 1) {
			ARRAY_SET(&graph->EA, ef.index,
				FoldGraphEdgeAssignment,
				FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY);
		}
	}
	return OK;
}

Error fold_graph_EA_from_EFA(FoldGraph* graph) {
	ASSERT(NOT(graph->EA.is_view));
	TRY(array_resize(&graph->EA, graph->EFA.size));
	ARRAY_FOR_EACH(&graph->EFA, ei, real*, efa) {
		if (*efa < 0.0) {
			ARRAY_SET(&graph->EA, ei,
				FoldGraphEdgeAssignment,
				FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN);
		} else if (*efa > 0.0) {
			ARRAY_SET(&graph->EA, ei,
				FoldGraphEdgeAssignment,
				FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY);
		} else {
			ARRAY_SET(&graph->EA, ei,
				FoldGraphEdgeAssignment,
				FOLD_GRAPH_EDGE_ASSIGNMENT_JOIN);
		}
	}
	return OK;
}

Error fold_graph_EFA_from_EA(FoldGraph* graph) {
	ASSERT(NOT(graph->EFA.is_view));
	TRY(array_resize(&graph->EFA, graph->EA.size));
	ARRAY_FOR_EACH(&graph->EA, ei, real*, ea) {
		if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN) {
			ARRAY_SET(&graph->EFA, ei, real, -180.0);
		} else if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY) {
			ARRAY_SET(&graph->EFA, ei, real, 180.0);
		} else {
			ARRAY_SET(&graph->EFA, ei, real, 0.0);
		}
	}
	return OK;
}

Error fold_graph_EL_from_EVC(FoldGraph* graph) {
	ASSERT(NOT(graph->EL.is_view));
	if (fold_graph_is_abstract(graph)) {
		TRY(array_resize(&graph->EL, 0));
	} else if (fold_graph_is_2D(graph)) {
		TRY(array_resize(&graph->EL, graph->EV.size));
		ARRAY_FOR_EACH(&graph->EV, ei, FoldGraphEdge*, ev) {
			Vector2* A = array_get(&graph->VC, ev->a);
			Vector2* B = array_get(&graph->VC, ev->b);
			real length = vector2_distance(*A, *B);
			array_set(&graph->EL, ei, &length);
		}
	} else if (fold_graph_is_3D(graph)) {
		TRY(array_resize(&graph->EL, graph->EV.size));
		ARRAY_FOR_EACH(&graph->EV, ei, FoldGraphEdge*, ev) {
			Vector3* A = array_get(&graph->VC, ev->a);
			Vector3* B = array_get(&graph->VC, ev->b);
			real length = vector3_distance(*A, *B);
			array_set(&graph->EL, ei, &length);
		}
	}
	return OK;
}