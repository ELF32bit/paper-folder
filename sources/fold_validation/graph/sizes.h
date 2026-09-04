#ifndef FOLD_GRAPH_VALIDATION_SIZES_H
#define FOLD_GRAPH_VALIDATION_SIZES_H

#include "fold_specification/graph.h"

void fold_graph_validate_VV_VC_sizes(const FoldGraph* graph, bool* is);
void fold_graph_validate_VE_VC_sizes(const FoldGraph* graph, bool* is);
void fold_graph_validate_VF_VC_sizes(const FoldGraph* graph, bool* is);
void fold_graph_validate_EF_EV_sizes(const FoldGraph* graph, bool* is);
void fold_graph_validate_EA_EV_sizes(const FoldGraph* graph, bool* is);
void fold_graph_validate_EFA_EV_sizes(const FoldGraph* graph, bool* is);
void fold_graph_validate_EL_EV_sizes(const FoldGraph* graph, bool* is);
void fold_graph_validate_FE_FV_sizes(const FoldGraph* graph, bool* is);
void fold_graph_validate_FF_FV_sizes(const FoldGraph* graph, bool* is);
void fold_graph_validate_FV_sizes(const FoldGraph* graph, bool* is);

void fold_graph_validate_VP_VC_sizes(const FoldGraph* graph, bool* is);
void fold_graph_validate_FT_FV_sizes(const FoldGraph* graph, bool* is);
void fold_graph_validate_FT2_FV_sizes(const FoldGraph* graph, bool* is);
void fold_graph_validate_FN_FV_sizes(const FoldGraph* graph, bool* is);
void fold_graph_validate_FM_FV_sizes(const FoldGraph* graph, bool* is);

#endif /* FOLD_GRAPH_VALIDATION_SIZES_H */