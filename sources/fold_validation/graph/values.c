#include "values.h"

#include "collections/map.h"

void fold_graph_validate_EA_values(const FoldGraph* graph, bool* is) {
	*is = true;
	if (graph->EA.is_view) return;
	ARRAY_FOR_EACH(&graph->EA, _, FoldGraphEdgeAssignment*, ea) {
		if NOT(FOLD_GRAPH_EDGE_ASSIGNMENT_ANY(*ea)) {
			*is = false;
			return;
		}
	}
}

void fold_graph_validate_EFA_values(const FoldGraph* graph, bool* is) {
	*is = true;
	if (graph->EFA.is_view) return;
	ARRAY_FOR_EACH(&graph->EFA, _, real*, efa) {
		if (*efa < -180.0 || *efa > 180.0) {
			*is = false;
			return;
		}
	}
}

void fold_graph_validate_EA_EFA_values(const FoldGraph* graph, bool* is) {
	*is = true;
	if ((graph->EA.is_view && graph->EFA.is_view) ||
		(graph->EA.size == 0 || graph->EFA.size == 0)) return;
	*is = false;
	ARRAY_FOR_EACH(&graph->EA, ei, FoldGraphEdgeAssignment*, ea) {
		real* efa = array_get(&graph->EFA, ei);
		if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN) {
			if (*efa > 0.0) return;
		} else if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY) {
			if (*efa < 0.0) return;
		} else {
			if (*efa != 0.0) return;
		}
	}
	*is = true;
}

void fold_graph_validate_EL_values(const FoldGraph* graph, bool* is) {
	*is = true;
	if (graph->EL.is_view) return;
	ARRAY_FOR_EACH(&graph->EL, _, real*, el) {
		if (*el < 0.0) {
			*is = false;
			return;
		}
	}
}

static inline
Error _fold_graph_validate_orders_pairs(const Array* orders, bool* is) {
	*is = true;
	if (orders->is_view) return OK;
	*is = false;
	TRY_MULTIPLY(orders->size, 2);

	MAP_CREATE(pairs, usize, usize);
	TRY_OR_ELSE(map_reserve(&pairs, orders->size * 2),
		map_destroy(&pairs));

	pairs.hash = usize_hash_identity;
	ARRAY_FOR_EACH(orders, _, FoldGraphOrder*, order) {
		if (NOT(FOLD_GRAPH_ORDER_ANY(order->o)) ||
			order->a == order->b) {
			map_destroy(&pairs);
			return OK;
		} else {
			usize ab = hash_usize_mix2(order->a, order->b);
			usize ba = hash_usize_mix2(order->b, order->a);
			usize bao = FOLD_GRAPH_ORDER_INVERT(order->o);
			usize abo = order->o;

			if (map_has(&pairs, &ab)) {
				if (abo != *(usize*)map_get(&pairs, &ab, NULL) ||
					bao != *(usize*)map_get(&pairs, &ba, NULL)) {
					map_destroy(&pairs);
					return OK;
				}
			} else {
				TRY_OR_ELSE(map_add(&pairs, &ab, &abo, NULL),
					map_destroy(&pairs));
				TRY_OR_ELSE(map_add(&pairs, &ba, &bao, NULL),
					map_destroy(&pairs));
			}
		}
	}

	map_destroy(&pairs);
	*is = true;
	return OK;
}

Error fold_graph_validate_EO_pairs(const FoldGraph* graph, bool* is) {
	return _fold_graph_validate_orders_pairs(&graph->EO, is);
}

Error fold_graph_validate_FO_pairs(const FoldGraph* graph, bool* is) {
	return _fold_graph_validate_orders_pairs(&graph->FO, is);
}