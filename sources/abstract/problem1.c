#include "problems.h"

#include "fold/file.h"
#include "definitions/random.h"
#include "io/svg.h"

#include "triangle.h"

struct ProblemParameters {
	RNG rng;
	usize max_vertices;
	real boundary_offset;
	real cell_size;
	bool success;
};

static inline
Error generate_fold_graph(FoldGraph* graph,
	struct ProblemParameters* parameters);

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
	trifree(io->pointmarkerlist);
	trifree(io->edgelist);
	trifree(io->edgemarkerlist);
	trifree(io->trianglelist);
}

static inline
Error generate_fold_graph_step5(FoldGraph* graph,
	struct ProblemParameters* parameters)
{
	TRY(fold_graph_VV_from_EV(graph));
	TRY(fold_graph_VE_from_VV(graph, NULL));
	ARRAY2_ITERATE(&graph->VE, ve) {
		bool is_boundary = false;
		ARRAY_FOR_EACH_IN_RANGE(&graph->VE.data,
			i, usize*, vei, ve.start, ve.end)
		{
			FoldGraphEdgeAssignment* ea = array_get(&graph->EA, *vei);
			if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY) {
				is_boundary = true;
				break;
			}
		}
		if (is_boundary) continue;
		if (ve.size % 2 == 1) {
			return OK;
		}
	}
	parameters->success = true;
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
		isize a = out.edgelist[i * 2 + 0];
		isize b = out.edgelist[i * 2 + 1];
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
		isize a = out.trianglelist[i * 3 + 0];
		isize b = out.trianglelist[i * 3 + 1];
		isize c = out.trianglelist[i * 3 + 2];
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

	ARRAY_CREATE(unique_points, Vector2);
	TRY_OR_ELSE(array_resize(&unique_points, graph->VC.size),
		array_destroy(&unique_points);
		set_destroy(&set));

	unique_points.size = 0;
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
			array_destroy(&unique_points);
			set_destroy(&set));

		unique_points.size++;
		array_set(&unique_points,
			unique_points.size - 1, vc);
	}

	set_destroy(&set);
	TRY_OR_ELSE(array_resize(&unique_points, unique_points.size),
		array_destroy(&unique_points));

	free(graph->VC.data);
	graph->VC = unique_points;

	TRY(generate_fold_graph_step4(graph, parameters));
	return OK;
}

static inline
Error generate_fold_graph_step2(FoldGraph* graph,
	struct ProblemParameters* parameters)
{
	RNG* rng = &parameters->rng;
	usize n = parameters->max_vertices;
	usize n1 = random_usize_in_range(rng, 0, n);
	usize n2 = random_usize_in_range(rng, 0, n + n1 * n1);
	real offset = parameters->boundary_offset;

	usize old_size = graph->VC.size;
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

Error abstract_problems_solve1(void) {
	FoldFile file;
	TRY(fold_file_create(&file));
	FoldFrame* key_frame = array_get(&file.frames, 0);
	FoldGraph* graph = &key_frame->graph;
	graph->VC.element_size = sizeof(Vector2);

	struct ProblemParameters parameters;
	random_set_seed(&parameters.rng, 777);
	parameters.max_vertices = 100;
	parameters.boundary_offset = 0.04;
	parameters.cell_size = 0.02;
	parameters.success = false;

	while NOT(parameters.success) {
		TRY_OR_ELSE(generate_fold_graph(graph, &parameters),
			fold_file_destroy(&file));
	}

	TRY_OR_ELSE(fold_file_frame_to_svg(&file, 0, "assets/problem1.svg"),
		fold_file_destroy(&file));

	fold_file_destroy(&file);
	return OK;
}