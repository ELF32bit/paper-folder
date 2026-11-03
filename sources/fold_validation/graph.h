#ifndef FOLD_VALIDATION_GRAPH_H
#define FOLD_VALIDATION_GRAPH_H

#include "fold_specification/graph.h"

Error fold_graph_validate(const FoldGraph* graph, bool* is,
	String* NULLABLE(errors));

Error fold_graph_validate_inherited(FoldGraph* graph, bool* is,
	String* NULLABLE(errors));

#endif /* FOLD_VALIDATION_GRAPH_H */