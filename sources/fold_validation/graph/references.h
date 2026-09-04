#ifndef FOLD_GRAPH_VALIDATION_REFERENCES_H
#define FOLD_GRAPH_VALIDATION_REFERENCES_H

#include "fold_specification/graph.h"

void fold_graph_validate_VV_VC_references(const FoldGraph* graph, bool* is);
void fold_graph_validate_VE_EV_references(const FoldGraph* graph, bool* is);
void fold_graph_validate_VF_FV_references(const FoldGraph* graph, bool* is);
void fold_graph_validate_EV_VC_references(const FoldGraph* graph, bool* is);
void fold_graph_validate_EF_FV_references(const FoldGraph* graph, bool* is);
void fold_graph_validate_EO_EV_references(const FoldGraph* graph, bool* is);
void fold_graph_validate_FV_VC_references(const FoldGraph* graph, bool* is);
void fold_graph_validate_FE_EV_references(const FoldGraph* graph, bool* is);
void fold_graph_validate_FF_FV_references(const FoldGraph* graph, bool* is);
void fold_graph_validate_FO_FV_references(const FoldGraph* graph, bool* is);

void fold_graph_validate_VP_PC_references(const FoldGraph* graph, bool* is);
void fold_graph_validate_FT_TC_references(const FoldGraph* graph, bool* is);
void fold_graph_validate_FT2_T2C_references(const FoldGraph* graph, bool* is);
void fold_graph_validate_FN_NC_references(const FoldGraph* graph, bool* is);
void fold_graph_validate_FM_MN_references(const FoldGraph* graph, bool* is);

#endif /* FOLD_GRAPH_VALIDATION_REFERENCES_H */