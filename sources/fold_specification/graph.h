#ifndef FOLD_GRAPH_H
#define FOLD_GRAPH_H

#include "collections/array.h"
#include "collections/array2.h"
#include "collections/string.h"

#define FOLD_GRAPH_NULL USIZE_MAX

#define FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY 'B'
#define FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN 'M'
#define FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY 'V'
#define FOLD_GRAPH_EDGE_ASSIGNMENT_FLAT 'F'
#define FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN 'U'
#define FOLD_GRAPH_EDGE_ASSIGNMENT_CUT 'C'
#define FOLD_GRAPH_EDGE_ASSIGNMENT_JOIN 'J'

#define FOLD_GRAPH_EDGE_ASSIGNMENTS_COUNT 7
#define FOLD_GRAPH_EDGE_ASSIGNMENT_ANY(assignment) ( \
	(assignment) == FOLD_GRAPH_EDGE_ASSIGNMENT_BOUNDARY || \
	(assignment) == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN || \
	(assignment) == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY || \
	(assignment) == FOLD_GRAPH_EDGE_ASSIGNMENT_FLAT || \
	(assignment) == FOLD_GRAPH_EDGE_ASSIGNMENT_UNKNOWN || \
	(assignment) == FOLD_GRAPH_EDGE_ASSIGNMENT_CUT || \
	(assignment) == FOLD_GRAPH_EDGE_ASSIGNMENT_JOIN)

#define FOLD_GRAPH_EDGE_ASSIGNMENT_INVERT(assignment) ( \
	((assignment) == FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN) \
		? FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY \
	: ((assignment) == FOLD_GRAPH_EDGE_ASSIGNMENT_VALLEY) \
		? FOLD_GRAPH_EDGE_ASSIGNMENT_MOUNTAIN \
	: (assignment))

#define FOLD_GRAPH_ORDER_POSITIVE 1
#define FOLD_GRAPH_ORDER_NEGATIVE 0
#define FOLD_GRAPH_ORDER_UNKNOWN 2

#define FOLD_GRAPH_ORDER_ANY(order) ( \
	(order) == FOLD_GRAPH_ORDER_POSITIVE || \
	(order) == FOLD_GRAPH_ORDER_NEGATIVE || \
	(order) == FOLD_GRAPH_ORDER_UNKNOWN)

#define FOLD_GRAPH_ORDER_INVERT(order) ( \
	((order) == FOLD_GRAPH_ORDER_POSITIVE) \
		? FOLD_GRAPH_ORDER_NEGATIVE \
	: ((order) == FOLD_GRAPH_ORDER_NEGATIVE) \
		? FOLD_GRAPH_ORDER_POSITIVE \
	: FOLD_GRAPH_ORDER_UNKNOWN)

#define FOLD_GRAPH_EDGE_ORDER_LEFT FOLD_GRAPH_ORDER_POSITIVE
#define FOLD_GRAPH_EDGE_ORDER_RIGHT FOLD_GRAPH_ORDER_NEGATIVE
#define FOLD_GRAPH_EDGE_ORDER_UNKNOWN FOLD_GRAPH_ORDER_UNKNOWN

#define FOLD_GRAPH_FACE_ORDER_ABOVE FOLD_GRAPH_ORDER_POSITIVE
#define FOLD_GRAPH_FACE_ORDER_BELOW FOLD_GRAPH_ORDER_NEGATIVE
#define FOLD_GRAPH_FACE_ORDER_UNKNOWN FOLD_GRAPH_ORDER_UNKNOWN

typedef struct FoldGraphExtensions {
	Array PC; /* paints_color */
	Array TC; /* uvs_coords */
	Array T2C; /* uv2s_coords */
	Array NC; /* normals_coords */
	Array MN; /* materials_name */
	Array VP; /* vertices_paint */
	Array2 FT; /* faces_uvs */
	Array2 FT2; /* faces_uv2s */
	Array2 FN; /* faces_normals */
	Array FM; /* faces_material */
} FoldGraphExtensions;

typedef struct FoldGraph {
	Array VC; /* vertices_coords */
	Array2 VV; /* vertices_vertices */
	Array2 VE; /* vertices_edges */
	Array2 VF; /* vertices_faces */
	Array EV; /* edges_vertices */
	Array2 EF; /* edges_faces */
	Array EA; /* edges_assignment */
	Array EFA; /* edges_foldAngle */
	Array EL; /* edges_length */
	Array EO; /* edgeOrders */
	Array2 FV; /* faces_vertices */
	Array2 FE; /* faces_edges */
	Array2 FF; /* faces_faces */
	Array FO; /* faceOrders */
	union {
		FoldGraphExtensions extensions;
		struct {
			Array PC; /* paints_color */
			Array TC; /* uvs_coords */
			Array T2C; /* uv2s_coords */
			Array NC; /* normals_coords */
			Array MN; /* materials_name */
			Array VP; /* vertices_paint */
			Array2 FT; /* faces_uvs */
			Array2 FT2; /* faces_uv2s */
			Array2 FN; /* faces_normals */
			Array FM; /* faces_material */
		};
	};
} FoldGraph;

typedef struct FoldGraphEdge {
	union {
		struct { usize a, b; };
		usize components[2];
	};
} FoldGraphEdge;

typedef char FoldGraphEdgeAssignment;

typedef struct FoldGraphOrder {
	union {
		struct { usize a, b, o; };
		usize components[3];
	};
} FoldGraphOrder;

typedef struct FoldGraphEdgeOrder {
	union {
		struct { usize edge1, edge2, order; };
		usize components[3];
	};
} FoldGraphEdgeOrder;

typedef struct FoldGraphFaceOrder {
	union {
		struct { usize face1, face2, order; };
		usize components[3];
	};
} FoldGraphFaceOrder;

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

void fold_graph_create(FoldGraph* graph);

#define FOLD_GRAPH_CREATE(graph) \
	FoldGraph graph; \
	fold_graph_create(&graph)

#define FoldGraph_destroy fold_graph_destroy
void fold_graph_destroy(FoldGraph* graph);

void fold_graph_recreate(FoldGraph* graph);

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

#define FoldGraph_copy fold_graph_copy
Error fold_graph_copy(FoldGraph* graph, const FoldGraph* source);

void fold_graph_inherit(FoldGraph* graph, const FoldGraph* source);
bool fold_graph_is_inherited(const FoldGraph* graph);

#endif /* FOLD_GRAPH_H */