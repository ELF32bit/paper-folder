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
#define _GENERATE(A, a) \
	array##a##_destroy(&extensions->A);

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

void fold_graph_destroy(FoldGraph* graph) {
#define _GENERATE(A, a) \
	array##a##_destroy(&graph->A);

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
	fold_graph_extensions_destroy(&graph->extensions);
}

/* ========================================================================= */
/* Methods                                                                   */
/* ========================================================================= */

static
Error fold_graph_extensions_copy(FoldGraphExtensions* extensions,
	const FoldGraphExtensions* source)
{
#define _GENERATE(A, a) \
	TRY(array##a##_copy(&extensions->A, &source->A));

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
	return OK;
}

Error fold_graph_copy(FoldGraph* graph, const FoldGraph* source) {
#define _GENERATE(A, a) \
	TRY(array##a##_copy(&graph->A, &source->A));

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
#define _GENERATE(A) \
	if (extensions->A.is_view) { \
		return true; \
	}

	_GENERATE(PC)
	_GENERATE(TC)
	_GENERATE(T2C)
	_GENERATE(NC)
	_GENERATE(MN)
	_GENERATE(VP)
	_GENERATE(FT)
	_GENERATE(FT2)
	_GENERATE(FN)
	_GENERATE(FM)

#undef _GENERATE
	return false;
}

bool fold_graph_is_inherited(const FoldGraph* graph) {
#define _GENERATE(A) \
	if (graph->A.is_view) { \
		return true; \
	}

	_GENERATE(VC)
	_GENERATE(VV)
	_GENERATE(VE)
	_GENERATE(VF)
	_GENERATE(EV)
	_GENERATE(EF)
	_GENERATE(EA)
	_GENERATE(EFA)
	_GENERATE(EL)
	_GENERATE(EO)
	_GENERATE(FV)
	_GENERATE(FE)
	_GENERATE(FF)
	_GENERATE(FO)

#undef _GENERATE
	if (fold_graph_extensions_is_inherited(
		&graph->extensions)) return true;

	return false;
}