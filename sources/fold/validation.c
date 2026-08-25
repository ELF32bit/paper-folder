#include "file.h"
#include "frame.h"
#include "graph.h"

#include "collections/set.h"
#include "collections/map.h"

#include <stdio.h>

#define MAX_FRAME_PARENTS 64;

/* ========================================================================= */
/* FOLD Frame Validation Parents                                             */
/* ========================================================================= */

static inline
Error _validate_frame_parents(const FoldFile* file, usize index, bool* is) {
	*is = false;
	FoldFrame* frame = array_get(&file->frames, index);
	if (frame->parent == FOLD_FRAME_PARENT_NONE ||
		frame->parent == index ||
		NOT(frame->inherit)) {
		*is = true;
		return OK;
	} else if (frame->parent >= file->frames.size) {
		return OK;
	}

	usize parent_index = frame->parent;
	if (parent_index >= file->frames.size) return OK;
	FoldFrame* parent = array_get(&file->frames, parent_index);
	if (parent->parent == FOLD_FRAME_PARENT_NONE ||
		parent->parent == parent_index ||
		NOT(parent->inherit)) {
		*is = true;
		return OK;
	} else if (parent->parent == index) {
		*is = false;
		return OK;
	}

	SET_CREATE(parents, usize);
	TRY_OR_ELSE(set_add(&parents, &index, NULL),
		set_destroy(&parents));
	TRY_OR_ELSE(set_add(&parents, &parent_index, NULL),
		set_destroy(&parents));

	usize max_parents = MAX_FRAME_PARENTS;
	while (max_parents > 1) {
		parent_index = parent->parent;
		if (parent_index >= file->frames.size) {
			set_destroy(&parents);
			return OK;
		}

		parent = array_get(&file->frames, parent_index);
		if (parent->parent == FOLD_FRAME_PARENT_NONE ||
			parent->parent == parent_index ||
			NOT(parent->inherit)) {
			set_destroy(&parents);
			*is = true;
			return OK;
		} else if (set_has(&parents, &parent_index)) {
			set_destroy(&parents);
			return OK;
		}

		TRY_OR_ELSE(set_add(&parents, &parent_index, NULL),
			set_destroy(&parents));
		max_parents--;
	}

	set_destroy(&parents);
	return OK;
}

/* ========================================================================= */
/* FOLD Graph Validation Arrays Sizes                                        */
/* ========================================================================= */

#define GENERATE_VALIDATE_SIZES(A, B) \
static inline \
void _validate_##A##_##B##_sizes(const FoldGraph* graph, bool* is) { \
	*is = (graph->A.size == graph->B.size || graph->A.size == 0); \
}

GENERATE_VALIDATE_SIZES(VV, VC)
GENERATE_VALIDATE_SIZES(VE, VC)
GENERATE_VALIDATE_SIZES(VF, VC)
GENERATE_VALIDATE_SIZES(EF, EV)
GENERATE_VALIDATE_SIZES(EA, EV)
GENERATE_VALIDATE_SIZES(EFA, EV)
GENERATE_VALIDATE_SIZES(EL, EV)
GENERATE_VALIDATE_SIZES(FE, FV)
GENERATE_VALIDATE_SIZES(FF, FV)

static inline
void _validate_FV_sizes(const FoldGraph* graph, bool* is) {
	*is = true;
	if (graph->FV.is_view) return;
	ARRAY2_ITERATE(&graph->FV, array) {
		if (array.size < 3) {
			*is = false;
			return;
		}
	}
}

GENERATE_VALIDATE_SIZES(VCC, VC)
GENERATE_VALIDATE_SIZES(VTC, VC)
GENERATE_VALIDATE_SIZES(VNC, VC)
GENERATE_VALIDATE_SIZES(FM, FV)

/* ========================================================================= */
/* FOLD Graph Validation Arrays References                                   */
/* ========================================================================= */

#define GENERATE_VALIDATE_REFERENCES(A, B) \
static inline \
void _validate_##A##_##B##_references(const FoldGraph* graph, bool* is) { \
	*is = true; \
	if ((graph->A.is_view && graph->B.is_view) || \
		(graph->A.size == 0 && graph->B.size == 0)) return; \
	usize max_index = graph->B.size; \
	for (usize i = 0; i < graph->A.data.size; i++) { \
		usize* index = array_get(&(graph->A.data), i); \
		if (*index >= max_index) { \
			*is = false; \
			return; \
		} \
	} \
}

#define GENERATE_VALIDATE_REFERENCES_WITH_NULL(A, B) \
static inline \
void _validate_##A##_##B##_references(const FoldGraph* graph, bool* is) { \
	*is = true; \
	if ((graph->A.is_view && graph->B.is_view) || \
		(graph->A.size == 0 && graph->B.size == 0)) return; \
	usize max_index = graph->B.size; \
	for (usize i = 0; i < graph->A.data.size; i++) { \
		usize* index = array_get(&(graph->A.data), i); \
		if (*index == FOLD_GRAPH_NULL) continue; \
		if (*index >= max_index) { \
			*is = false; \
			return; \
		} \
	} \
}

#define GENERATE_VALIDATE_REFERENCES_PAIRS(A, B) \
static inline \
void _validate_##A##_##B##_references(const FoldGraph* graph, bool* is) { \
	*is = true; \
	if ((graph->A.is_view && graph->B.is_view) || \
		(graph->A.size == 0 && graph->B.size == 0)) return; \
	usize max_index = graph->B.size; \
	for (usize i = 0; i < graph->A.size; i++) { \
		usize* pair = array_get(&graph->A, i); \
		if (pair[0] >= max_index || \
			pair[1] >= max_index) { \
			*is = false; \
			return; \
		} \
	} \
}

GENERATE_VALIDATE_REFERENCES(VV, VC)
GENERATE_VALIDATE_REFERENCES(VE, EV)
GENERATE_VALIDATE_REFERENCES_WITH_NULL(VF, FV)
GENERATE_VALIDATE_REFERENCES_PAIRS(EV, VC)
GENERATE_VALIDATE_REFERENCES_WITH_NULL(EF, FV)
GENERATE_VALIDATE_REFERENCES_PAIRS(EO, EV)
GENERATE_VALIDATE_REFERENCES(FV, VC)
GENERATE_VALIDATE_REFERENCES(FE, EV)
GENERATE_VALIDATE_REFERENCES_WITH_NULL(FF, FV)
GENERATE_VALIDATE_REFERENCES_PAIRS(FO, FV)

/* ========================================================================= */
/* FOLD Graph Validation Arrays Reflexive                                    */
/* ========================================================================= */

static inline
Error __to_map1(const Array2* array2, Set* map1, bool has_null) {
	SET_CREATE(set, usize);
	set.hash = usize_hash_identity;

	TRY(set_reserve(&set, array2->data.size));
	ARRAY2_FOR_EACH(array2, array, _, usize*, a) {
		if (NOT(has_null) || *a != FOLD_GRAPH_NULL) {
			usize Ia = hash_usize_mix2(array.index, *a);
			TRY_OR_ELSE(set_add(&set, &Ia, NULL),
				set_destroy(&set));
		}
	}

	*map1 = set;
	return OK;
}

static inline
bool __validate_reflexive(const Array2* array2, const Set* map1, bool has_null) {
	ARRAY2_FOR_EACH(array2, array, _, usize*, a) {
		if (NOT(has_null) || *a != FOLD_GRAPH_NULL) {
			usize aI = hash_usize_mix2(*a, array.index);
			if NOT(set_has(map1, &aI)) {
				return false;
			}
		}
	}
	return true;
}

static inline
Error _validate_VE_EV_reflexive(const FoldGraph* graph, bool* is) {
	*is = true;
	if ((graph->VE.is_view && graph->EV.is_view) ||
		(graph->VE.size == 0 || graph->EV.size == 0)) return OK;
	*is = false;

	ARRAY2_FOR_EACH(&graph->VE, ve, _, usize*, vei) {
		FoldGraphEdge* vei_ev = array_get(&graph->EV, *vei);
		if NOT(vei_ev->a == ve.index || vei_ev->b == ve.index) {
			*is = false;
			return OK;
		}
	}

	Set VE_map;
	TRY(__to_map1(&graph->VE, &VE_map, false));
	ARRAY_FOR_EACH(&graph->EV, ei, FoldGraphEdge*, ev) {
		usize aI = hash_usize_mix2(ev->a, ei);
		usize bI = hash_usize_mix2(ev->b, ei);
		if NOT(set_has(&VE_map, &aI) &&
			set_has(&VE_map, &bI)) {
			*is = false;
			return OK;
		}
	}

	set_destroy(&VE_map);
	*is = true;
	return OK;
}

#define GENERATE_VALIDATE_REFLEXIVE(A, B, has_null, is_same) \
static inline \
Error _validate_##A##_##B##_reflexive(const FoldGraph* graph, bool* is) { \
	*is = true; \
	if ((graph->A.is_view && graph->B.is_view) || \
		(graph->A.size == 0 || graph->B.size == 0)) return OK; \
	*is = false; \
	Set B##_map; \
	TRY(__to_map1(&graph->B, &B##_map, NOT(has_null))); \
	if (__validate_reflexive(&graph->A, &B##_map, has_null)) { \
		if (is_same) { *is = true; \
			set_destroy(&B##_map); return OK; } \
		Set A##_map; \
		TRY_OR_ELSE(__to_map1(&graph->A, &A##_map, has_null), \
			set_destroy(&B##_map)); \
		if (__validate_reflexive(&graph->B, &A##_map, NOT(has_null))) { \
			*is = true; \
		} \
		set_destroy(&A##_map); \
	} \
	set_destroy(&B##_map); \
	return OK; \
}

GENERATE_VALIDATE_REFLEXIVE(VV, VV, false, true)
GENERATE_VALIDATE_REFLEXIVE(VF, FV, true, false)
GENERATE_VALIDATE_REFLEXIVE(EF, FE, true, false)
GENERATE_VALIDATE_REFLEXIVE(FF, FF, true, true)

/* ========================================================================= */
/* FOLD Graph Validation Arrays Values                                       */
/* ========================================================================= */

static inline
void _validate_EA_values(const FoldGraph* graph, bool* is) {
	*is = true;
	if (graph->EA.is_view) return;
	ARRAY_FOR_EACH(&graph->EA, _, FoldGraphEdgeAssignment*, ea) {
		if NOT(FOLD_GRAPH_EDGE_ASSIGNMENT_ANY(*ea)) {
			*is = false;
			return;
		}
	}
}

static inline
void _validate_EFA_values(const FoldGraph* graph, bool* is) {
	*is = true;
	if (graph->EFA.is_view) return;
	ARRAY_FOR_EACH(&graph->EFA, _, real*, efa) {
		if (*efa < -180.0 || *efa > 180.0) {
			*is = false;
			return;
		}
	}
}

static inline
void _validate_EA_EFA_values(const FoldGraph* graph, bool* is) {
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

static inline
void _validate_EL_values(const FoldGraph* graph, bool* is) {
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
Error __validate_orders_pairs(const Array* orders, bool* is) {
	*is = true;
	if (orders->is_view) return OK;
	*is = false;

	MAP_CREATE(pairs, usize, usize);
	pairs.hash = usize_hash_identity;

	TRY_MULTIPLY(orders->size, 2);
	TRY(map_reserve(&pairs, orders->size * 2));
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

#define GENERATE_VALIDATE_ORDERS_PAIRS(O) \
static inline \
Error _validate_##O##_pairs(const FoldGraph* graph, bool* is) { \
	return __validate_orders_pairs(&graph->O, is); \
}

GENERATE_VALIDATE_ORDERS_PAIRS(EO);
GENERATE_VALIDATE_ORDERS_PAIRS(FO);

/* ========================================================================= */
/* FOLD Graph Validation Arrays Winding                                      */
/* ========================================================================= */

static inline
Error __to_map2(const Array2* array2, Set* map2) {
	SET_CREATE(set, usize);
	set.hash = usize_hash_identity;

	TRY_MULTIPLY(array2->data.size, 2);
	TRY(set_reserve(&set, array2->data.size * 2));
	ARRAY2_FOR_EACH(array2, array, index, usize*, a) {
		usize* b = array_get(&array2->data,
			wrap_index_in_range(index, +1, array.start, array.end));

		usize Iab = hash_usize_mix3(array.index, *a, *b);
		TRY_OR_ELSE(set_add(&set, &Iab, NULL),
			set_destroy(&set));

		usize Iba = hash_usize_mix3(array.index, *b, *a);
		TRY_OR_ELSE(set_add(&set, &Iba, NULL),
			set_destroy(&set));
	}

	*map2 = set;
	return OK;
}

static inline
Error __to_map3(const Array2* array2, Set* map3) {
	SET_CREATE(set, usize);
	set.hash = usize_hash_identity;

	TRY_MULTIPLY(array2->data.size, 2);
	TRY(set_reserve(&set, array2->data.size * 2));
	ARRAY2_FOR_EACH(array2, array, index, usize*, a) {
		usize* b = array_get(&array2->data,
			wrap_index_in_range(index, +1, array.start, array.end));
		usize* c = array_get(&array2->data,
			wrap_index_in_range(index, +2, array.start, array.end));

		usize Iabc = hash_usize_mix4(array.index, *a, *b, *c);
		TRY_OR_ELSE(set_add(&set, &Iabc, NULL),
			set_destroy(&set));

		usize Icba = hash_usize_mix4(array.index, *c, *b, *a);
		TRY_OR_ELSE(set_add(&set, &Icba, NULL),
			set_destroy(&set));
	}

	*map3 = set;
	return OK;
}

static inline
void _validate_VV_VE_winding(const FoldGraph* graph, bool* is) {
	*is = true;
	if ((graph->VV.is_view && graph->VE.is_view && graph->EV.is_view) ||
		(graph->VV.size == 0 || graph->VE.size == 0 || graph->EV.size == 0))
		return;

	ARRAY2_ITERATE(&graph->VV, vv) {
		usize ve_size = array2_size_at(&graph->VE, vv.index);
		if (vv.size != ve_size) {
			*is = false;
			return;
		}

		ARRAY_FOR_EACH_IN_RANGE(&graph->VV.data,
			i, usize*, vvi, vv.start, vv.end) {
			usize* vei = array_get(&graph->VE.data, i);
			FoldGraphEdge* vei_ev = array_get(&graph->EV, *vei);
			if NOT((vei_ev->a == vv.index && vei_ev->b == *vvi) ||
				(vei_ev->a == *vvi && vei_ev->b == vv.index)) {
				*is = false;
				return;
			}
		}
	}
}

static inline
Error _validate_VV_VF_winding(const FoldGraph* graph, bool* is) {
	*is = true;
	if ((graph->VV.is_view && graph->VF.is_view && graph->FV.is_view) ||
		(graph->VV.size == 0 || graph->VF.size == 0 || graph->FV.size == 0))
		return OK;
	*is = false;

	Set FV_map;
	TRY(__to_map3(&graph->FV, &FV_map));
	ARRAY2_ITERATE(&graph->VV, vv) {
		usize vf_size = array2_size_at(&graph->VF, vv.index);
		if (vv.size != vf_size) {
			set_destroy(&FV_map);
			return OK;
		}

		ARRAY_FOR_EACH_IN_RANGE(&graph->VV.data,
			i, usize*, vvi, vv.start, vv.end) {
			usize* vnvi = array_get(&graph->VV.data,
				wrap_index_in_range(i, +1, vv.start, vv.end));

			usize* vfi = array_get(&graph->VF.data, i);
			if (*vfi == FOLD_GRAPH_NULL) continue;

			usize Iabc = hash_usize_mix4(*vfi, *vvi, vv.index, *vnvi);
			usize Icba = hash_usize_mix4(*vfi, *vnvi, vv.index, *vvi);
			if NOT(set_has(&FV_map, &Iabc) ||
				set_has(&FV_map, &Icba)) {
				set_destroy(&FV_map);
				return OK;
			}
		}
	}

	set_destroy(&FV_map);
	*is = true;
	return OK;
}

static inline
Error _validate_VE_VF_winding(const FoldGraph* graph, bool* is) {
	*is = true;
	if ((graph->VE.is_view && graph->VF.is_view && graph->FE.is_view) ||
		(graph->VE.size == 0 || graph->VF.size == 0 || graph->FE.size == 0))
		return OK;
	*is = false;

	Set FE_map;
	TRY(__to_map2(&graph->FE, &FE_map));
	ARRAY2_ITERATE(&graph->VE, ve) {
		usize vf_size = array2_size_at(&graph->VF, ve.index);
		if (ve.size != vf_size) {
			set_destroy(&FE_map);
			return OK;
		}

		ARRAY_FOR_EACH_IN_RANGE(&graph->VE.data,
			i, usize*, vei, ve.start, ve.end) {
			usize* vnei = array_get(&graph->VE.data,
				wrap_index_in_range(i, +1, ve.start, ve.end));

			usize* vfi = array_get(&graph->VF.data, i);
			if (*vfi == FOLD_GRAPH_NULL) continue;

			usize Iab = hash_usize_mix3(*vfi, *vei, *vnei);
			usize Iba = hash_usize_mix3(*vfi, *vnei, *vei);
			if NOT(set_has(&FE_map, &Iab) ||
				set_has(&FE_map, &Iba)) {
				set_destroy(&FE_map);
				return OK;
			}
		}
	}

	set_destroy(&FE_map);
	*is = true;
	return OK;
}

static inline
void _validate_FV_FE_winding(const FoldGraph* graph, bool* is) {
	*is = true;
	if ((graph->FV.is_view && graph->FE.is_view && graph->EV.is_view) ||
		(graph->FV.size == 0 || graph->FE.size == 0 || graph->EV.size == 0))
		return;

	ARRAY2_ITERATE(&graph->FV, fv) {
		usize fe_size = array2_size_at(&graph->FE, fv.index);
		if (fv.size != fe_size) {
			*is = false;
			return;
		}

		ARRAY_FOR_EACH_IN_RANGE(&graph->FV.data,
			i, usize*, fvi, fv.start, fv.end) {
			usize* fnvi = array_get(&graph->FV.data,
				wrap_index_in_range(i, +1, fv.start, fv.end));

			usize* fei = array_get(&graph->FE.data, i);
			FoldGraphEdge* fei_ev = array_get(&graph->EV, *fei);
			if NOT((fei_ev->a == *fvi && fei_ev->b == *fnvi) ||
				(fei_ev->a == *fnvi && fei_ev->b == *fvi)) {
				*is = false;
				return;
			}
		}
	}
}

static inline
Error _validate_FV_FF_winding(const FoldGraph* graph, bool* is) {
	*is = true;
	if ((graph->FV.is_view && graph->FF.is_view) ||
		(graph->FV.size == 0 || graph->FF.size == 0)) return OK;
	*is = false;
	usize ff_offset = 0;

	Set FV_map;
	TRY(__to_map2(&graph->FV, &FV_map));
	ARRAY2_ITERATE(&graph->FV, fv) {
		usize ff_size = array2_size_at(&graph->FF, fv.index);
		if (ff_size == 0) { ff_offset += fv.size; continue; }
		else if (fv.size != ff_size) {
			set_destroy(&FV_map);
			return OK;
		}

		ARRAY_FOR_EACH_IN_RANGE(&graph->FV.data,
			i, usize*, fvi, fv.start, fv.end) {
			usize* fnvi = array_get(&graph->FV.data,
				wrap_index_in_range(i, +1, fv.start, fv.end));

			usize* ffi = array_get(&graph->FF.data, i - ff_offset);
			if (*ffi == FOLD_GRAPH_NULL) continue;

			usize Iab = hash_usize_mix3(*ffi, *fvi, *fnvi);
			usize Iba = hash_usize_mix3(*ffi, *fnvi, *fvi);
			if NOT(set_has(&FV_map, &Iab) ||
				set_has(&FV_map, &Iba)) {
				set_destroy(&FV_map);
				return OK;
			}
		}
	}

	set_destroy(&FV_map);
	*is = true;
	return OK;
}

static inline
void _validate_FE_FF_winding(const FoldGraph* graph, bool* is) {
	*is = true;
	if ((graph->FE.is_view && graph->FF.is_view && graph->EF.is_view) ||
		(graph->FE.size == 0 || graph->FF.size == 0 || graph->EF.size == 0))
		return;

	ARRAY2_ITERATE(&graph->FE, fe) {
		usize ff_start = array2_start_offset_at(&graph->FF, fe.index);
		usize ff_end = array2_end_offset_at(&graph->FF, fe.index);

		bool match_found = BOOL(ff_start == ff_end);
		FOR_EACH_IN_RANGE(i, ff_start, ff_end) {
			bool is_matching = true;
			usize match_size = 0;

			ARRAY_FOR_EACH_IN_RANGE(&graph->FE.data,
				_, usize*, fei, fe.start, fe.end) {
				usize ef_start = array2_start_offset_at(&graph->EF, *fei);
				usize ef_end = array2_end_offset_at(&graph->EF, *fei);

				usize current_match_size = 0;
				bool current_face_found = false;
				ARRAY_FOR_EACH_IN_RANGE(&graph->EF.data,
					__, usize*, efi, ef_start, ef_end) {
					if (*efi == fe.index && NOT(current_face_found)) {
						current_face_found = true;
						continue;
					}
					if (*efi != *(usize*)array_get(&graph->FF.data,
						wrap_index_in_range(i, match_size, ff_start, ff_end)))
					{
						is_matching = false;
						break;
					}
					current_match_size++;
					match_size++;
				}

				if NOT(is_matching) break;
				if (current_face_found && current_match_size == 0) {
					if (FOLD_GRAPH_NULL != *(usize*)array_get(&graph->FF.data,
						wrap_index_in_range(i, match_size, ff_start, ff_end)))
					{
						is_matching = false;
						break;
					}
					match_size++;
				}
			}

			if NOT(is_matching) continue;
			match_found = true;
			break;
		}

		if NOT(match_found) {
			*is = false;
			return;
		}
	}
}

/* ========================================================================= */
/* FOLD Graph Validation                                                     */
/* ========================================================================= */

#define VALIDATE(method, error) do { \
	bool _is = true; \
	_validate_##method(graph, &_is); \
	if NOT(_is) { \
		is_still = false; \
		TRY(string_append_raw(errors, error)); \
	} \
} while(0)

static
Error fold_graph_extensions_validate(const FoldGraph* graph, bool* is, String* errors) {
	*is = false;
	bool is_still = true;
	VALIDATE(VCC_VC_sizes, "ERROR: vertices_color_code and vertices_coords sizes are different\n");
	VALIDATE(VTC_VC_sizes, "ERROR: vertices_texture_coords and vertices_coords sizes are different\n");
	VALIDATE(VNC_VC_sizes, "ERROR: vertices_normal_coords and vertices_coords sizes are different\n");
	VALIDATE(FM_FV_sizes, "ERROR: faces_material and faces_vertices sizes are different\n");
	*is = is_still;
	return OK;
}

Error fold_graph_validate(const FoldGraph* graph, bool* is, String* errors) {
	*is = false;
	bool is_still = true;
	bool are_extensions = true;

	VALIDATE(VV_VC_sizes, "ERROR: vertices_vertices and vertices_coords sizes are different\n");
	VALIDATE(VE_VC_sizes, "ERROR: vertices_edges and vertices_coords sizes are different\n");
	VALIDATE(VF_VC_sizes, "ERROR: vertices_faces and vertices_coords sizes are different\n");
	VALIDATE(EF_EV_sizes, "ERROR: edges_faces and edges_vertices sizes are different\n");
	VALIDATE(EA_EV_sizes, "ERROR: edges_assignment and edges_vertices sizes are different\n");
	VALIDATE(EFA_EV_sizes, "ERROR: edges_foldAngle and edges_vertices sizes are different\n");
	VALIDATE(EL_EV_sizes, "ERROR: edges_length and edges_vertices sizes are different\n");
	VALIDATE(FE_FV_sizes, "ERROR: faces_edges and faces_vertices sizes are different\n");
	VALIDATE(FF_FV_sizes, "ERROR: faces_faces and faces_vertices sizes are different\n");
	VALIDATE(FV_sizes, "ERROR: unexpected sizes found in faces_vertices\n");
	if NOT(is_still) return OK;

	if (graph->VC.size > 0) {
	VALIDATE(VV_VC_references, "ERROR: vertices_vertices references missing in vertices_coords\n"); }
	VALIDATE(VE_EV_references, "ERROR: vertices_edges references missing in edges_vertices\n");
	VALIDATE(VF_FV_references, "ERROR: vertices_faces references missing in faces_vertices\n");
	if (graph->VC.size > 0) {
	VALIDATE(EV_VC_references, "ERROR: edges_vertices references missing in vertices_coords\n"); }
	VALIDATE(EF_FV_references, "ERROR: edges_faces references missing in faces_vertices\n");
	VALIDATE(EO_EV_references, "ERROR: edgeOrders references missing in edges_vertices\n");
	if (graph->VC.size > 0) {
	VALIDATE(FV_VC_references, "ERROR: faces_vertices references missing in vertices_coords\n"); }
	VALIDATE(FE_EV_references, "ERROR: faces_edges references missing in edges_vertices\n");
	VALIDATE(FF_FV_references, "ERROR: faces_faces references missing in faces_vertices\n");
	VALIDATE(FO_FV_references, "ERROR: faceOrders references missing in faces_vertices\n");
	if NOT(is_still) return OK;

	VALIDATE(VV_VV_reflexive, "ERROR: vertices_vertices and vertices_vertices mismatching\n");
	VALIDATE(VE_EV_reflexive, "ERROR: vertices_edges and edges_vertices mismatching\n");
	VALIDATE(VF_FV_reflexive, "ERROR: vertices_faces and faces_vertices mismatching\n");
	VALIDATE(EF_FE_reflexive, "ERROR: edges_faces and faces_edges mismatching\n");
	VALIDATE(FF_FF_reflexive, "ERROR: faces_faces and faces_faces mismatching\n");
	if NOT(is_still) return OK;

	VALIDATE(EA_values, "WARNING: unexpected values found in edges_assignment\n");
	VALIDATE(EFA_values, "WARNING: unexpected values found in edges_foldAngle\n");
	VALIDATE(EA_EFA_values, "WARNING: edges_assignment and edges_foldAngle mismatching\n");
	VALIDATE(EL_values, "WARNING: unexpected values found in edges_length\n");

	VALIDATE(EO_pairs, "WARNING: edgeOrders pairs mismatching\n");
	VALIDATE(FO_pairs, "WARNING: faceOrders pairs mismatching\n");

	VALIDATE(VV_VE_winding, "WARNING: vertices_vertices and vertices_edges mismatching winding\n");
	VALIDATE(VV_VF_winding, "WARNING: vertices_vertices and vertices_faces mismatching winding\n");
	VALIDATE(VE_VF_winding, "WARNING: vertices_edges and vertices_faces mismatching winding\n");
	VALIDATE(FV_FE_winding, "WARNING: faces_vertices and faces_edges mismatching winding\n");
	VALIDATE(FV_FF_winding, "WARNING: faces_vertices and faces_faces mismatching winding\n");
	VALIDATE(FE_FF_winding, "WARNING: faces_edges and faces_faces mismatching winding\n");

	TRY(fold_graph_extensions_validate(graph, &are_extensions, errors));
	*is = (is_still && are_extensions);
	return OK;
}

Error fold_graph_validate_inherited(FoldGraph* graph, bool* is, String* errors) {
	bool is_VC_view = graph->VC.is_view;
	bool is_VV_view = graph->VV.is_view;
	bool is_VE_view = graph->VE.is_view;
	bool is_VF_view = graph->VF.is_view;
	bool is_EV_view = graph->EV.is_view;
	bool is_EF_view = graph->EF.is_view;
	bool is_EA_view = graph->EA.is_view;
	bool is_EFA_view = graph->EFA.is_view;
	bool is_EL_view = graph->EL.is_view;
	bool is_EO_view = graph->EO.is_view;
	bool is_FV_view = graph->FV.is_view;
	bool is_FE_view = graph->FE.is_view;
	bool is_FF_view = graph->FF.is_view;
	bool is_FO_view = graph->FO.is_view;
	bool is_VCC_view = graph->extensions.VCC.is_view;
	bool is_VTC_view = graph->extensions.VTC.is_view;
	bool is_VNC_view = graph->extensions.VNC.is_view;
	bool is_FM_view = graph->extensions.FM.is_view;

	graph->VC.is_view = false;
	graph->VV.is_view = false;
	graph->VE.is_view = false;
	graph->VF.is_view = false;
	graph->EV.is_view = false;
	graph->EF.is_view = false;
	graph->EA.is_view = false;
	graph->EFA.is_view = false;
	graph->EL.is_view = false;
	graph->EO.is_view = false;
	graph->FV.is_view = false;
	graph->FE.is_view = false;
	graph->FF.is_view = false;
	graph->FO.is_view = false;
	graph->extensions.VCC.is_view = false;
	graph->extensions.VTC.is_view = false;
	graph->extensions.VNC.is_view = false;
	graph->extensions.FM.is_view = false;

	Error result = fold_graph_validate(graph, is, errors);

	graph->VC.is_view = is_VC_view;
	graph->VV.is_view = is_VV_view;
	graph->VE.is_view = is_VE_view;
	graph->VF.is_view = is_VF_view;
	graph->EV.is_view = is_EV_view;
	graph->EF.is_view = is_EF_view;
	graph->EA.is_view = is_EA_view;
	graph->EFA.is_view = is_EFA_view;
	graph->EL.is_view = is_EL_view;
	graph->EO.is_view = is_EO_view;
	graph->FV.is_view = is_FV_view;
	graph->FE.is_view = is_FE_view;
	graph->FF.is_view = is_FF_view;
	graph->FO.is_view = is_FO_view;
	graph->extensions.VCC.is_view = is_VCC_view;
	graph->extensions.VTC.is_view = is_VTC_view;
	graph->extensions.VNC.is_view = is_VNC_view;
	graph->extensions.FM.is_view = is_FM_view;

	return result;
}

/* ========================================================================= */
/* FOLD Frame Validation                                                     */
/* ========================================================================= */

static
Error fold_frame_metadata_validate(const FoldFrame* frame, bool* is, String* errors) {
	(void)frame; (void)errors;
	*is = true;
	return OK;
}

static
Error fold_frame_metadata_validate_inherited(FoldFrame* frame, bool* is, String* errors) {
	(void)frame; (void)errors;
	*is = true;
	return OK;
}

Error fold_frame_validate(const FoldFrame* frame, bool* is, String* errors) {
	*is = false;
	bool is_graph = true;
	bool is_metadata = true;
	TRY(fold_graph_validate(&frame->graph, &is_graph, errors));
	TRY(fold_frame_metadata_validate(frame, &is_metadata, errors));
	*is = (is_graph && is_metadata);
	return OK;
}

Error fold_frame_validate_inherited(FoldFrame* frame, bool* is, String* errors) {
	*is = false;
	bool is_graph = true;
	bool is_metadata = true;
	TRY(fold_graph_validate_inherited(&frame->graph, &is_graph, errors));
	TRY(fold_frame_metadata_validate_inherited(frame, &is_metadata, errors));
	*is = (is_graph && is_metadata);
	return OK;
}

/* ========================================================================= */
/* FOLD File Validation                                                      */
/* ========================================================================= */

Error fold_file_validate(FoldFile* file, bool* is, String* errors) {
	*is = false;
	bool are_frames = true;
	FOR_EACH(index, file->frames.size) {
		bool frame_parent_is = true;
		TRY(_validate_frame_parents(file, index,
			&frame_parent_is));

		if NOT(frame_parent_is) {
			if (errors != NULL) {
				char buffer[64];
				snprintf(buffer, sizeof(buffer),
					"ERROR: frame %zu has bad parents\n", index);
				TRY(string_append_raw(errors, buffer));
			}
			are_frames = false;
		}
	}
	if NOT(are_frames) return OK;

	fold_file_frames_inherit(file);
	ARRAY_FOR_EACH(&file->frames, index, FoldFrame*, frame) {
		STRING_CREATE(frame_errors);
		bool frame_is = true;

		TRY_OR_ELSE(fold_frame_validate(frame,
			&frame_is, &frame_errors),
			string_destroy(&frame_errors));

		if NOT(frame_is) {
			if (errors != NULL) {
				char buffer[32];
				snprintf(buffer, sizeof(buffer),
					"FRAME: %zu\n", index);

				TRY_OR_ELSE(string_append_raw(errors, buffer),
					string_destroy(&frame_errors));

				TRY_OR_ELSE(string_append(errors, &frame_errors),
					string_destroy(&frame_errors));
			}
			are_frames = false;
		}

		string_destroy(&frame_errors);
	}

	*is = are_frames;
	return OK;
}