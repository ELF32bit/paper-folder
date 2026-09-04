#include "reflexive.h"

#include "collections/set.h"

static inline
Error _to_map1(const Array2* array2, Set* map1, bool has_null) {
	SET_CREATE(set, usize);
	TRY_OR_ELSE(set_reserve(&set, array2->data.size),
		set_destroy(&set));

	set.hash = usize_hash_identity;
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
bool _validate_reflexive(const Array2* array2, const Set* map1, bool has_null) {
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

Error fold_graph_validate_VE_EV_reflexive(const FoldGraph* graph, bool* is) {
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
	TRY(_to_map1(&graph->VE, &VE_map, false));
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
Error fold_graph_validate_##A##_##B##_reflexive( \
	const FoldGraph* graph, bool* is) \
{ \
	*is = true; \
	if ((graph->A.is_view && graph->B.is_view) || \
		(graph->A.size == 0 || graph->B.size == 0)) return OK; \
	*is = false; \
	Set B##_map; \
	TRY(_to_map1(&graph->B, &B##_map, NOT(has_null))); \
	if (_validate_reflexive(&graph->A, &B##_map, has_null)) { \
		if (is_same) { *is = true; \
			set_destroy(&B##_map); return OK; } \
		Set A##_map; \
		TRY_OR_ELSE(_to_map1(&graph->A, &A##_map, has_null), \
			set_destroy(&B##_map)); \
		if (_validate_reflexive(&graph->B, &A##_map, NOT(has_null))) { \
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