#ifndef CODECS_SVG_H
#define CODECS_SVG_H

#include "fold/file.h"
#include "fold/frame.h"
#include "fold/graph.h"

Error fold_file_from_svg(FoldFile* file, void* SVG);
Error fold_file_to_svg(const FoldFile* file, void* SVG);

Error fold_frame_from_svg(FoldFrame* frame, void* SVG);
Error fold_frame_to_svg(const FoldFrame* frame, void* SVG);

Error fold_graph_from_svg(FoldGraph* graph, void* SVG);
Error fold_graph_to_svg(const FoldGraph* graph, void* SVG);

#endif /* CODECS_SVG_H */
