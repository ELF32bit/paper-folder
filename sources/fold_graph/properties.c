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
				break;
			} else if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN) {
				M_edges++;
			} else if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY) {
				V_edges++;
			}
		}

		if (is_boundary) continue;
		if (M_edges == 0 && V_edges == 0) continue;
		if (M_edges == 0 || V_edges == 0) return false;
		if (MAX(M_edges, V_edges) - MIN(M_edges, V_edges) != 2) {
			return false;
		}
	}

	return true;
}

bool fold_graph_is_kawasaki_compliant(const FoldGraph* graph, real error) {
	ASSERT(graph->VC.element_size == sizeof(Vector2));
	ASSERT(graph->VC.size == graph->VE.size);
	ASSERT(error >= VECTOR2_EPSILON);
	ASSERT(graph->VE.size != 0);
	ASSERT(graph->EA.size != 0);

	ARRAY2_ITERATE(&graph->VE, ve) {
		bool is_boundary = false;
		usize M_edges = 0, V_edges = 0;
		usize MV_start = USIZE_MAX;
		usize MV_end = USIZE_MAX;

		ARRAY_FOR_EACH_IN_RANGE(&graph->VE.data,
			i, usize*, vei, ve.start, ve.end)
		{
			FoldGraphEdgeAssignment* ea = array_get(&graph->EA, *vei);
			if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY ||
				*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_CUT) {
				is_boundary = true;
				break;
			} else if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN) {
				if (MV_start == USIZE_MAX) MV_start = i;
				MV_end = i;
				M_edges++;
			} else if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY) {
				if (MV_start == USIZE_MAX) MV_start = i;
				MV_end = i;
				V_edges++;
			}
		}

		if (is_boundary) continue;
		if (M_edges == 0 && V_edges == 0) continue;
		if (M_edges == 0 || V_edges == 0) return false;
		if ((TRY_SAFE(M_edges + V_edges)) % 2 != 0) {
			return false;
		}

		real kawasaki_sum = 0.0, sign = 1.0;
		Vector2* C = array_get(&graph->VC, ve.index);
		Vector2 CA_start = vector2_zero();
		Vector2 CA = vector2_zero();

		ARRAY_FOR_EACH_IN_RANGE(&graph->VE.data,
			i, usize*, vei, MV_start, TRY_SAFE(MV_end + 1))
		{
			if (MV_start == i) {
				FoldGraphEdge* e1 = array_get(&graph->EV, *vei);
				usize a = (ve.index != e1->a) ? e1->a : e1->b;
				Vector2* A = array_get(&graph->VC, a);
				CA = vector2_subtract(*A, *C);
				CA_start = CA;
				continue;
			}

			else if (MV_end != i) {
				FoldGraphEdgeAssignment* ea = array_get(&graph->EA, *vei);
				if NOT(*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN ||
					*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY) {
					continue;
				}
			}

			FoldGraphEdge* e2 = array_get(&graph->EV, *vei);
			usize b = (ve.index != e2->a) ? e2->a : e2->b;
			Vector2* B = array_get(&graph->VC, b);
			Vector2 CB = vector2_subtract(*B, *C);

			kawasaki_sum += vector2_angle_to(CA, CB) * sign;
			sign *= -1.0;
			CA = CB;

			if (MV_end == i) kawasaki_sum +=
				vector2_angle_to(CB, CA_start) * sign;
		}

		if (ABS(kawasaki_sum) > error) {
			return false;
		}
	}

	return true;
}

bool fold_graph_is_locally_flat_foldable(const FoldGraph* graph, real error) {
	return fold_graph_is_maekawa_compliant(graph) &&
		fold_graph_is_kawasaki_compliant(graph, error);
}

bool fold_graph_has_unassigned_vertices(const FoldGraph* graph) {
	ASSERT(graph->VE.size != 0);
	ASSERT(graph->EA.size != 0);

	ARRAY2_ITERATE(&graph->VE, ve) {
		bool has_BCMV_edges = false;

		ARRAY_FOR_EACH_IN_RANGE(&graph->VE.data,
			i, usize*, vei, ve.start, ve.end)
		{
			FoldGraphEdgeAssignment* ea = array_get(&graph->EA, *vei);
			if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY ||
				*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_CUT ||
				*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN ||
				*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY) {
				has_BCMV_edges = true;
				break;
			}
		}

		if NOT(has_BCMV_edges) {
			return true;
		}
	}

	return false;
}