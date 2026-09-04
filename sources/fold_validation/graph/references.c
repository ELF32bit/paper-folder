#include "references.h"

#define GENERATE_VALIDATE_REFERENCES(A, AA, B) \
void fold_graph_validate_##A##_##B##_references( \
	const FoldGraph* graph, bool* is) \
{ \
	*is = true; \
	if ((graph->A.is_view && graph->B.is_view) || \
		(graph->A.size == 0 && graph->B.size == 0)) return; \
	usize max_index = graph->B.size; \
	for (usize i = 0; i < graph->A AA.size; i++) { \
		usize* index = array_get(&(graph->A AA), i); \
		if (*index >= max_index) { \
			*is = false; \
			return; \
		} \
	} \
}

#define GENERATE_VALIDATE_REFERENCES_WITH_NULL(A, AA, B) \
void fold_graph_validate_##A##_##B##_references( \
	const FoldGraph* graph, bool* is) \
{ \
	*is = true; \
	if ((graph->A.is_view && graph->B.is_view) || \
		(graph->A.size == 0 && graph->B.size == 0)) return; \
	usize max_index = graph->B.size; \
	for (usize i = 0; i < graph->A AA.size; i++) { \
		usize* index = array_get(&(graph->A AA), i); \
		if (*index == FOLD_GRAPH_NULL) continue; \
		if (*index >= max_index) { \
			*is = false; \
			return; \
		} \
	} \
}

#define GENERATE_VALIDATE_REFERENCES_PAIRS(A, B) \
void fold_graph_validate_##A##_##B##_references( \
	const FoldGraph* graph, bool* is) \
{ \
	*is = true; \
	if ((graph->A.is_view && graph->B.is_view) || \
		(graph->A.size == 0 && graph->B.size == 0)) return; \
	usize max_index = graph->B.size; \
	for (usize i = 0; i < graph->A.size; i++) { \
		usize* pair = array_get(&graph->A, i); \
		if (pair[0] >= max_index || \
			pair[1] >= max_index) { \
			*is = false; \
			return; \
		} \
	} \
}

GENERATE_VALIDATE_REFERENCES(VV,.data, VC)
GENERATE_VALIDATE_REFERENCES(VE,.data, EV)
GENERATE_VALIDATE_REFERENCES_WITH_NULL(VF,.data, FV)
GENERATE_VALIDATE_REFERENCES_PAIRS(EV, VC)
GENERATE_VALIDATE_REFERENCES_WITH_NULL(EF,.data, FV)
GENERATE_VALIDATE_REFERENCES_PAIRS(EO, EV)
GENERATE_VALIDATE_REFERENCES(FV,.data, VC)
GENERATE_VALIDATE_REFERENCES(FE,.data, EV)
GENERATE_VALIDATE_REFERENCES_WITH_NULL(FF,.data, FV)
GENERATE_VALIDATE_REFERENCES_PAIRS(FO, FV)

GENERATE_VALIDATE_REFERENCES(VP,, PC)
GENERATE_VALIDATE_REFERENCES_WITH_NULL(FT,.data, TC)
GENERATE_VALIDATE_REFERENCES_WITH_NULL(FT2,.data, T2C)
GENERATE_VALIDATE_REFERENCES_WITH_NULL(FN,.data, NC)
GENERATE_VALIDATE_REFERENCES_WITH_NULL(FM,, MN)