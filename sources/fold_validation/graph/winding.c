#include "winding.h"

#include "collections/set.h"

static inline
Error _to_map2(const Array2* array2, Set* map2) {
	TRY_MULTIPLY(array2->data.size, 2);

	SET_CREATE(set, usize);
	TRY_OR_ELSE(set_reserve(&set, array2->data.size * 2),
		set_destroy(&set));

	set.hash = usize_hash_identity;
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
Error _to_map3(const Array2* array2, Set* map3) {
	TRY_MULTIPLY(array2->data.size, 2);

	SET_CREATE(set, usize);
	TRY_OR_ELSE(set_reserve(&set, array2->data.size * 2),
		set_destroy(&set));

	set.hash = usize_hash_identity;
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

void fold_graph_validate_VV_VE_winding(const FoldGraph* graph, bool* is) {
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
			i, usize*, vvi, vv.start, vv.end)
		{
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

Error fold_graph_validate_VV_VF_winding(const FoldGraph* graph, bool* is) {
	*is = true;
	if ((graph->VV.is_view && graph->VF.is_view && graph->FV.is_view) ||
		(graph->VV.size == 0 || graph->VF.size == 0 || graph->FV.size == 0))
		return OK;
	*is = false;

	Set FV_map;
	TRY(_to_map3(&graph->FV, &FV_map));
	ARRAY2_ITERATE(&graph->VV, vv) {
		usize vf_size = array2_size_at(&graph->VF, vv.index);
		if (vv.size != vf_size) {
			set_destroy(&FV_map);
			return OK;
		}

		ARRAY_FOR_EACH_IN_RANGE(&graph->VV.data,
			i, usize*, vvi, vv.start, vv.end)
		{
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

Error fold_graph_validate_VE_VF_winding(const FoldGraph* graph, bool* is) {
	*is = true;
	if ((graph->VE.is_view && graph->VF.is_view && graph->FE.is_view) ||
		(graph->VE.size == 0 || graph->VF.size == 0 || graph->FE.size == 0))
		return OK;
	*is = false;

	Set FE_map;
	TRY(_to_map2(&graph->FE, &FE_map));
	ARRAY2_ITERATE(&graph->VE, ve) {
		usize vf_size = array2_size_at(&graph->VF, ve.index);
		if (ve.size != vf_size) {
			set_destroy(&FE_map);
			return OK;
		}

		ARRAY_FOR_EACH_IN_RANGE(&graph->VE.data,
			i, usize*, vei, ve.start, ve.end)
		{
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

void fold_graph_validate_FV_FE_winding(const FoldGraph* graph, bool* is) {
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
			i, usize*, fvi, fv.start, fv.end)
		{
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

Error fold_graph_validate_FV_FF_winding(const FoldGraph* graph, bool* is) {
	*is = true;
	if ((graph->FV.is_view && graph->FF.is_view) ||
		(graph->FV.size == 0 || graph->FF.size == 0)) return OK;
	*is = false;
	usize ff_offset = 0;

	Set FV_map;
	TRY(_to_map2(&graph->FV, &FV_map));
	ARRAY2_ITERATE(&graph->FV, fv) {
		usize ff_size = array2_size_at(&graph->FF, fv.index);
		if (ff_size == 0) { ff_offset += fv.size; continue; }
		else if (fv.size != ff_size) {
			set_destroy(&FV_map);
			return OK;
		}

		ARRAY_FOR_EACH_IN_RANGE(&graph->FV.data,
			i, usize*, fvi, fv.start, fv.end)
		{
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

void fold_graph_validate_FE_FF_winding(const FoldGraph* graph, bool* is) {
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
				_, usize*, fei, fe.start, fe.end)
			{
				usize ef_start = array2_start_offset_at(&graph->EF, *fei);
				usize ef_end = array2_end_offset_at(&graph->EF, *fei);

				usize current_match_size = 0;
				bool current_face_found = false;
				ARRAY_FOR_EACH_IN_RANGE(&graph->EF.data,
					__, usize*, efi, ef_start, ef_end)
				{
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