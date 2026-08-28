#ifndef CODECS_SVG_H
#define CODECS_SVG_H

#include "fold/file.h"
#include "fold/frame.h"
#include "fold/graph.h"

Error fold_file_from_svg(FoldFile* file, void* SVG);
Error fold_file_to_svg(FoldFile* file, void* SVG);

Error fold_frame_from_svg(FoldFrame* frame, void* SVG);
Error fold_frame_to_svg(FoldFrame* frame, void* SVG);

#endif /* CODECS_SVG_H */
