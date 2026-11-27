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

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

static
Error fold_graph_extensions_copy(FoldGraphExtensions* extensions,
	const FoldGraphExtensions* source)
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

	TRY(fold_graph_extensions_copy(
		&graph->extensions, &source->extensions));

	return OK;
}

static
void fold_graph_extensions_inherit(FoldGraphExtensions* extensions,
	const FoldGraphExtensions* source)
{
#define _GENERATE(A, a) \
	if (extensions->A.is_view || extensions->A.size == 0) { \
		array##a##_view(&extensions->A, &source->A); \
	}

	_GENERATE(PC,)
	_GENERATE(TC,)
	_GENERATE(T2C,)
	_GENERATE(NC,)
	_GENERATE(MN,)
	_GENERATE(VP,)
	_GENERATE(FT,2)
	_GENERATE(FT2,2)
	_GENERATE(FN,2)
	_GENERATE(FM,)

#undef _GENERATE
}

void fold_graph_inherit(FoldGraph* graph, const FoldGraph* source) {
#define _GENERATE(A, a) \
	if (graph->A.is_view || graph->A.size == 0) { \
		array##a##_view(&graph->A, &source->A); \
	}

	_GENERATE(VC,)
	_GENERATE(VV,2)
	_GENERATE(VE,2)
	_GENERATE(VF,2)
	_GENERATE(EV,)
	_GENERATE(EF,2)
	_GENERATE(EA,)
	_GENERATE(EFA,)
	_GENERATE(EL,)
	_GENERATE(EO,)
	_GENERATE(FV,2)
	_GENERATE(FE,2)
	_GENERATE(FF,2)
	_GENERATE(FO,)

#undef _GENERATE
	fold_graph_extensions_inherit(
		&graph->extensions, &source->extensions);
}

static
bool fold_graph_extensions_is_inherited(const FoldGraphExtensions* extensions)
{
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

	if (fold_graph_extensions_is_inherited(
		&graph->extensions)) return true;

	return false;
}