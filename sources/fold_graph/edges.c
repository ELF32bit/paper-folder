#include "edges.h"
#include "attributes.h"

#include "math/vector2.h"
#include "math/vector3.h"

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

Error fold_graph_get_EV_map(const FoldGraph* graph, Map* map) {
	TRY_MULTIPLY(graph->EV.size, 2);

	MAP_CREATE(EV_map, usize, usize);
	TRY_OR_ELSE(map_reserve(&EV_map, graph->EV.size * 2),
		map_destroy(&EV_map));

	EV_map.hash = usize_hash_identity;
	ARRAY_FOR_EACH(&graph->EV, ei, FoldGraphEdge*, ev) {
		usize ab = hash_usize_mix2(ev->a, ev->b);
		TRY_OR_ELSE(map_add(&EV_map, &ab, &ei),
			map_destroy(&EV_map));

		usize ba = hash_usize_mix2(ev->b, ev->a);
		TRY_OR_ELSE(map_add(&EV_map, &ba, &ei),
			map_destroy(&EV_map));
	}

	*map = EV_map;
	return OK;
}

static inline
Error fold_graph_get_EF_map_from_FV(const FoldGraph* graph, Map* map) {
/*
	local EV = self.EV
	local EF_map, O_map = {}, {}
	local EV_map = self:get_EV_map()
	for fi, fv in ipairs(self.FV) do
		local d = #fv
		for i = 1, d do
			local a = fv[i]
			local b = fv[i % d + 1]
			local ei = EV_map[Hash2(a, b)]
			if ei ~= nil then
				local ev, ef = EV[ei], EF_map[ei]
				local o = (ev[1] == a and ev[2] == b)
				if ef == nil then
					EF_map[ei] = { fi }
					O_map[ei] = { o }
				else
					local ef_size = #ef
					ef[ef_size + 1] = fi
					O_map[ei][ef_size + 1] = o
				end
			end
		end
	end
	return EF_map, O_map
*/
	return OK;
}

static inline
Error fold_graph_EF_from_EF_map_unsorted(FoldGraph* graph,
	const Map* EF_map)
{
/*
	local EF = self.EF
	local EF_map, O_map = get_EF_maps(self)
	for ei = 1, #self.EV do
		local ef = EF_map[ei] or {}
		local ef_size = #ef
		if ef_size == 2 then
			local o = O_map[ei]
			local o1, o2 = o[1], o[2]
			if o1 == false and o2 == true then
				ef[1], ef[2] = ef[2], ef[1]
			end
		elseif ef_size == 1 then
			if O_map[ei][1] == true then
				ef[2] = "null"
			else
				ef[2] = ef[1]
				ef[1] = "null"
			end
		end
		EF[ei] = ef
	end
*/
	return OK;
}

static inline
Error fold_graph_EF_sort2D(FoldGraph* graph) {
/*
	local VC, Sort = self.VC, table.sort
	local Average = UTILITIES.Vector.average
	local Normalize = UTILITIES.Vector.normalize2
	local Atan = math.atan2 or math.atan
	local FV, EV = self.FV, self.EV

	for ei, ef in ipairs(self.EF) do
		local ef_size = #ef
		if ef_size > 2 then
			local ev = EV[ei]
			local a, b = VC[ev[1]], VC[ev[2]]
			local b1, b2 = b[1], b[2]

			local e1 = b1 - a[1]
			local e2 = b2 - a[2]
			e1, e2 = Normalize(e1, e2)

			local v1 = -e2
			local v2 = e1

			for i = 1, ef_size do
				local efi = ef[i]
				local c = Average(VC, FV[efi])
				local d1 = c[1] - b1
				local d2 = c[2] - b2

				local x = d1 * e1 + d2 * e2
				local y = d1 * v1 + d2 * v2

				_SORT_CACHE1[i] = efi
				_SORT_CACHE2[efi] = Atan(y, x)
			end

			Sort(_SORT_CACHE1, function(w1, w2)
				return _SORT_CACHE2[w1] < _SORT_CACHE2[w2]
			end)

			for i = 1, ef_size do
				local efi = _SORT_CACHE1[i]
				_SORT_CACHE2[efi] = nil
				_SORT_CACHE1[i] = nil
				ef[i] = efi
			end
		end
	end
*/
	return OK;
}

static inline
Error fold_graph_EF_sort3D(FoldGraph* graph) {
/*
	local VC, Sort = self.VC, table.sort
	local Average = UTILITIES.Vector.average
	local Normalize = UTILITIES.Vector.normalize3
	local Atan, Abs = math.atan2 or math.atan, math.abs
	local FV, EV = self.FV, self.EV

	for ei, ef in ipairs(self.EF) do
		local ef_size = #ef
		if ef_size > 2 then
			local ev = EV[ei]
			local a, b = VC[ev[1]], VC[ev[2]]
			local b1, b2, b3 = b[1], b[2], b[3]

			local e1 = b1 - a[1]
			local e2 = b2 - a[2]
			local e3 = b3 - a[3]
			e1, e2, e3 = Normalize(e1, e2, e3)
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

			for i = 1, ef_size do
				local efi = ef[i]
				local c = Average(VC, FV[efi])
				local d1 = c[1] - b1
				local d2 = c[2] - b2
				local d3 = c[3] - b3

				local x = d1 * u1 + d2 * u2 + d3 * u3
				local y = d1 * v1 + d2 * v2 + d3 * v3

				_SORT_CACHE1[i] = efi
				_SORT_CACHE2[efi] = Atan(y, x)
			end

			Sort(_SORT_CACHE1, function(w1, w2)
				return _SORT_CACHE2[w1] < _SORT_CACHE2[w2]
			end)

			for i = 1, ef_size do
				local efi = _SORT_CACHE1[i]
				_SORT_CACHE2[efi] = nil
				_SORT_CACHE1[i] = nil
				ef[i] = efi
			end
		end
	end
*/
	return OK;
}

static inline
Error fold_graph_EF_from_FV_unsorted(FoldGraph* graph) {
	Map EF_map;
	TRY(fold_graph_get_EF_map_from_FV(graph, &EF_map));
	TRY_OR_ELSE(fold_graph_EF_from_EF_map_unsorted(graph, &EF_map),
		map_destroy(&EF_map));
	map_destroy(&EF_map);
	return OK;
}

Error fold_graph_EF_from_FV(FoldGraph* graph, bool sorted) {
	ASSERT(NOT(graph->EF.is_view));
	TRY(fold_graph_EF_from_FV_unsorted(graph));
	if NOT(sorted) return OK;
	if (fold_graph_is_abstract(graph)) {
		return OK;
	} else if (fold_graph_is_2D(graph)) {
		TRY(fold_graph_EF_sort2D(graph));
	} else if (fold_graph_is_3D(graph)) {
		TRY(fold_graph_EF_sort3D(graph));
	}
	return OK;
}

Error fold_graph_EA_from_EF(FoldGraph* graph) {
	ASSERT(NOT(graph->EA.is_view));
	ASSERT(graph->EA.size == graph->EF.size);
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
			Vector2* A = array_get(&graph->VC, ev->a);
			Vector2* B = array_get(&graph->VC, ev->b);
			real length = vector2_distance(*A, *B);
			array_set(&graph->EL, ei, &length);
		}
	} else if (fold_graph_is_3D(graph)) {
		TRY(array_resize(&graph->EL, graph->EV.size));
		ARRAY_FOR_EACH(&graph->EV, ei, FoldGraphEdge*, ev) {
			Vector3* A = array_get(&graph->VC, ev->a);
			Vector3* B = array_get(&graph->VC, ev->b);
			real length = vector3_distance(*A, *B);
			array_set(&graph->EL, ei, &length);
		}
	}
	return OK;
}