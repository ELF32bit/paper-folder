#ifndef FOLD_DESERIALIZATION_JSON_H
#define FOLD_DESERIALIZATION_JSON_H

#include "fold_specification/file.h"

Error fold_file_from_json(FoldFile* file, void* JSON, void* Object);
Error fold_frame_from_json(FoldFrame* frame, void* JSON, void* Object);
Error fold_graph_from_json(FoldGraph* graph, void* JSON, void* Object);

#endif /* FOLD_DESERIALIZATION_JSON_H */