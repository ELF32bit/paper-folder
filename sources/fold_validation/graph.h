#ifndef FOLD_GRAPH_VALIDATION
#define FOLD_GRAPH_VALIDATION

#include "fold_specification/graph.h"

Error fold_graph_validate(const FoldGraph* graph, bool* is, String* errors);
Error fold_graph_validate_inherited(FoldGraph* graph, bool* is, String* errors);

#endif /* FOLD_GRAPH_VALIDATION */