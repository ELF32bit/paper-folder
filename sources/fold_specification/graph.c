#include "graph.h"

#include "math/color.h"
#include "math/vector2.h"
#include "math/vector3.h"

/* ========================================================================= */
/* Creation & Destruction                                                    */
/* ========================================================================= */

static
void fold_graph_extensions_create(FoldGraphExtensions* extensions) {
	array_create(&extensions->PC, sizeof(Color));
	array_create(&extensions->TC, sizeof(Vector2));
	array_create(&extensions->T2C, sizeof(Vector2));
	array_create(&extensions->NC, sizeof(Vector3));
	array_create(&extensions->MN, SIZEOF(char, 32));
	array_create(&extensions->VP, sizeof(usize));
	array2_create(&extensions->FT, sizeof(usize));
	array2_create(&extensions->FT2, sizeof(usize));
	array2_create(&extensions->FN, sizeof(usize));
	array_create(&extensions->FM, sizeof(usize));
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

static
void fold_graph_extensions_destroy(FoldGraphExtensions* extensions) {
	array_destroy(&extensions->PC);
	array_destroy(&extensions->TC);
	array_destroy(&extensions->T2C);
	array_destroy(&extensions->NC);
	array_destroy(&extensions->MN);
	array_destroy(&extensions->VP);
	array2_destroy(&extensions->FT);
	array2_destroy(&extensions->FT2);
	array2_destroy(&extensions->FN);
	array_destroy(&extensions->FM);
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

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

static
Error fold_graph_extensions_copy(
	FoldGraphExtensions* extensions, const FoldGraphExtensions* source)
{
	TRY(array_copy(&extensions->PC, &source->PC));
	TRY(array_copy(&extensions->TC, &source->TC));
	TRY(array_copy(&extensions->T2C, &source->T2C));
	TRY(array_copy(&extensions->NC, &source->NC));
	TRY(array_copy(&extensions->MN, &source->MN));
	TRY(array_copy(&extensions->VP, &source->VP));
	TRY(array2_copy(&extensions->FT, &source->FT));
	TRY(array2_copy(&extensions->FT2, &source->FT2));
	TRY(array2_copy(&extensions->FN, &source->FN));
	TRY(array_copy(&extensions->FM, &source->FM));
	return OK;
}

Error fold_graph_copy(FoldGraph* graph, const FoldGraph* source) {
	TRY(array_copy(&graph->VC, &source->VC));
	TRY(array2_copy(&graph->VV, &source->VV));
	TRY(array2_copy(&graph->VE, &source->VE));
	TRY(array2_copy(&graph->VF, &source->VF));
	TRY(array_copy(&graph->EV, &source->EV));
	TRY(array2_copy(&graph->EF, &source->EF));
	TRY(array_copy(&graph->EA, &source->EA));
	TRY(array_copy(&graph->EFA, &source->EFA));
	TRY(array_copy(&graph->EL, &source->EL));
	TRY(array_copy(&graph->EO, &source->EO));
	TRY(array2_copy(&graph->FV, &source->FV));
	TRY(array2_copy(&graph->FE, &source->FE));
	TRY(array2_copy(&graph->FF, &source->FF));
	TRY(array_copy(&graph->FO, &source->FO));
	TRY(fold_graph_extensions_copy(&graph->extensions, &source->extensions));
	return OK;
}

static
void fold_graph_extensions_inherit(
	FoldGraphExtensions* extensions, const FoldGraphExtensions* source)
{
	if (extensions->PC.is_view || extensions->PC.size == 0)
		array_view(&extensions->PC, &source->PC);
	if (extensions->TC.is_view || extensions->TC.size == 0)
		array_view(&extensions->TC, &source->TC);
	if (extensions->T2C.is_view || extensions->T2C.size == 0)
		array_view(&extensions->T2C, &source->T2C);
	if (extensions->NC.is_view || extensions->NC.size == 0)
		array_view(&extensions->NC, &source->NC);
	if (extensions->MN.is_view || extensions->MN.size == 0)
		array_view(&extensions->MN, &source->MN);
	if (extensions->VP.is_view || extensions->VP.size == 0)
		array_view(&extensions->VP, &source->VP);
	if (extensions->FT.is_view || extensions->FT.size == 0)
		array2_view(&extensions->FT, &source->FT);
	if (extensions->FT2.is_view || extensions->FT2.size == 0)
		array2_view(&extensions->FT2, &source->FT2);
	if (extensions->FN.is_view || extensions->FN.size == 0)
		array2_view(&extensions->FN, &source->FN);
	if (extensions->FM.is_view || extensions->FM.size == 0)
		array_view(&extensions->FM, &source->FM);
}

void fold_graph_inherit(FoldGraph* graph, const FoldGraph* source) {
	if (graph->VC.is_view || graph->VC.size == 0)
		array_view(&graph->VC, &source->VC);
	if (graph->VV.is_view || graph->VV.size == 0)
		array2_view(&graph->VV, &source->VV);
	if (graph->VE.is_view || graph->VE.size == 0)
		array2_view(&graph->VE, &source->VE);
	if (graph->VF.is_view || graph->VF.size == 0)
		array2_view(&graph->VF, &source->VF);
	if (graph->EV.is_view || graph->EV.size == 0)
		array_view(&graph->EV, &source->EV);
	if (graph->EF.is_view || graph->EF.size == 0)
		array2_view(&graph->EF, &source->EF);
	if (graph->EA.is_view || graph->EA.size == 0)
		array_view(&graph->EA, &source->EA);
	if (graph->EFA.is_view || graph->EFA.size == 0)
		array_view(&graph->EFA, &source->EFA);
	if (graph->EL.is_view || graph->EL.size == 0)
		array_view(&graph->EL, &source->EL);
	if (graph->EO.is_view || graph->EO.size == 0)
		array_view(&graph->EO, &source->EO);
	if (graph->FV.is_view || graph->FV.size == 0)
		array2_view(&graph->FV, &source->FV);
	if (graph->FE.is_view || graph->FE.size == 0)
		array2_view(&graph->FE, &source->FE);
	if (graph->FF.is_view || graph->FF.size == 0)
		array2_view(&graph->FF, &source->FF);
	if (graph->FO.is_view || graph->FO.size == 0)
		array_view(&graph->FO, &source->FO);
	fold_graph_extensions_inherit(&graph->extensions, &source->extensions);
}

static
bool fold_graph_extensions_is_inherited(const FoldGraphExtensions* extensions) {
	if (extensions->PC.is_view) return true;
	if (extensions->TC.is_view) return true;
	if (extensions->T2C.is_view) return true;
	if (extensions->NC.is_view) return true;
	if (extensions->MN.is_view) return true;
	if (extensions->VP.is_view) return true;
	if (extensions->FT.is_view) return true;
	if (extensions->FT2.is_view) return true;
	if (extensions->FN.is_view) return true;
	if (extensions->FM.is_view) return true;
	return false;
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