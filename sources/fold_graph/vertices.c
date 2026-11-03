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

static inline
Error fold_graph_VV_sort3D(FoldGraph* graph) {
/*
	local VC, Sort = self.VC, table.sort
	local Normalize = UTILITIES.Vector.normalize3
	local Atan, Abs = math.atan2 or math.atan, math.abs

	for vi, vv in ipairs(self.VV) do
		local c, vv_size = VC[vi], #vv
		local c1, c2, c3 = c[1], c[2], c[3]
		local m11, m12, m13 = 0.0, 0.0, 0.0
		local m22, m23, m33 = 0.0, 0.0, 0.0

		for i = 1, vv_size do
			local v = VC[vv[i]]
			local d1 = v[1] - c1
			local d2 = v[2] - c2
			local d3 = v[3] - c3

			m11 = m11 + d1 * d1
			m12 = m12 + d1 * d2
			m13 = m13 + d1 * d3
			m22 = m22 + d2 * d2
			m23 = m23 + d2 * d3
			m33 = m33 + d3 * d3
		end

		local e1, e2, e3 = 1, 0, 0
		if m22 >= m11 and m22 >= m33 then
			e1, e2, e3 = 0, 1, 0
		elseif m33 >= m11 and m33 >= m22 then
			e1, e2, e3 = 0, 0, 1
		end

		for i = 1, 8 do
			local n1 = m11 * e1 + m12 * e2 + m13 * e3
			local n2 = m12 * e1 + m22 * e2 + m23 * e3
			local n3 = m13 * e1 + m23 * e2 + m33 * e3

			n1, n2, n3 = Normalize(n1, n2, n3)
			local dot = e1 * n1 + e2 * n2 + e3 * n3
			e1, e2, e3 = n1, n2, n3

			if Abs(dot) > 0.999 then break end
		end
		local e1A, e2A, e3A = Abs(e1), Abs(e2), Abs(e3)

		local p1, p2, p3 = 1, 0, 0
		if e2A <= e1A and e2A <= e3A then
			p1, p2, p3 = 0, 1, 0
		elseif e3A <= e1A and e3A <= e2A then
			p1, p2, p3 = 0, 0, 1
		end

		local u1 = e2 * p3 - e3 * p2
		local u2 = e3 * p1 - e1 * p3
		local u3 = e1 * p2 - e2 * p1
		u1, u2, u3 = Normalize(u1, u2, u3)

		local v1 = e2 * u3 - e3 * u2
		local v2 = e3 * u1 - e1 * u3
		local v3 = e1 * u2 - e2 * u1

		for i = 1, vv_size do
			local vvi = vv[i]
			local v = VC[vvi]
			local d1 = v[1] - c1
			local d2 = v[2] - c2
			local d3 = v[3] - c3

			local x = d1 * u1 + d2 * u2 + d3 * u3
			local y = d1 * v1 + d2 * v2 + d3 * v3

			_SORT_CACHE1[i] = vvi
			_SORT_CACHE2[vvi] = Atan(y, x)
		end

		Sort(_SORT_CACHE1, function(w1, w2)
			return _SORT_CACHE2[w1] < _SORT_CACHE2[w2]
		end)

		for i = 1, vv_size do
			local vvi = _SORT_CACHE1[i]
			_SORT_CACHE2[vvi] = nil
			_SORT_CACHE1[i] = nil
			vv[i] = vvi
		end
	end
*/
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
/* ! WRONG OLD ALGORITHM !
	var FV_map: Dictionary = {}
	for fi in range(graph.FV.size()):
		var fv: Array = graph.FV[fi]
		var d := fv.size()
		for i in range(d):
			var u: int = fv[i]
			var v: int = fv[(i + 1) % d]
			var w: int = fv[(i + 2) % d]
			FV_map.get_or_add([u, v, w], {})[fi] = true
			FV_map.get_or_add([w, v, u], {})[fi] = true

	var new_VF: Array[Array] = []
	new_VF.resize(graph.VV.size())
	for vi in range(graph.VV.size()):
		var vv: Array = graph.VV[vi]
		var vf: Array = new_VF[vi]
		var d := vv.size()
		vf.resize(d)
		for i in range(d):
			var vvi: int = vv[i]
			var vnvi: int = vv[(i + 1) % d]
			if FV_map.has([vvi, vi, vnvi]):
				vf.append_array(FV_map[[vvi, vi, vnvi]].keys())
			elif FV_map.has([vnvi, vi, vvi]):
				vf.append_array(FV_map[[vnvi, vi, vvi]].keys())
			else: vf.append(null)
*/
	return OK;
}