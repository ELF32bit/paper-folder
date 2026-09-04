#include "problems.h"

#include "fold_graph/vertices.h"
#include "fold_graph/properties.h"
#include "definitions/constants.h"

#include "triangle.h"

struct ProblemParameters {
	union {
		struct LFFParameters parameters;
		struct {
			usize min_boundary_vertices;
			usize max_boundary_vertices;
			usize min_interior_vertices;
			usize max_interior_vertices;
			real boundary_offset;
			real cell_size;
			real kawasaki_error;
			usize seed;
		};
	};
	bool success;
	RNG* rng;
};

static inline
void _triangulateio_create(struct triangulateio* io) {
	io->pointlist = NULL;
	io->pointattributelist = NULL;
	io->pointmarkerlist = NULL;
	io->numberofpoints = 0;
	io->numberofpointattributes = 0;
	io->trianglelist = NULL;
	io->triangleattributelist = NULL;
	io->trianglearealist = NULL;
	io->neighborlist = NULL;
	io->numberoftriangles = 0;
	io->numberofcorners = 0;
	io->numberoftriangleattributes = 0;
	io->segmentlist = NULL;
	io->segmentmarkerlist = NULL;
	io->numberofsegments = 0;
	io->holelist = NULL;
	io->numberofholes = 0;
	io->regionlist = NULL;
	io->numberofregions = 0;
	io->edgelist = NULL;
	io->edgemarkerlist = NULL;
	io->normlist = NULL;
	io->numberofedges = 0;
}

static inline
void _triangulateio_destroy(struct triangulateio* io) {
	trifree(io->pointlist);
	trifree(io->pointattributelist);
	trifree(io->pointmarkerlist);
	trifree(io->trianglelist);
	trifree(io->triangleattributelist);
	trifree(io->trianglearealist);
	trifree(io->neighborlist);
	trifree(io->segmentlist);
	trifree(io->segmentmarkerlist);
	trifree(io->holelist);
	trifree(io->regionlist);
	trifree(io->edgelist);
	trifree(io->edgemarkerlist);
	trifree(io->normlist);
}

static inline
Error generate_fold_graph_step6(FoldGraph* graph,
	struct ProblemParameters* parameters)
{
	RNG* rng = parameters->rng;
	ARRAY_FOR_EACH(&graph->EA, ei, FoldGraphEdgeAssignment*, ea) {
		if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_JOIN) {
			ARRAY_SET(&graph->EA, ei,
				FoldGraphEdgeAssignment, random_bool(rng)
					? FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN
					: FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY);
		}
	}

	while NOT(fold_graph_is_maekawa_compliant(graph)) {
		ARRAY_FOR_EACH(&graph->EA, ei, FoldGraphEdgeAssignment*, ea) {
			if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN ||
				*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY) {
				ARRAY_SET(&graph->EA, ei,
					FoldGraphEdgeAssignment, random_bool(rng)
						? FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN
						: FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY);
			}
		}
	}

	parameters->success = true;
	return OK;
}

static inline
Error generate_fold_graph_step5(FoldGraph* graph,
	struct ProblemParameters* parameters)
{
	TRY(fold_graph_VV_from_EV(graph, true));
	TRY(fold_graph_VE_from_VV(graph, NULL));

	if NOT(fold_graph_is_kawasaki_compatible(
		graph, parameters->kawasaki_error)) {
		return OK;
	}

	TRY(generate_fold_graph_step6(graph, parameters));
	return OK;
}

static inline
Error generate_fold_graph_step4(FoldGraph* graph,
	struct ProblemParameters* parameters)
{
	struct triangulateio in;
	struct triangulateio out;
	_triangulateio_create(&in);
	_triangulateio_create(&out);

	in.pointlist = graph->VC.data;
	in.numberofpoints = graph->VC.size;

	triangulate("zeQ", &in, &out, NULL);

	TRY_OR_ELSE(array_resize(&graph->EV, out.numberofedges),
		_triangulateio_destroy(&out));
	TRY_OR_ELSE(array_resize(&graph->EA, out.numberofedges),
		_triangulateio_destroy(&out));
	TRY_OR_ELSE(array_resize(&graph->FV.data, out.numberoftriangles * 3),
		_triangulateio_destroy(&out));
	TRY_OR_ELSE(array_resize(&graph->FV.offsets, out.numberoftriangles),
		_triangulateio_destroy(&out));

	FOR_EACH(i, out.numberofedges) {
		usize a = out.edgelist[i * 2 + 0];
		usize b = out.edgelist[i * 2 + 1];
		ARRAY_SET(&graph->EV, i, FoldGraphEdge,
			{ .a = a, .b = b });

		if (out.edgemarkerlist[i] == 0) {
			ARRAY_SET(&graph->EA, i, FoldGraphEdgeAssignment,
				FOLD_GRAPH_EDGE_ASSIGNMENT_JOIN);
		} else {
			ARRAY_SET(&graph->EA, i, FoldGraphEdgeAssignment,
				FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY);
		}
	}

	FOR_EACH(i, out.numberoftriangles) {
		usize a = out.trianglelist[i * 3 + 0];
		usize b = out.trianglelist[i * 3 + 1];
		usize c = out.trianglelist[i * 3 + 2];
		array_set(&graph->FV.data, i * 3 + 0, &a);
		array_set(&graph->FV.data, i * 3 + 1, &b);
		array_set(&graph->FV.data, i * 3 + 2, &c);
		ARRAY_SET(&graph->FV.offsets, i, usize,
			i * 3 + 3);
	}

	_triangulateio_destroy(&out);
	TRY(generate_fold_graph_step5(graph, parameters));
	return OK;
}

static inline
Error generate_fold_graph_step3(FoldGraph* graph,
	struct ProblemParameters* parameters)
{
	SET_CREATE(set, usize);
	set.hash = usize_hash_identity;

	ARRAY_CREATE(unique_vertices, Vector2);
	TRY_OR_ELSE(array_resize(&unique_vertices, graph->VC.size),
		array_destroy(&unique_vertices);
		set_destroy(&set));

	unique_vertices.size = 0;
	usize unique_boundary_vertices = 0;
	usize unique_interior_vertices = 0;
	ARRAY_FOR_EACH(&graph->VC, vi, Vector2*, vc) {
		isize ix = (isize)(vc->x / parameters->cell_size);
		isize iy = (isize)(vc->y / parameters->cell_size);

		bool is_duplicate = false;
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				usize cell = (usize)hash_isize_pack2(ix + dx, iy + dy);
				if (set_has(&set, &cell)) {
					is_duplicate = true;
					break;
				}
			}
			if (is_duplicate) break;
		}
		if (is_duplicate) continue;

		usize cell = (usize)hash_isize_pack2(ix, iy);
		TRY_OR_ELSE(set_add(&set, &cell, NULL),
			array_destroy(&unique_vertices);
			set_destroy(&set));

		unique_vertices.size++;
		array_set(&unique_vertices,
			unique_vertices.size - 1, vc);

		if (vc->x == 0.0 || vc->y == 0.0 ||
			vc->x == 1.0 || vc->y == 1.0) {
			unique_boundary_vertices++;
		} else {
			unique_interior_vertices++;
		}
	}

	set_destroy(&set);
	TRY_OR_ELSE(array_resize(&unique_vertices, unique_vertices.size),
		array_destroy(&unique_vertices));

	free(graph->VC.data);
	graph->VC = unique_vertices;

	if (unique_boundary_vertices <
		parameters->min_boundary_vertices) {
		return OK;
	}
	if (unique_interior_vertices <
		parameters->min_interior_vertices) {
		return OK;
	}

	TRY(generate_fold_graph_step4(graph, parameters));
	return OK;
}

static inline
Error generate_fold_graph_step2(FoldGraph* graph,
	struct ProblemParameters* parameters)
{
	RNG* rng = parameters->rng;
	usize n1 = random_usize_in_range(rng,
		parameters->min_boundary_vertices,
		parameters->max_boundary_vertices);
	usize n2 = random_usize_in_range(rng,
		parameters->min_interior_vertices,
		parameters->max_interior_vertices);
	real offset = parameters->boundary_offset;

	usize old_size = graph->VC.size;
	TRY_ADD(old_size, n1); TRY_ADD(old_size + n1, n2);
	TRY(array_resize(&graph->VC, old_size + n1 + n2));

	FOR_EACH_IN_RANGE(i, old_size, old_size + n1) {
		real t = random_real_in_range(rng, 0.0, 4.0);
		usize side = (usize)t;
		if (side > 3) side = 3;
		t -= side;
		Vector2 v = {
			.x = (side == 0) ? t
				: (side == 1) ? 1.0
				: (side == 2) ? t
				: 0.0,
			.y = (side == 0) ? 0.0
				: (side == 1) ? t
				: (side == 2) ? 1.0
				: t
		};
		array_set(&graph->VC, i, &v);
	}

	FOR_EACH_IN_RANGE(i, old_size + n1, old_size + n1 + n2) {
		Vector2 v = {
			.x = random_real_in_range(rng, offset, 1.0 - offset),
			.y = random_real_in_range(rng, offset, 1.0 - offset)};
		array_set(&graph->VC, i, &v);
	}

	TRY(generate_fold_graph_step3(graph, parameters));
	return OK;
}

static inline
Error generate_fold_graph(FoldGraph* graph,
	struct ProblemParameters* parameters)
{
	TRY(array_resize(&graph->VC, 4));
	ARRAY_SET(&graph->VC, 0, Vector2, { .x = 0.0, .y = 0.0 });
	ARRAY_SET(&graph->VC, 1, Vector2, { .x = 1.0, .y = 0.0 });
	ARRAY_SET(&graph->VC, 2, Vector2, { .x = 1.0, .y = 1.0 });
	ARRAY_SET(&graph->VC, 3, Vector2, { .x = 0.0, .y = 1.0 });
	TRY(generate_fold_graph_step2(graph, parameters));
	return OK;
}

Error fold_generate_locally_flat_foldable(FoldFile* file,
	const struct LFFParameters* parameters)
{
	TRY(fold_file_recreate(file));
	FoldFrame* key_frame = array_get(&file->frames, 0);
	FoldGraph* graph = &key_frame->graph;
	graph->VC.element_size = sizeof(Vector2);

	struct ProblemParameters p = { .parameters = *parameters };
	p.min_boundary_vertices = CLAMP(p.min_boundary_vertices, 4, USIZE_MAX) - 4;
	p.max_boundary_vertices = CLAMP(p.max_boundary_vertices, 4, USIZE_MAX) - 4;
	p.max_boundary_vertices = MAX(p.min_boundary_vertices, p.max_boundary_vertices);
	p.max_interior_vertices = MAX(p.min_interior_vertices, p.max_interior_vertices);
	p.boundary_offset = CLAMP(p.boundary_offset, 0.0, 0.5);
	p.cell_size = CLAMP(p.cell_size, 1.0 / 1024.0, 0.25);
	p.kawasaki_error = REAL_RADIANS(p.kawasaki_error);
	p.kawasaki_error = CLAMP(p.kawasaki_error, VECTOR2_EPSILON, REAL_MAX);

	p.rng = NULL;
	random_set_seed(p.rng, p.seed);
	p.success = false;

	while NOT(p.success) {
		TRY(generate_fold_graph(graph, &p));
	}

	return OK;
}