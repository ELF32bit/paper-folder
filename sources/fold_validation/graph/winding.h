#ifndef FOLD_GRAPH_VALIDATION_WINDING_H
#define FOLD_GRAPH_VALIDATION_WINDING_H

#include "fold_specification/graph.h"

void fold_graph_validate_VV_VE_winding(const FoldGraph* graph, bool* is);
Error fold_graph_validate_VV_VF_winding(const FoldGraph* graph, bool* is);
Error fold_graph_validate_VE_VF_winding(const FoldGraph* graph, bool* is);
void fold_graph_validate_FV_FE_winding(const FoldGraph* graph, bool* is);
Error fold_graph_validate_FV_FF_winding(const FoldGraph* graph, bool* is);
void fold_graph_validate_FE_FF_winding(const FoldGraph* graph, bool* is);

#endif /* FOLD_GRAPH_VALIDATION_WINDING_H */