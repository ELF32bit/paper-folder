#include "attributes.h"

#include "math/vector2.h"
#include "math/vector3.h"

bool fold_graph_is_abstract(const FoldGraph* graph) {
	return (graph->VC.size == 0);
}

bool fold_graph_is_2D(const FoldGraph* graph) {
	return (graph->VC.element_size == sizeof(Vector2));
}

bool fold_graph_is_3D(const FoldGraph* graph) {
	return (graph->VC.element_size == sizeof(Vector3));
}

bool fold_graph_is_manifold(const FoldGraph* graph) {
	ARRAY2_ITERATE(&graph->EF, ef) {
		if (ef.size > 2) {
			return false;
		}
	}
	return true;
}

Error fold_graph_is_orientable(const FoldGraph* graph, bool* is) {
	(void)graph; // TODO
	*is = false;
	return OK;
}

bool fold_graph_has_cuts(const FoldGraph* graph) {
	ARRAY_FOR_EACH(&graph->EA, ei, FoldGraphEdgeAssignment*, ea) {
		if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_CUT) {
			return true;
		}
	}
	return false;
}

bool fold_graph_has_joins(const FoldGraph* graph) {
	ARRAY_FOR_EACH(&graph->EA, ei, FoldGraphEdgeAssignment*, ea) {
		if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_JOIN) {
			return true;
		}
	}
	return false;
}

Error fold_graph_is_self_touching(const FoldGraph* graph, bool* is) {
	(void)graph; // TODO
	*is = false;
	return OK;
}

Error fold_graph_is_self_intersecting(const FoldGraph* graph, bool* is) {
	(void)graph; // TODO
	*is = false;
	return OK;
}

bool fold_graph_has_concave_faces(const FoldGraph* graph) {
	if (fold_graph_is_abstract(graph)) {
		return false;
	} else if (fold_graph_is_2D(graph)) {
		ARRAY2_ITERATE(&graph->FV, fv) {
			if (fv.size < 4) continue;

			bool is_positive = false;
			bool is_negative = false;
			ARRAY_FOR_EACH_IN_RANGE(&graph->FV.data,
				i, usize*, a, fv.start, fv.end)
			{
				usize* b = array_get(&graph->FV.data,
					wrap_index_in_range(i, +1, fv.start, fv.end));
				usize* c = array_get(&graph->FV.data,
					wrap_index_in_range(i, +2, fv.start, fv.end));

				Vector2* A = array_get(&graph->VC, *a);
				Vector2* B = array_get(&graph->VC, *b);
				Vector2* C = array_get(&graph->VC, *c);

				Vector2 AB = vector2_subtract(*B, *A);
				Vector2 BC = vector2_subtract(*C, *B);
				real sign = vector2_cross(AB, BC);

				if (sign < -VECTOR2_EPSILON) {
					is_negative = true;
				} else if (sign > VECTOR2_EPSILON) {
					is_positive = true;
				}

				if (is_negative && is_positive) {
					return true;
				}
			}
		}
	} else if (fold_graph_is_3D(graph)) {
		ARRAY2_ITERATE(&graph->FV, fv) {
			if (fv.size < 4) continue;

			bool has_normal = false;
			Vector3 normal = vector3_zero();
			ARRAY_FOR_EACH_IN_RANGE(&graph->FV.data,
				i, usize*, a, fv.start, fv.end)
			{
				usize* b = array_get(&graph->FV.data,
					wrap_index_in_range(i, +1, fv.start, fv.end));
				usize* c = array_get(&graph->FV.data,
					wrap_index_in_range(i, +2, fv.start, fv.end));

				Vector3* A = array_get(&graph->VC, *a);
				Vector3* B = array_get(&graph->VC, *b);
				Vector3* C = array_get(&graph->VC, *c);

				Vector3 AB = vector3_subtract(*B, *A);
				Vector3 BC = vector3_subtract(*C, *B);
				Vector3 ABxBC = vector3_cross(AB, BC);

				if (vector3_length2(ABxBC) > VECTOR3_EPSILON) {
					normal = vector3_normalize(ABxBC);
					has_normal = true;
					break;
				}
			}
			if NOT(has_normal) {
				continue;
			}

			bool is_positive = false;
			bool is_negative = false;
			ARRAY_FOR_EACH_IN_RANGE(&graph->FV.data,
				i, usize*, a, fv.start, fv.end)
			{
				usize* b = array_get(&graph->FV.data,
					wrap_index_in_range(i, +1, fv.start, fv.end));
				usize* c = array_get(&graph->FV.data,
					wrap_index_in_range(i, +2, fv.start, fv.end));

				Vector3* A = array_get(&graph->VC, *a);
				Vector3* B = array_get(&graph->VC, *b);
				Vector3* C = array_get(&graph->VC, *c);

				Vector3 AB = vector3_subtract(*B, *A);
				Vector3 BC = vector3_subtract(*C, *B);
				Vector3 ABxBC = vector3_cross(AB, BC);
				real sign = vector3_dot(ABxBC, normal);

				if (sign < -VECTOR3_EPSILON) {
					is_negative = true;
				} else if (sign > VECTOR3_EPSILON) {
					is_positive = true;
				}

				if (is_negative && is_positive) {
					return true;
				}
			}
		}
	}
	return false;
}