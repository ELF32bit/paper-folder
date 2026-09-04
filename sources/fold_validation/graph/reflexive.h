#ifndef FOLD_GRAPH_VALIDATION_REFLEXIVE_H
#define FOLD_GRAPH_VALIDATION_REFLEXIVE_H

#include "fold_specification/graph.h"

Error fold_graph_validate_VV_VV_reflexive(const FoldGraph* graph, bool* is);
Error fold_graph_validate_VE_EV_reflexive(const FoldGraph* graph, bool* is);
Error fold_graph_validate_VF_FV_reflexive(const FoldGraph* graph, bool* is);
Error fold_graph_validate_EF_FE_reflexive(const FoldGraph* graph, bool* is);
Error fold_graph_validate_FF_FF_reflexive(const FoldGraph* graph, bool* is);

#endif /* FOLD_GRAPH_VALIDATION_REFLEXIVE_H */