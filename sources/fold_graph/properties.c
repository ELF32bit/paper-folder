#include "properties.h"

#include "math/vector2.h"

bool fold_graph_is_maekawa_compliant(const FoldGraph* graph) {
	ASSERT(graph->VE.size != 0);
	ASSERT(graph->EA.size != 0);

	ARRAY2_ITERATE(&graph->VE, ve) {
		bool is_boundary = false;
		usize M_edges = 0, V_edges = 0;

		ARRAY_FOR_EACH_IN_RANGE(&graph->VE.data,
			_, usize*, vei, ve.start, ve.end)
		{
			FoldGraphEdgeAssignment* ea = array_get(&graph->EA, *vei);
			if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY ||
				*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_CUT) {
				is_boundary = true;
			} else if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN) {
				M_edges++;
			} else if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY) {
				V_edges++;
			}
		}

		if (is_boundary) continue;
		if ((M_edges > 0 || V_edges > 0) &&
			(MAX(M_edges, V_edges) - MIN(M_edges, V_edges) != 2)) {
			return false;
		}
	}

	return true;
}

bool fold_graph_is_kawasaki_compliant(const FoldGraph* graph, real error) {
	return fold_graph_is_kawasaki_compatible(graph, error); //TODO
}

bool fold_graph_is_kawasaki_compatible(const FoldGraph* graph, real error) {
	ASSERT(graph->VC.element_size == sizeof(Vector2));
	ASSERT(graph->VC.size == graph->VE.size);
	ASSERT(error >= VECTOR2_EPSILON);
	ASSERT(graph->VE.size != 0);
	ASSERT(graph->EA.size != 0);

	ARRAY2_ITERATE(&graph->VE, ve) {
		bool is_boundary = false;
		usize J_edges = 0;

		ARRAY_FOR_EACH_IN_RANGE(&graph->VE.data,
			_, usize*, vei, ve.start, ve.end)
		{
			FoldGraphEdgeAssignment* ea = array_get(&graph->EA, *vei);
			if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY ||
				*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_CUT) {
				is_boundary = true;
			} else J_edges++;
		}

		if (is_boundary) continue;
		if (J_edges % 2 != 0) {
			return false;
		}

		real kawasaki_sum = 0.0;
		Vector2* C = array_get(&graph->VC, ve.index);
		ARRAY_FOR_EACH_IN_RANGE(&graph->VE.data,
			i, usize*, vei, ve.start, ve.end)
		{
			usize* vnei = array_get(&graph->VE.data,
				wrap_index_in_range(i, +1, ve.start, ve.end));

			FoldGraphEdge* e1 = array_get(&graph->EV, *vei);
			FoldGraphEdge* e2 = array_get(&graph->EV, *vnei);

			usize a = (ve.index != e1->a) ? e1->a : e1->b;
			usize b = (ve.index != e2->a) ? e2->a : e2->b;

			Vector2* A = array_get(&graph->VC, a);
			Vector2* B = array_get(&graph->VC, b);

			Vector2 CA = vector2_subtract(*A, *C);
			Vector2 CB = vector2_subtract(*B, *C);

			kawasaki_sum += vector2_angle_to(CA, CB) *
				((i % 2 == 0) ? 1.0 : -1.0);
		}

		if (ABS(kawasaki_sum) > error) {
			return false;
		}
	}

	return true;
}