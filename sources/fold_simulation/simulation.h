#ifndef FOLD_SIMULATION_H
#define FOLD_SIMULATION_H

#include "fold_specification/file.h"

struct FoldSimulationParameters {
	f32 fold_percent;
	f32 axial_stiffness;
	f32 fold_stiffness;
	f32 join_stiffness;
	f32 face_stiffness;
	f32 damping_ratio;
	f32 delta_ratio;
};

/* TODO: My initial attempt can be viable, given a limited scope */
/* I also could not get it working with the original stability */
/* Perhaphs another paper might be better for this tool */

#endif /* FOLD_SIMULATION_H */