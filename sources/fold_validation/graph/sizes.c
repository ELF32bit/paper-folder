#include "sizes.h"

#define GENERATE_VALIDATE_SIZES(A, B) \
void fold_graph_validate_##A##_##B##_sizes(const FoldGraph* graph, bool* is) \
{ \
	*is = (graph->A.size == graph->B.size || graph->A.size == 0); \
}

GENERATE_VALIDATE_SIZES(VV, VC)
GENERATE_VALIDATE_SIZES(VE, VC)
GENERATE_VALIDATE_SIZES(VF, VC)
GENERATE_VALIDATE_SIZES(EF, EV)
GENERATE_VALIDATE_SIZES(EA, EV)
GENERATE_VALIDATE_SIZES(EFA, EV)
GENERATE_VALIDATE_SIZES(EL, EV)
GENERATE_VALIDATE_SIZES(FE, FV)
GENERATE_VALIDATE_SIZES(FF, FV)

void fold_graph_validate_FV_sizes(const FoldGraph* graph, bool* is) {
	*is = true;
	if (graph->FV.is_view) return;
	ARRAY2_ITERATE(&graph->FV, fv) {
		if (fv.size < 3) {
			*is = false;
			return;
		}
	}
}

GENERATE_VALIDATE_SIZES(VP, VC)
GENERATE_VALIDATE_SIZES(FT, FV)
GENERATE_VALIDATE_SIZES(FT2, FV)
GENERATE_VALIDATE_SIZES(FN, FV)
GENERATE_VALIDATE_SIZES(FM, FV)