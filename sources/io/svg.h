#ifndef IO_SVG_H
#define IO_SVG_H

#include "fold/file.h"

Error fold_file_frame_from_svg(FoldFile* file, usize index, void* SVG);
Error fold_file_frame_to_svg(FoldFile* file, usize index, void* SVG);

#endif /* IO_SVG_H */
