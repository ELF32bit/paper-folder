#ifndef FOLD_GRAPH_VALIDATION_VALUES_H
#define FOLD_GRAPH_VALIDATION_VALUES_H

#include "fold_specification/graph.h"

void fold_graph_validate_EA_values(const FoldGraph* graph, bool* is);
void fold_graph_validate_EFA_values(const FoldGraph* graph, bool* is);
void fold_graph_validate_EA_EFA_values(const FoldGraph* graph, bool* is);
void fold_graph_validate_EL_values(const FoldGraph* graph, bool* is);
Error fold_graph_validate_EO_pairs(const FoldGraph* graph, bool* is);
Error fold_graph_validate_FO_pairs(const FoldGraph* graph, bool* is);

#endif /* FOLD_GRAPH_VALIDATION_VALUES_H */