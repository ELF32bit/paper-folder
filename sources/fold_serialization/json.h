#ifndef FOLD_SERIALIZATION_JSON_H
#define FOLD_SERIALIZATION_JSON_H

#include "fold_specification/file.h"

Error fold_file_to_json(const FoldFile* file, void* JSON, void* Object);
Error fold_frame_to_json(const FoldFrame* frame, void* JSON, void* Object);
Error fold_graph_to_json(const FoldGraph* graph, void* JSON, void* Object);

#endif /* FOLD_SERIALIZATION_JSON_H */