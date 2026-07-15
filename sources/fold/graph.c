#include "graph.h"

#include "math/color.h"
#include "math/vector2.h"
#include "math/vector3.h"

static void fold_graph_extensions_create(FoldGraphExtensions* extensions) {
	array_create(&extensions->VCC, sizeof(Color));
	array_create(&extensions->VTC, sizeof(Vector2));
	array_create(&extensions->VNC, sizeof(Vector3));
	array_create(&extensions->FM, SIZEOF(usize, 8));
}

static void fold_graph_extensions_destroy(FoldGraphExtensions* extensions) {
	array_destroy(&extensions->VCC);
	array_destroy(&extensions->VTC);
	array_destroy(&extensions->VNC);
	array_destroy(&extensions->FM);
}

static Error fold_graph_extensions_copy(FoldGraphExtensions* extensions, const FoldGraphExtensions* source_extensions) {
	TRY(array_copy(&extensions->VCC, &source_extensions->VCC));
	TRY(array_copy(&extensions->VTC, &source_extensions->VTC));
	TRY(array_copy(&extensions->VNC, &source_extensions->VNC));
	TRY(array_copy(&extensions->FM, &source_extensions->FM));
	return OK;
}

static void fold_graph_extensions_inherit(FoldGraphExtensions* extensions, const FoldGraphExtensions* source_extensions) {
	if (extensions->VCC.size == 0) array_view(&extensions->VCC, &source_extensions->VCC);
	if (extensions->VTC.size == 0) array_view(&extensions->VTC, &source_extensions->VTC);
	if (extensions->VNC.size == 0) array_view(&extensions->VNC, &source_extensions->VNC);
	if (extensions->FM.size == 0) array_view(&extensions->FM, &source_extensions->FM);
}

static bool fold_graph_extensions_is_inherited(const FoldGraphExtensions* extensions) {
	if (extensions->VCC.is_view) return true;
	if (extensions->VTC.is_view) return true;
	if (extensions->VNC.is_view) return true;
	if (extensions->FM.is_view) return true;
	return false;
}

void fold_graph_create(FoldGraph* graph) {
	array_create(&graph->VC, sizeof(Vector3));
	array2_create(&graph->VV, sizeof(usize));
	array2_create(&graph->VE, sizeof(usize));
	array2_create(&graph->VF, sizeof(usize));
	array_create(&graph->EV, sizeof(FoldGraphEdge));
	array2_create(&graph->EF, sizeof(usize));
	array_create(&graph->EA, sizeof(FoldGraphEdgeAssignment));
	array_create(&graph->EFA, sizeof(real));
	array_create(&graph->EL, sizeof(real));
	array_create(&graph->EO, sizeof(FoldGraphEdgeOrder));
	array2_create(&graph->FV, sizeof(usize));
	array2_create(&graph->FE, sizeof(usize));
	array2_create(&graph->FF, sizeof(usize));
	array_create(&graph->FO, sizeof(FoldGraphFaceOrder));
	fold_graph_extensions_create(&graph->extensions);
}

void fold_graph_destroy(FoldGraph* graph) {
	array_destroy(&graph->VC);
	array2_destroy(&graph->VV);
	array2_destroy(&graph->VE);
	array2_destroy(&graph->VF);
	array_destroy(&graph->EV);
	array2_destroy(&graph->EF);
	array_destroy(&graph->EA);
	array_destroy(&graph->EFA);
	array_destroy(&graph->EL);
	array_destroy(&graph->EO);
	array2_destroy(&graph->FV);
	array2_destroy(&graph->FE);
	array2_destroy(&graph->FF);
	array_destroy(&graph->FO);
	fold_graph_extensions_destroy(&graph->extensions);
}

void fold_graph_recreate(FoldGraph* graph) {
	fold_graph_destroy(graph);
	fold_graph_create(graph);
}

Error fold_graph_copy(FoldGraph* graph, const FoldGraph* source_graph) {
	TRY(array_copy(&graph->VC, &source_graph->VC));
	TRY(array2_copy(&graph->VV, &source_graph->VV));
	TRY(array2_copy(&graph->VE, &source_graph->VE));
	TRY(array2_copy(&graph->VF, &source_graph->VF));
	TRY(array_copy(&graph->EV, &source_graph->EV));
	TRY(array2_copy(&graph->EF, &source_graph->EF));
	TRY(array_copy(&graph->EA, &source_graph->EA));
	TRY(array_copy(&graph->EFA, &source_graph->EFA));
	TRY(array_copy(&graph->EL, &source_graph->EL));
	TRY(array_copy(&graph->EO, &source_graph->EO));
	TRY(array2_copy(&graph->FV, &source_graph->FV));
	TRY(array2_copy(&graph->FE, &source_graph->FE));
	TRY(array2_copy(&graph->FF, &source_graph->FF));
	TRY(array_copy(&graph->FO, &source_graph->FO));
	TRY(fold_graph_extensions_copy(&graph->extensions, &source_graph->extensions));
	return OK;
}

void fold_graph_inherit(FoldGraph* graph, const FoldGraph* source_graph) {
	if (graph->VC.size == 0) array_view(&graph->VC, &source_graph->VC);
	if (graph->VV.size == 0) array2_view(&graph->VV, &source_graph->VV);
	if (graph->VE.size == 0) array2_view(&graph->VE, &source_graph->VE);
	if (graph->VF.size == 0) array2_view(&graph->VF, &source_graph->VF);
	if (graph->EV.size == 0) array_view(&graph->EV, &source_graph->EV);
	if (graph->EF.size == 0) array2_view(&graph->EF, &source_graph->EF);
	if (graph->EA.size == 0) array_view(&graph->EA, &source_graph->EA);
	if (graph->EFA.size == 0) array_view(&graph->EFA, &source_graph->EFA);
	if (graph->EL.size == 0) array_view(&graph->EL, &source_graph->EL);
	if (graph->EO.size == 0) array_view(&graph->EO, &source_graph->EO);
	if (graph->FV.size == 0) array2_view(&graph->FV, &source_graph->FV);
	if (graph->FE.size == 0) array2_view(&graph->FE, &source_graph->FE);
	if (graph->FF.size == 0) array2_view(&graph->FF, &source_graph->FF);
	if (graph->FO.size == 0) array_view(&graph->FO, &source_graph->FO);
	fold_graph_extensions_inherit(&graph->extensions, &source_graph->extensions);
}

bool fold_graph_is_inherited(const FoldGraph* graph) {
	if (graph->VC.is_view) return true;
	if (graph->VV.is_view) return true;
	if (graph->VE.is_view) return true;
	if (graph->VF.is_view) return true;
	if (graph->EV.is_view) return true;
	if (graph->EF.is_view) return true;
	if (graph->EA.is_view) return true;
	if (graph->EFA.is_view) return true;
	if (graph->EL.is_view) return true;
	if (graph->EO.is_view) return true;
	if (graph->FV.is_view) return true;
	if (graph->FE.is_view) return true;
	if (graph->FF.is_view) return true;
	if (graph->FO.is_view) return true;
	if (fold_graph_extensions_is_inherited(&graph->extensions)) return true;
	return false;
}

bool fold_graph_is_abstract(const FoldGraph* graph) {
	return (graph->VC.size == 0);
}

bool fold_graph_is_2D(const FoldGraph* graph) {
	return graph->VC.element_size == sizeof(Vector2);
}

bool fold_graph_is_3D(const FoldGraph* graph) {
	return graph->VC.element_size == sizeof(Vector3);
}

bool fold_graph_is_manifold(const FoldGraph* graph) {
	ARRAY2_ITERATE(&graph->EF, array) {
		if (array.size > 2) return false;
	}
	return true;
}

bool fold_graph_is_orientable(const FoldGraph* graph) {
	(void)graph; // TODO
	return false;
}

bool fold_graph_has_cuts(const FoldGraph* graph) {
	ARRAY_FOR_EACH(&graph->EA, ei, FoldGraphEdgeAssignment*, ea) {
		if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_CUT) return true;
	}
	return false;
}

bool fold_graph_has_joins(const FoldGraph* graph) {
	ARRAY_FOR_EACH(&graph->EA, ei, FoldGraphEdgeAssignment*, ea) {
		if (*ea == FOLD_GRAPH_EDGE_ASSIGNMENT_JOIN) return true;
	}
	return false;
}

bool fold_graph_is_self_touching(const FoldGraph* graph) {
	(void)graph; // TODO
	return false;
}

bool fold_graph_is_self_intersecting(const FoldGraph* graph) {
	(void)graph; // TODO
	return false;
}

bool fold_graph_has_non_convex_faces(const FoldGraph* graph) {
	(void)graph; // TODO
	return false;
}