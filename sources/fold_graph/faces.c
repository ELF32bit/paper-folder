#include "faces.h"
#include "edges.h"

#include "attributes.h"
#include "math/vector2.h"
#include "math/vector3.h"

Error fold_graph_FE_from_FV(FoldGraph* graph,
	const Map* NULLABLE(EV_map))
{
	ASSERT(NOT(graph->FE.is_view));

	Map _EV_map;
	if (EV_map == NULL) {
		TRY(fold_graph_get_EV_map(graph, &_EV_map));
		EV_map = &_EV_map;
	}

	TRY_OR_ELSE(
		array_resize(&graph->FE.data, graph->FV.data.size),
			if (EV_map == &_EV_map) map_destroy(&_EV_map));

	TRY_OR_ELSE(
		array_resize(&graph->FE.offsets, graph->FV.offsets.size),
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
			ASSERT(fei != NULL);

			array_set(&graph->FE.data, i, fei);
		}
		array_set(&graph->FE.offsets, fv.index, &fv.end);
	}

	if (EV_map == &_EV_map) map_destroy(&_EV_map);
	return OK;
}

static inline
Error fold_graph_get_FF_map_from_FV(const FoldGraph* graph, Map* map) {
/*
	local FF_map = {}
	for fi, fv in ipairs(self.FV) do
		local d = #fv
		for i = 1, d do
			local a = fv[i]
			local b = fv[i % d + 1]
			local ab = Hash2(a, b)
			local ff = FF_map[ab]
			if ff == nil then
				ff = {}; FF_map[ab] = ff;
				local ba = Hash2(b, a)
				FF_map[ba] = ff
			end
			ff[#ff + 1] = fi
		end
	end
	return FF_map
end
*/
	return OK;
}

Error fold_graph_FF_from_FV(FoldGraph* graph) {
	ASSERT(NOT(graph->FF.is_view));
/*
	local FF = self.FF; FF:clear();
	local FF_map = get_FF_map(self)
	for fi, fv in ipairs(self.FV) do
		local ff = {}; FF[fi] = ff;
		local ff_size = 0
		local d = #fv
		for i = 1, d do
			local a = fv[i]
			local b = fv[i % d + 1]
			local ab = Hash2(a, b)
			local has_ffi = false
			for _, ffi in ipairs(FF_map[ab]) do
				if ffi ~= fi then
					ff_size = ff_size + 1
					ff[ff_size] = ffi
					has_ffi = true
				end
			end
			if not has_ffi then
				ff_size = ff_size + 1
				ff[ff_size] = "null"
			end
		end
	end
*/
	return OK;
}

Error fold_graph_FV_triangulate(FoldGraph* graph) {
	ASSERT(NOT(graph->FV.is_view) || graph->FV.size == 0);
	ASSERT(NOT(graph->FO.is_view) || graph->FO.size == 0);
	ASSERT(NOT(graph->FO.is_view) || graph->FO.size == 0);
	ASSERT(NOT(graph->FT.is_view) || graph->FT.size == 0);
	ASSERT(NOT(graph->FT2.is_view) || graph->FT2.size == 0);
	ASSERT(NOT(graph->FN.is_view) || graph->FN.size == 0);
	ASSERT(NOT(graph->FM.is_view) || graph->FM.size == 0);

	if NOT(fold_graph_is_abstract(graph)) {
		ASSERT(NOT(graph->VC.is_view));
		ASSERT(NOT(graph->VP.is_view) || graph->VP.size == 0);
		ASSERT(NOT(graph->TC.is_view) || graph->TC.size == 0);
		ASSERT(NOT(graph->T2C.is_view) || graph->T2C.size == 0);
		ASSERT(NOT(graph->NC.is_view) || graph->NC.size == 0);
	}

	array2_recreate(&graph->VV);
	array2_recreate(&graph->VE);
	array2_recreate(&graph->VF);
	array2_recreate(&graph->EF);
	array2_recreate(&graph->FE);
	array2_recreate(&graph->FF);

	usize FV_offset = 0;
	ARRAY2_CREATE(new_FV, usize);
	ARRAY2_CREATE(new_FT, usize);
	ARRAY2_CREATE(new_FT2, usize);
	ARRAY2_CREATE(new_FN, usize);
	ARRAY_CREATE(new_FM, usize);

	usize FO_offset = 0;
	ARRAY2_CREATE(FO_map, usize);
	if (graph->FO.size > 0) {
		TRY(array_resize(&FO_map.offsets,
			graph->FV.size));
	}

#define _CLEANUP \
	array2_destroy(&new_FV); \
	array2_destroy(&new_FT); \
	array2_destroy(&new_FT2); \
	array2_destroy(&new_FN); \
	array_destroy(&new_FM); \
	array2_destroy(&FO_map)

#define _APPEND2(F_, i0, i1, i2, O) \
	if (graph->F_.size > 0) { \
		TRY_OR_ELSE(array_append(&new_##F_.data, \
			array_get(&graph->F_.data, fv.start + i0)), \
				_CLEANUP); \
		TRY_OR_ELSE(array_append(&new_##F_.data, \
			array_get(&graph->F_.data, fv.start + i1)), \
				_CLEANUP); \
		TRY_OR_ELSE(array_append(&new_##F_.data, \
			array_get(&graph->F_.data, fv.start + i2)), \
				_CLEANUP); \
		ARRAY_APPEND_OR_ELSE(&new_##F_.offsets, \
			usize, FV_offset O, \
				_CLEANUP); \
	}

#define _APPEND3(i0, i1, i2, O) \
	_APPEND2(FV, i0, i1, i2, O) \
	_APPEND2(FT, i0, i1, i2,) \
	_APPEND2(FT2, i0, i1, i2,) \
	_APPEND2(FN, i0, i1, i2,) \
	if (graph->FM.size > 0) { \
		TRY_OR_ELSE(array_append(&new_FM, \
			array_get(&graph->FM, fv.index)), \
				_CLEANUP); \
	}

#define _APPEND0(i) \
	ARRAY_APPEND_OR_ELSE(&FO_map.data, \
		usize, new_FV.size + i, \
			_CLEANUP);

#define _OFFSET0(O) \
	ARRAY_SET(&FO_map.offsets, fv.index, \
		usize, FO_offset O); \

	ARRAY2_ITERATE(&graph->FV, fv) {
		if (fv.size <= 3) {
			if (graph->FO.size > 0) {
				_APPEND0(0);
				_OFFSET0(+= 1);
			}

			ASSERT(fv.size == 3);
			_APPEND3(0, 1, 2, += fv.size);

			continue;
		} else if (fv.size == 4) {
			if (graph->FO.size > 0) {
				_APPEND0(0);
				_APPEND0(1);
				_OFFSET0(+= 2);
			}

			if (fold_graph_is_abstract(graph)) {
				_APPEND3(0, 1, 2, += 3);
				_APPEND3(0, 2, 3, += 3);
				continue;
			}

			real d1 = 0.0, d2 = 0.0;
			if (fold_graph_is_2D(graph)) {
				Vector2* v0 = array_get(&graph->FV.data, fv.start + 0);
				Vector2* v1 = array_get(&graph->FV.data, fv.start + 1);
				Vector2* v2 = array_get(&graph->FV.data, fv.start + 2);
				Vector2* v3 = array_get(&graph->FV.data, fv.start + 3);
				d1 = vector2_distance2(*v0, *v2);
				d2 = vector2_distance2(*v1, *v3);
			} else if (fold_graph_is_3D(graph)) {
				Vector3* v0 = array_get(&graph->FV.data, fv.start + 0);
				Vector3* v1 = array_get(&graph->FV.data, fv.start + 1);
				Vector3* v2 = array_get(&graph->FV.data, fv.start + 2);
				Vector3* v3 = array_get(&graph->FV.data, fv.start + 3);
				d1 = vector3_distance2(*v0, *v2);
				d2 = vector3_distance2(*v1, *v3);
			}

			/* TODO: can fail for concave quads */
			if (d1 <= d2) {
				_APPEND3(0, 1, 2, += 3);
				_APPEND3(0, 2, 3, += 3);
			} else {
				_APPEND3(0, 1, 3, += 3);
				_APPEND3(2, 3, 1, += 3);
			}

			continue;
		} else if (fv.size > 4) {
			if (fold_graph_is_abstract(graph)) {
todo_centroids:
				FOR_EACH_IN_RANGE(index, 1, fv.size - 1) {
					if (graph->FO.size > 0) {
						_APPEND0(0);
						_OFFSET0(+= 1);
					}
					_APPEND3(0, index, index + 1, += 3);
				}
				continue;
			}

			if (fold_graph_is_2D(graph)) {
				goto todo_centroids;
			} else if (fold_graph_is_3D(graph)) {
				goto todo_centroids;
			}

			continue;
		}
	}

#undef _OFFSET0
#undef _APPEND0
#undef _APPEND3
#undef _APPEND2
#undef _CLEANUP

	if (graph->FV.size > 0) {
		array2_destroy(&graph->FV);
		graph->FV = new_FV;
	}
	if (graph->FT.size > 0) {
		array2_destroy(&graph->FT);
		graph->FT = new_FT;
	}
	if (graph->FT2.size > 0) {
		array2_destroy(&graph->FT2);
		graph->FT2 = new_FT2;
	}
	if (graph->FN.size > 0) {
		array2_destroy(&graph->FN);
		graph->FN = new_FN;
	}
	if (graph->FM.size > 0) {
		array_destroy(&graph->FM);
		graph->FM = new_FM;
	}

	if (graph->FO.size > 0) {
		ARRAY_CREATE(new_FO, FoldGraphFaceOrder);
		ARRAY_FOR_EACH(&graph->FO, oi, FoldGraphOrder*, fo) {
			ARRAY_FOR_EACH_IN_RANGE(&FO_map.data, _, usize*, a_remap,
				array2_start_offset_at(&FO_map, fo->a),
				array2_end_offset_at(&FO_map, fo->a))
			{
				ARRAY_FOR_EACH_IN_RANGE(&FO_map.data, __, usize*, b_remap,
					array2_start_offset_at(&FO_map, fo->b),
					array2_end_offset_at(&FO_map, fo->b))
				{
					FoldGraphOrder order = {
						.a = *a_remap,
						.b = *b_remap,
						.o = fo->o
					};
					TRY_OR_ELSE(array_append(&new_FO, &order),
						array2_destroy(&FO_map);
						array_destroy(&new_FO));
				}
			}
		}

		array_destroy(&graph->FO);
		graph->FO = new_FO;
	}

	return OK;
}

void fold_graph_FV_flip(FoldGraph* graph) {
	ASSERT(NOT(graph->VV.is_view) || graph->VV.size == 0);
	ASSERT(NOT(graph->VE.is_view) || graph->VE.size == 0);
	ASSERT(NOT(graph->VF.is_view) || graph->VF.size == 0);
	ASSERT(NOT(graph->EV.is_view) || graph->EV.size == 0);
	ASSERT(NOT(graph->EA.is_view) || graph->EA.size == 0);
	ASSERT(NOT(graph->EFA.is_view) || graph->EFA.size == 0);
	ASSERT(NOT(graph->FV.is_view) || graph->FV.size == 0);
	ASSERT(NOT(graph->FE.is_view) || graph->FE.size == 0);
	ASSERT(NOT(graph->FF.is_view) || graph->FF.size == 0);
	ASSERT(NOT(graph->FO.is_view) || graph->FO.size == 0);
	ASSERT(NOT(graph->NC.is_view) || graph->NC.size == 0);
	ASSERT(NOT(graph->FT.is_view) || graph->FT.size == 0);
	ASSERT(NOT(graph->FT2.is_view) || graph->FT2.size == 0);
	ASSERT(NOT(graph->FN.is_view) || graph->FN.size == 0);

	ARRAY2_ITERATE(&graph->VV, vv) {
		array_reverse_range(&graph->VV.data, vv.start, vv.end);
	}

	ARRAY2_ITERATE(&graph->VE, ve) {
		array_reverse_range(&graph->VE.data, ve.start, ve.end);
	}

	ARRAY2_ITERATE(&graph->VF, vf) {
		array_reverse_range(&graph->VF.data, vf.start, vf.end);
		array_shift_range(&graph->VF.data, -1, vf.start, vf.end);
	}

	ARRAY_FOR_EACH(&graph->EV, ei, FoldGraphEdge*, ev) {
		SWAP(ev->a, ev->b);
	}

	ARRAY_FOR_EACH(&graph->EA, ei, FoldGraphEdgeAssignment*, ea) {
		if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN) {
			ARRAY_SET(&graph->EA, ei,
				FoldGraphEdgeAssignment,
				FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY);
		} else if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY) {
			ARRAY_SET(&graph->EA, ei,
				FoldGraphEdgeAssignment,
				FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN);
		}
	}

	ARRAY_FOR_EACH(&graph->EFA, ei, real*, efa) {
		ARRAY_SET(&graph->EFA, ei, real, (-1.0) * (*efa));
	}

	ARRAY2_ITERATE(&graph->FV, fv) {
		array_reverse_range(&graph->FV.data, fv.start, fv.end);
	}

	ARRAY2_ITERATE(&graph->FE, fe) {
		array_reverse_range(&graph->FE.data, fe.start, fe.end);
		array_shift_range(&graph->FE.data, -1, fe.start, fe.end);
	}

	ARRAY2_ITERATE(&graph->FF, ff) {
		array_reverse_range(&graph->FF.data, ff.start, ff.end);
		array_shift_range(&graph->FF.data, -1, ff.start, ff.end);
	}

	ARRAY_FOR_EACH(&graph->FO, oi, FoldGraphOrder*, fo) {
		FoldGraphOrder order = *fo;
		order.o = FOLD_GRAPH_ORDER_INVERT(fo->o);
		array_set(&graph->FO, oi, &order);
	}

	ARRAY_FOR_EACH(&graph->NC, ni, Vector3*, nc) {
		Vector3 normal = vector3_scale(*nc, -1.0);
		array_set(&graph->NC, ni, &normal);
	}

	ARRAY2_ITERATE(&graph->FT, ft) {
		array_reverse_range(&graph->FT.data, ft.start, ft.end);
	}

	ARRAY2_ITERATE(&graph->FT2, ft2) {
		array_reverse_range(&graph->FT2.data, ft2.start, ft2.end);
	}

	ARRAY2_ITERATE(&graph->FN, fn) {
		array_reverse_range(&graph->FN.data, fn.start, fn.end);
	}
}