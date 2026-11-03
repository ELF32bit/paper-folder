#include "vertices.h"
#include "attributes.h"
#include "properties.h"
#include "edges.h"

#include <stdlib.h>

struct WeightedIndex { usize index; real weight; };

static inline
int _weighted_index_sort_ascending(const void* a, const void* b) {
	real w1 = ((const struct WeightedIndex*)a)->weight;
	real w2 = ((const struct WeightedIndex*)b)->weight;
	if (w1 < w2) return -1;
	if (w1 > w2) return 1;
	return 0;
}

static inline
Error fold_graph_get_VV_map_from_EV(const FoldGraph* graph, Map* map) {
	TRY_MULTIPLY(graph->EV.size, 2);

	MAP_CREATE_MANAGED_VALUES(VV_map, usize, Set);
	TRY_OR_ELSE(map_reserve(&VV_map, graph->EV.size * 2),
		map_destroy(&VV_map));

	VV_map.hash = usize_hash_identity;
	ARRAY_FOR_EACH(&graph->EV, _, FoldGraphEdge*, ev) {
		usize a_hash = hash_usize(ev->a);
		Set* a_set = map_get(&VV_map, &a_hash, NULL);
		if (a_set == NULL) {
			SET_CREATE(vv_set, usize);
			TRY_OR_ELSE(set_add(&vv_set, &ev->b),
				set_destroy(&vv_set); map_destroy(&VV_map));

			TRY_OR_ELSE(map_add(&VV_map, &a_hash, &vv_set),
				set_destroy(&vv_set); map_destroy(&VV_map));
		} else {
			TRY_OR_ELSE(set_add(a_set, &ev->b),
				map_destroy(&VV_map));
		}

		usize b_hash = hash_usize(ev->b);
		Set* b_set = map_get(&VV_map, &b_hash, NULL);
		if (b_set == NULL) {
			SET_CREATE(vv_set, usize);
			TRY_OR_ELSE(set_add(&vv_set, &ev->a),
				set_destroy(&vv_set); map_destroy(&VV_map));

			TRY_OR_ELSE(map_add(&VV_map, &b_hash, &vv_set),
				set_destroy(&vv_set); map_destroy(&VV_map));
		} else {
			TRY_OR_ELSE(set_add(b_set, &ev->a),
				map_destroy(&VV_map));
		}
	}

	*map = VV_map;
	return OK;
}

static inline
Error fold_graph_get_VV_map_from_FV(const FoldGraph* graph, Map* map) {
	TRY_MULTIPLY(graph->FV.size, 3);

	MAP_CREATE_MANAGED_VALUES(VV_map, usize, Set);
	TRY_OR_ELSE(map_reserve(&VV_map, graph->FV.size * 3),
		map_destroy(&VV_map));

	VV_map.hash = usize_hash_identity;
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
				SET_CREATE(vv_set, usize);
				TRY_OR_ELSE(set_add(&vv_set, a),
					set_destroy(&vv_set); map_destroy(&VV_map));
				TRY_OR_ELSE(set_add(&vv_set, c),
					set_destroy(&vv_set); map_destroy(&VV_map));

				TRY_OR_ELSE(map_add(&VV_map, &b_hash, &vv_set),
					set_destroy(&vv_set); map_destroy(&VV_map));
			} else {
				TRY_OR_ELSE(set_add(b_set, a),
					map_destroy(&VV_map));
				TRY_OR_ELSE(set_add(b_set, c),
					map_destroy(&VV_map));
			}
		}
	}

	*map = VV_map;
	return OK;
}

static inline
Error fold_graph_VV_from_VV_map_unsorted(FoldGraph* graph,
	const Map* VV_map)
{
	array2_recreate(&graph->VV);
	array2_recreate(&graph->VE);
	array2_recreate(&graph->VF);

	usize size = fold_graph_is_abstract(graph)
		? fold_graph_get_abstract_size(graph)
		: graph->VC.size;

	usize data_size = 0;
	MAP_ITERATE(VV_map, vv) {
		Set* set = vv.value;
		TRY_ADD(data_size, set->size);
		data_size += set->size;
	}

	TRY(array_resize(&graph->VV.data, data_size));
	TRY_OR_ELSE(array_resize(&graph->VV.offsets, size),
		array_recreate(&graph->VV.data));

	usize offset = 0;
	FOR_EACH(vi, size) {
		usize a_hash = hash_usize(vi);
		Set* vv = map_get(VV_map, &a_hash, NULL);
		if (vv != NULL) {
			SET_ITERATE(vv, vvi) {
				array_set(&graph->VV.data,
					offset + vvi.index, vvi.key);
			}
			offset += vv->size;
		}
		array_set(&graph->VV.offsets, vi, &offset);
	}

	return OK;
}

static inline
Error fold_graph_VV_from_EV_unsorted(FoldGraph* graph) {
	Map VV_map;
	TRY(fold_graph_get_VV_map_from_EV(graph, &VV_map));
	TRY_OR_ELSE(fold_graph_VV_from_VV_map_unsorted(graph, &VV_map),
		map_destroy(&VV_map));
	map_destroy(&VV_map);
	return OK;
}

static inline
Error fold_graph_VV_sort2D(FoldGraph* graph) {
	usize buffer_size = 0;
	ARRAY2_ITERATE(&graph->VV, array) {
		buffer_size = MAX(buffer_size, array.size);
	}

	if (buffer_size < 3) return OK;
	struct WeightedIndex* buffer = malloc(
		SIZEOF(struct WeightedIndex, buffer_size));
	TRY_NEW_MEMORY(buffer);

	ARRAY2_ITERATE(&graph->VV, vv) {
		if (vv.size < 3) continue;

		Vector2* A = array_get(&graph->VC, vv.index);
		ARRAY_FOR_EACH_IN_RANGE(&graph->VV.data,
			i, usize*, vvi, vv.start, vv.end)
		{
			Vector2* B = array_get(&graph->VC, *vvi);

			Vector2 AB = vector2_direction(*A, *B);
			buffer[i - vv.start] = (struct WeightedIndex){
				.weight = ATAN2(AB.y, AB.x),
				.index = *vvi
			};
		}

		qsort(buffer, vv.size,
			sizeof(struct WeightedIndex),
			_weighted_index_sort_ascending);

		FOR_EACH_IN_RANGE(i, vv.start, vv.end) {
			array_set(&graph->VV.data, i,
				&(buffer[i - vv.start].index));
		}
	}

	free(buffer);
	return OK;
}

/* TODO: Generate covariance matrix to obtain 3D plane and project */
static inline
Error fold_graph_VV_sort3D(FoldGraph* graph) {
	(void)graph;
	return OK;
}

Error fold_graph_VV_from_EV(FoldGraph* graph, bool sorted) {
	ASSERT(NOT(graph->VV.is_view));
	TRY(fold_graph_VV_from_EV_unsorted(graph));
	if NOT(sorted) return OK;
	if (fold_graph_is_abstract(graph)) {
		return OK;
	} else if (fold_graph_is_2D(graph)) {
		TRY(fold_graph_VV_sort2D(graph));
	} else if (fold_graph_is_3D(graph)) {
		TRY(fold_graph_VV_sort3D(graph));
	}
	return OK;
}

static inline
Error fold_graph_VV_from_FV_unsorted(FoldGraph* graph) {
	Map VV_map;
	TRY(fold_graph_get_VV_map_from_FV(graph, &VV_map));
	TRY_OR_ELSE(fold_graph_VV_from_VV_map_unsorted(graph, &VV_map),
		map_destroy(&VV_map));
	map_destroy(&VV_map);
	return OK;
}

Error fold_graph_VV_from_FV(FoldGraph* graph, bool sorted) {
	ASSERT(NOT(graph->VV.is_view));
	TRY(fold_graph_VV_from_FV_unsorted(graph));
	if NOT(sorted) return OK;
	if (fold_graph_is_abstract(graph)) {
		return OK;
	} else if (fold_graph_is_2D(graph)) {
		TRY(fold_graph_VV_sort2D(graph));
	} else if (fold_graph_is_3D(graph)) {
		TRY(fold_graph_VV_sort3D(graph));
	}
	return OK;
}

Error fold_graph_VE_from_VV(FoldGraph* graph,
	const Map* NULLABLE(EV_map))
{
	ASSERT(NOT(graph->VE.is_view));

	Map _EV_map;
	if (EV_map == NULL) {
		TRY(fold_graph_get_EV_map(graph, &_EV_map));
		EV_map = &_EV_map;
	}

	TRY_OR_ELSE(
		array_resize(&graph->VE.data, graph->VV.data.size),
			if (EV_map == &_EV_map) map_destroy(&_EV_map));

	TRY_OR_ELSE(
		array_resize(&graph->VE.offsets, graph->VV.offsets.size),
			if (EV_map == &_EV_map) map_destroy(&_EV_map);
			array2_recreate(&graph->VE));

	ARRAY2_ITERATE(&graph->VV, vv) {
		ARRAY_FOR_EACH_IN_RANGE(&graph->VV.data,
			i, usize*, vvi, vv.start, vv.end)
		{
			usize ab = hash_usize_mix2(vv.index, *vvi);
			usize* vei = map_get(EV_map, &ab, NULL);
			ASSERT(vei != NULL);

			array_set(&graph->VE.data, i, vei);
		}
		array_set(&graph->VE.offsets, vv.index, &vv.end);
	}

	if (EV_map == &_EV_map) map_destroy(&_EV_map);
	return OK;
}

/* TODO: Reconstruct faces by walking over directed VV edges */
Error fold_graph_VF_from_VV(FoldGraph* graph) {
	(void)graph;
	return OK;
}