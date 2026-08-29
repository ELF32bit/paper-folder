#include "graph.h"

#include "math/color.h"
#include "math/vector2.h"
#include "math/vector3.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

static
void fold_graph_extensions_create(FoldGraphExtensions* extensions) {
	array_create(&extensions->PC, sizeof(Color));
	array_create(&extensions->TC, sizeof(Vector2));
	array_create(&extensions->T2C, sizeof(Vector2));
	array_create(&extensions->NC, sizeof(Vector3));
	array_create(&extensions->MN, SIZEOF(char, 32));
	array_create(&extensions->VP, sizeof(usize));
	array2_create(&extensions->FT, sizeof(usize));
	array2_create(&extensions->FT2, sizeof(usize));
	array2_create(&extensions->FN, sizeof(usize));
	array_create(&extensions->FM, sizeof(usize));
}

void fold_graph_create(FoldGraph* graph) {
	array_create(&graph->VC, sizeof(Vector3));
	array2_create(&graph->VV, sizeof(usize));
	array2_create(&graph->VE, sizeof(usize));
	array2_create(&graph->VF, sizeof(usize));
	array_create(&graph->EV, sizeof(FoldGraphEdge));
	array2_create(&graph->EF, sizeof(usize));
	array_create(&graph->EA, sizeof(FoldGraphEdgeAssignment));
	array_create(&graph->EFA, sizeof(real));
	array_create(&graph->EL, sizeof(real));
	array_create(&graph->EO, sizeof(FoldGraphEdgeOrder));
	array2_create(&graph->FV, sizeof(usize));
	array2_create(&graph->FE, sizeof(usize));
	array2_create(&graph->FF, sizeof(usize));
	array_create(&graph->FO, sizeof(FoldGraphFaceOrder));
	fold_graph_extensions_create(&graph->extensions);
}

static
void fold_graph_extensions_destroy(FoldGraphExtensions* extensions) {
	array_destroy(&extensions->PC);
	array_destroy(&extensions->TC);
	array_destroy(&extensions->T2C);
	array_destroy(&extensions->NC);
	array_destroy(&extensions->MN);
	array_destroy(&extensions->VP);
	array2_destroy(&extensions->FT);
	array2_destroy(&extensions->FT2);
	array2_destroy(&extensions->FN);
	array_destroy(&extensions->FM);
}

void fold_graph_destroy(FoldGraph* graph) {
	array_destroy(&graph->VC);
	array2_destroy(&graph->VV);
	array2_destroy(&graph->VE);
	array2_destroy(&graph->VF);
	array_destroy(&graph->EV);
	array2_destroy(&graph->EF);
	array_destroy(&graph->EA);
	array_destroy(&graph->EFA);
	array_destroy(&graph->EL);
	array_destroy(&graph->EO);
	array2_destroy(&graph->FV);
	array2_destroy(&graph->FE);
	array2_destroy(&graph->FF);
	array_destroy(&graph->FO);
	fold_graph_extensions_destroy(&graph->extensions);
}

void fold_graph_recreate(FoldGraph* graph) {
	fold_graph_destroy(graph);
	fold_graph_create(graph);
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

static
Error fold_graph_extensions_copy(
	FoldGraphExtensions* extensions, const FoldGraphExtensions* source)
{
	TRY(array_copy(&extensions->PC, &source->PC));
	TRY(array_copy(&extensions->TC, &source->TC));
	TRY(array_copy(&extensions->T2C, &source->T2C));
	TRY(array_copy(&extensions->NC, &source->NC));
	TRY(array_copy(&extensions->MN, &source->MN));
	TRY(array_copy(&extensions->VP, &source->VP));
	TRY(array2_copy(&extensions->FT, &source->FT));
	TRY(array2_copy(&extensions->FT2, &source->FT2));
	TRY(array2_copy(&extensions->FN, &source->FN));
	TRY(array_copy(&extensions->FM, &source->FM));
	return OK;
}

Error fold_graph_copy(FoldGraph* graph, const FoldGraph* source) {
	TRY(array_copy(&graph->VC, &source->VC));
	TRY(array2_copy(&graph->VV, &source->VV));
	TRY(array2_copy(&graph->VE, &source->VE));
	TRY(array2_copy(&graph->VF, &source->VF));
	TRY(array_copy(&graph->EV, &source->EV));
	TRY(array2_copy(&graph->EF, &source->EF));
	TRY(array_copy(&graph->EA, &source->EA));
	TRY(array_copy(&graph->EFA, &source->EFA));
	TRY(array_copy(&graph->EL, &source->EL));
	TRY(array_copy(&graph->EO, &source->EO));
	TRY(array2_copy(&graph->FV, &source->FV));
	TRY(array2_copy(&graph->FE, &source->FE));
	TRY(array2_copy(&graph->FF, &source->FF));
	TRY(array_copy(&graph->FO, &source->FO));
	TRY(fold_graph_extensions_copy(&graph->extensions, &source->extensions));
	return OK;
}

static
void fold_graph_extensions_inherit(
	FoldGraphExtensions* extensions, const FoldGraphExtensions* source)
{
	if (extensions->PC.is_view || extensions->PC.size == 0)
		array_view(&extensions->PC, &source->PC);
	if (extensions->TC.is_view || extensions->TC.size == 0)
		array_view(&extensions->TC, &source->TC);
	if (extensions->T2C.is_view || extensions->T2C.size == 0)
		array_view(&extensions->T2C, &source->T2C);
	if (extensions->NC.is_view || extensions->NC.size == 0)
		array_view(&extensions->NC, &source->NC);
	if (extensions->MN.is_view || extensions->MN.size == 0)
		array_view(&extensions->MN, &source->MN);
	if (extensions->VP.is_view || extensions->VP.size == 0)
		array_view(&extensions->VP, &source->VP);
	if (extensions->FT.is_view || extensions->FT.size == 0)
		array2_view(&extensions->FT, &source->FT);
	if (extensions->FT2.is_view || extensions->FT2.size == 0)
		array2_view(&extensions->FT2, &source->FT2);
	if (extensions->FN.is_view || extensions->FN.size == 0)
		array2_view(&extensions->FN, &source->FN);
	if (extensions->FM.is_view || extensions->FM.size == 0)
		array_view(&extensions->FM, &source->FM);
}

void fold_graph_inherit(FoldGraph* graph, const FoldGraph* source) {
	if (graph->VC.is_view || graph->VC.size == 0)
		array_view(&graph->VC, &source->VC);
	if (graph->VV.is_view || graph->VV.size == 0)
		array2_view(&graph->VV, &source->VV);
	if (graph->VE.is_view || graph->VE.size == 0)
		array2_view(&graph->VE, &source->VE);
	if (graph->VF.is_view || graph->VF.size == 0)
		array2_view(&graph->VF, &source->VF);
	if (graph->EV.is_view || graph->EV.size == 0)
		array_view(&graph->EV, &source->EV);
	if (graph->EF.is_view || graph->EF.size == 0)
		array2_view(&graph->EF, &source->EF);
	if (graph->EA.is_view || graph->EA.size == 0)
		array_view(&graph->EA, &source->EA);
	if (graph->EFA.is_view || graph->EFA.size == 0)
		array_view(&graph->EFA, &source->EFA);
	if (graph->EL.is_view || graph->EL.size == 0)
		array_view(&graph->EL, &source->EL);
	if (graph->EO.is_view || graph->EO.size == 0)
		array_view(&graph->EO, &source->EO);
	if (graph->FV.is_view || graph->FV.size == 0)
		array2_view(&graph->FV, &source->FV);
	if (graph->FE.is_view || graph->FE.size == 0)
		array2_view(&graph->FE, &source->FE);
	if (graph->FF.is_view || graph->FF.size == 0)
		array2_view(&graph->FF, &source->FF);
	if (graph->FO.is_view || graph->FO.size == 0)
		array_view(&graph->FO, &source->FO);
	fold_graph_extensions_inherit(&graph->extensions, &source->extensions);
}

static
bool fold_graph_extensions_is_inherited(const FoldGraphExtensions* extensions) {
	if (extensions->PC.is_view) return true;
	if (extensions->TC.is_view) return true;
	if (extensions->T2C.is_view) return true;
	if (extensions->NC.is_view) return true;
	if (extensions->MN.is_view) return true;
	if (extensions->VP.is_view) return true;
	if (extensions->FT.is_view) return true;
	if (extensions->FT2.is_view) return true;
	if (extensions->FN.is_view) return true;
	if (extensions->FM.is_view) return true;
	return false;
}

bool fold_graph_is_inherited(const FoldGraph* graph) {
	if (graph->VC.is_view) return true;
	if (graph->VV.is_view) return true;
	if (graph->VE.is_view) return true;
	if (graph->VF.is_view) return true;
	if (graph->EV.is_view) return true;
	if (graph->EF.is_view) return true;
	if (graph->EA.is_view) return true;
	if (graph->EFA.is_view) return true;
	if (graph->EL.is_view) return true;
	if (graph->EO.is_view) return true;
	if (graph->FV.is_view) return true;
	if (graph->FE.is_view) return true;
	if (graph->FF.is_view) return true;
	if (graph->FO.is_view) return true;
	if (fold_graph_extensions_is_inherited(&graph->extensions)) return true;
	return false;
}

/* ========================================================================= */
/* Attributes                                                                */
/* ========================================================================= */

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
	ARRAY2_ITERATE(&graph->EF, array) {
		if (array.size > 2) {
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

				real cross = vector2_cross(AB, BC);
				if (cross < -VECTOR2_EPSILON) {
					is_negative = true;
				} else if (cross > VECTOR2_EPSILON) {
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
				Vector3 cross = vector3_cross(AB, BC);

				if (vector3_length2(cross) > VECTOR3_EPSILON) {
					normal = vector3_normalize(cross);
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

				Vector3 cross = vector3_cross(AB, BC);
				real sign = vector3_dot(cross, normal);

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

/* ========================================================================= */
/* Vertices Building                                                         */
/* ========================================================================= */

AABB2 fold_graph_get_aabb2(const FoldGraph* graph) {
	AABB2 aabb2 = aabb2_empty();
	if (fold_graph_is_2D(graph)) {
		ARRAY_FOR_EACH(&graph->VC, vi, Vector2*, vc) {
			aabb2 = aabb2_expand(aabb2, *vc);
		}
	} else if (fold_graph_is_3D(graph)) {
		ARRAY_FOR_EACH(&graph->VC, vi, Vector3*, vc) {
			aabb2 = aabb2_expand(aabb2, (Vector2)
				{ .x = vc->x, .y = vc->y });
		}
	}
	return aabb2_is_degenerate(aabb2)
		? aabb2_from_point(vector2_zero())
		: aabb2;
}

AABB3 fold_graph_get_aabb3(const FoldGraph* graph) {
	AABB3 aabb3 = aabb3_empty();
	if (fold_graph_is_2D(graph)) {
		ARRAY_FOR_EACH(&graph->VC, vi, Vector2*, vc) {
			aabb3 = aabb3_expand(aabb3, (Vector3)
				{ .x = vc->x, .y = vc->y, .z = 0.0 });
		}
	} else if (fold_graph_is_3D(graph)) {
		ARRAY_FOR_EACH(&graph->VC, vi, Vector3*, vc) {
			aabb3 = aabb3_expand(aabb3, *vc);
		}
	}
	return aabb3_is_degenerate(aabb3)
		? aabb3_from_point(vector3_zero())
		: aabb3;
}

static inline
Error get_VV_map_from_EV(const FoldGraph* graph, Map* map) {
	MAP_CREATE_MANAGED_VALUES(VV_map, usize, Set);
	VV_map.hash = usize_hash_identity;

	TRY_MULTIPLY(graph->EV.size, 2);
	TRY(map_reserve(&VV_map, graph->EV.size * 2));
	ARRAY_FOR_EACH(&graph->EV, _, FoldGraphEdge*, ev) {
		usize a_hash = hash_usize(ev->a);
		Set* a_set = map_get(&VV_map, &a_hash, NULL);
		if (a_set == NULL) {
			SET_CREATE(VV_set, usize);
			TRY_OR_ELSE(set_add(&VV_set, &ev->b, NULL),
				set_destroy(&VV_set); map_destroy(&VV_map));

			TRY_OR_ELSE(map_add(&VV_map, &a_hash, &VV_set, NULL),
				set_destroy(&VV_set); map_destroy(&VV_map));
		} else {
			TRY_OR_ELSE(set_add(a_set, &ev->b, NULL),
				map_destroy(&VV_map));
		}

		usize b_hash = hash_usize(ev->b);
		Set* b_set = map_get(&VV_map, &b_hash, NULL);
		if (b_set == NULL) {
			SET_CREATE(VV_set, usize);
			TRY_OR_ELSE(set_add(&VV_set, &ev->a, NULL),
				set_destroy(&VV_set); map_destroy(&VV_map));

			TRY_OR_ELSE(map_add(&VV_map, &b_hash, &VV_set, NULL),
				set_destroy(&VV_set); map_destroy(&VV_map));
		} else {
			TRY_OR_ELSE(set_add(b_set, &ev->a, NULL),
				map_destroy(&VV_map));
		}
	}

	*map = VV_map;
	return OK;
}

static inline
Error fold_graph_VV_from_EV_unsorted(FoldGraph* graph) {
	Map VV_map;
	TRY(get_VV_map_from_EV(graph, &VV_map));
	array2_recreate(&graph->VV);
	array2_recreate(&graph->VE);
	array2_recreate(&graph->VF);
	// TODO
	return OK;
}

Error fold_graph_VV_from_EV(FoldGraph* graph) {
	(void)graph; // TODO
	return OK;
}

static inline
Error get_VV_map_from_FV(const FoldGraph* graph, Map* map) {
	MAP_CREATE_MANAGED_VALUES(VV_map, usize, Set);
	VV_map.hash = usize_hash_identity;

	TRY_MULTIPLY(graph->FV.size, 3);
	TRY(map_reserve(&VV_map, graph->FV.size * 3));
	ARRAY2_ITERATE(&graph->FV, fv) {
		ARRAY_FOR_EACH_IN_RANGE(&graph->FV.data,
			i, usize*, b, fv.start, fv.end)
		{
			usize* c = array_get(&graph->FV.data,
				wrap_index_in_range(i, +1, fv.start, fv.end));
			usize* a = array_get(&graph->FV.data,
				wrap_index_in_range(i, -1, fv.start, fv.end));

			usize b_hash = hash_usize(*b);
			Set* b_set = map_get(&VV_map, &b_hash, NULL);
			if (b_set == NULL) {
				SET_CREATE(VV_set, usize);
				TRY_OR_ELSE(set_add(&VV_set, a, NULL),
					set_destroy(&VV_set); map_destroy(&VV_map));
				TRY_OR_ELSE(set_add(&VV_set, c, NULL),
					set_destroy(&VV_set); map_destroy(&VV_map));

				TRY_OR_ELSE(map_add(&VV_map, &b_hash, &VV_set, NULL),
					set_destroy(&VV_set); map_destroy(&VV_map));
			} else {
				TRY_OR_ELSE(set_add(b_set, a, NULL),
					map_destroy(&VV_map));
				TRY_OR_ELSE(set_add(b_set, c, NULL),
					map_destroy(&VV_map));
			}
		}
	}

	*map = VV_map;
	return OK;
}

static inline
Error fold_graph_VV_from_FV_unsorted(FoldGraph* graph) {
	Map VV_map;
	TRY(get_VV_map_from_FV(graph, &VV_map));
	array2_recreate(&graph->VV);
	array2_recreate(&graph->VE);
	array2_recreate(&graph->VF);
	// TODO
	return OK;
}

Error fold_graph_VV_from_FV(FoldGraph* graph) {
	(void)graph; // TODO
	return OK;
}

Error fold_graph_VE_from_VV(FoldGraph* graph, const Map* EV_map) {
	ASSERT(NOT(graph->VE.is_view));
	Map _EV_map;
	if (EV_map == NULL) {
		TRY(fold_graph_get_EV_map(graph, &_EV_map));
		EV_map = &_EV_map;
	}

	TRY_OR_ELSE(array_resize(&graph->VE.data, graph->VV.data.size),
		if (EV_map == &_EV_map) map_destroy(&_EV_map));

	TRY_OR_ELSE(array_resize(&graph->VE.offsets, graph->VV.offsets.size),
		if (EV_map == &_EV_map) map_destroy(&_EV_map);
		array2_recreate(&graph->VE));

	ARRAY2_ITERATE(&graph->VV, vv) {
		ARRAY_FOR_EACH_IN_RANGE(&graph->VV.data,
			i, usize*, vvi, vv.start, vv.end)
		{
			usize ab = hash_usize_mix2(vv.index, *vvi);
			usize* vei = map_get(EV_map, &ab, NULL);
			if (vei == NULL) { if (EV_map == &_EV_map) {
				map_destroy(&_EV_map); }
				return ERROR;
			}

			array_set(&graph->VE.data, i, vei);
		}
		array_set(&graph->VE.offsets, vv.index, &vv.end);
	}

	if (EV_map == &_EV_map) {
	map_destroy(&_EV_map); }
	return OK;
}

Error fold_graph_VF_from_VV(FoldGraph* graph) {
	(void)graph; // TODO
	return OK;
}

/* ========================================================================= */
/* Edges Building                                                            */
/* ========================================================================= */

Error fold_graph_get_EV_map(const FoldGraph* graph, Map* map) {
	MAP_CREATE(EV_map, usize, usize);
	EV_map.hash = usize_hash_identity;

	TRY_MULTIPLY(graph->EV.size, 2);
	TRY(map_reserve(&EV_map, graph->EV.size * 2));
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

Error fold_graph_EF_from_FV(FoldGraph* graph) {
	(void)graph; // TODO
	return OK;
}

Error fold_graph_EA_from_EF(FoldGraph* graph) {
	ASSERT(NOT(graph->EA.is_view));
	if (graph->EA.size != graph->EF.size) return ERROR;
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
			Vector2* a = array_get(&graph->VC, ev->a);
			Vector2* b = array_get(&graph->VC, ev->b);
			real length = vector2_distance(*a, *b);
			array_set(&graph->EL, ei, &length);
		}
	} else if (fold_graph_is_3D(graph)) {
		TRY(array_resize(&graph->EL, graph->EV.size));
		ARRAY_FOR_EACH(&graph->EV, ei, FoldGraphEdge*, ev) {
			Vector3* a = array_get(&graph->VC, ev->a);
			Vector3* b = array_get(&graph->VC, ev->b);
			real length = vector3_distance(*a, *b);
			array_set(&graph->EL, ei, &length);
		}
	}
	return OK;
}

/* ========================================================================= */
/* Faces Building                                                            */
/* ========================================================================= */

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
			if (fei == NULL) { if (EV_map == &_EV_map) {
				map_destroy(&_EV_map); }
				return ERROR;
			}

			array_set(&graph->FE.data, i, fei);
		}
		array_set(&graph->FE.offsets, fv.index, &fv.end);
	}

	if (EV_map == &_EV_map) {
	map_destroy(&_EV_map); }
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