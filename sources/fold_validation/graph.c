#include "graph/sizes.h"
#include "graph/references.h"
#include "graph/reflexive.h"
#include "graph/values.h"
#include "graph/winding.h"

#define VALIDATE(method, error) do { \
	bool _is = true; \
	fold_graph_validate_##method(graph, &_is); \
	if NOT(_is) { \
		is_still = false; \
		TRY(string_append_raw(errors, error)); \
	} \
} while(0)

static
Error fold_graph_extensions_validate(const FoldGraph* graph, bool* is, String* errors) {
	*is = false;
	bool is_still = true;

	VALIDATE(VP_VC_sizes, "ERROR: vertices_paint and vertices_coords sizes are different\n");
	VALIDATE(FT_FV_sizes, "ERROR: faces_uvs and faces_vertices sizes are different\n");
	VALIDATE(FT2_FV_sizes, "ERROR: faces_uv2s and faces_vertices sizes are different\n");
	VALIDATE(FN_FV_sizes, "ERROR: faces_normals and faces_vertices sizes are different\n");
	VALIDATE(FM_FV_sizes, "ERROR: faces_material and faces_vertices sizes are different\n");
	if NOT(is_still) return OK;

	VALIDATE(VP_PC_references, "ERROR: vertices_paint references missing in paints_color\n");
	VALIDATE(FT_TC_references, "ERROR: faces_uvs references missing in uvs_coords\n");
	VALIDATE(FT2_T2C_references, "ERROR: faces_uv2s references missing in uv2s_coords\n");
	VALIDATE(FN_NC_references, "ERROR: faces_normals references missing in normals_coords\n");
	VALIDATE(FM_MN_references, "ERROR: faces_material references missing in materials_name\n");
	if NOT(is_still) return OK;

	*is = is_still;
	return OK;
}

Error fold_graph_validate(const FoldGraph* graph, bool* is, String* errors) {
	*is = false;
	bool is_still = true;
	bool are_extensions = true;

	VALIDATE(VV_VC_sizes, "ERROR: vertices_vertices and vertices_coords sizes are different\n");
	VALIDATE(VE_VC_sizes, "ERROR: vertices_edges and vertices_coords sizes are different\n");
	VALIDATE(VF_VC_sizes, "ERROR: vertices_faces and vertices_coords sizes are different\n");
	VALIDATE(EF_EV_sizes, "ERROR: edges_faces and edges_vertices sizes are different\n");
	VALIDATE(EA_EV_sizes, "ERROR: edges_assignment and edges_vertices sizes are different\n");
	VALIDATE(EFA_EV_sizes, "ERROR: edges_foldAngle and edges_vertices sizes are different\n");
	VALIDATE(EL_EV_sizes, "ERROR: edges_length and edges_vertices sizes are different\n");
	VALIDATE(FE_FV_sizes, "ERROR: faces_edges and faces_vertices sizes are different\n");
	VALIDATE(FF_FV_sizes, "ERROR: faces_faces and faces_vertices sizes are different\n");
	VALIDATE(FV_sizes, "ERROR: unexpected sizes found in faces_vertices\n");
	if NOT(is_still) return OK;

	if (graph->VC.size > 0) {
	VALIDATE(VV_VC_references, "ERROR: vertices_vertices references missing in vertices_coords\n"); }
	VALIDATE(VE_EV_references, "ERROR: vertices_edges references missing in edges_vertices\n");
	VALIDATE(VF_FV_references, "ERROR: vertices_faces references missing in faces_vertices\n");
	if (graph->VC.size > 0) {
	VALIDATE(EV_VC_references, "ERROR: edges_vertices references missing in vertices_coords\n"); }
	VALIDATE(EF_FV_references, "ERROR: edges_faces references missing in faces_vertices\n");
	VALIDATE(EO_EV_references, "ERROR: edgeOrders references missing in edges_vertices\n");
	if (graph->VC.size > 0) {
	VALIDATE(FV_VC_references, "ERROR: faces_vertices references missing in vertices_coords\n"); }
	VALIDATE(FE_EV_references, "ERROR: faces_edges references missing in edges_vertices\n");
	VALIDATE(FF_FV_references, "ERROR: faces_faces references missing in faces_vertices\n");
	VALIDATE(FO_FV_references, "ERROR: faceOrders references missing in faces_vertices\n");
	if NOT(is_still) return OK;

	VALIDATE(VV_VV_reflexive, "ERROR: vertices_vertices and vertices_vertices mismatching\n");
	VALIDATE(VE_EV_reflexive, "ERROR: vertices_edges and edges_vertices mismatching\n");
	VALIDATE(VF_FV_reflexive, "ERROR: vertices_faces and faces_vertices mismatching\n");
	VALIDATE(EF_FE_reflexive, "ERROR: edges_faces and faces_edges mismatching\n");
	VALIDATE(FF_FF_reflexive, "ERROR: faces_faces and faces_faces mismatching\n");
	if NOT(is_still) return OK;

	VALIDATE(EA_values, "WARNING: unexpected values found in edges_assignment\n");
	VALIDATE(EFA_values, "WARNING: unexpected values found in edges_foldAngle\n");
	VALIDATE(EA_EFA_values, "WARNING: edges_assignment and edges_foldAngle mismatching\n");
	VALIDATE(EL_values, "WARNING: unexpected values found in edges_length\n");

	VALIDATE(EO_pairs, "WARNING: edgeOrders pairs mismatching\n");
	VALIDATE(FO_pairs, "WARNING: faceOrders pairs mismatching\n");

	VALIDATE(VV_VE_winding, "WARNING: vertices_vertices and vertices_edges mismatching winding\n");
	VALIDATE(VV_VF_winding, "WARNING: vertices_vertices and vertices_faces mismatching winding\n");
	VALIDATE(VE_VF_winding, "WARNING: vertices_edges and vertices_faces mismatching winding\n");
	VALIDATE(FV_FE_winding, "WARNING: faces_vertices and faces_edges mismatching winding\n");
	VALIDATE(FV_FF_winding, "WARNING: faces_vertices and faces_faces mismatching winding\n");
	VALIDATE(FE_FF_winding, "WARNING: faces_edges and faces_faces mismatching winding\n");

	TRY(fold_graph_extensions_validate(graph, &are_extensions, errors));
	*is = (is_still && are_extensions);
	return OK;
}

Error fold_graph_validate_inherited(FoldGraph* graph, bool* is, String* errors) {
	bool is_VC_view = graph->VC.is_view;
	bool is_VV_view = graph->VV.is_view;
	bool is_VE_view = graph->VE.is_view;
	bool is_VF_view = graph->VF.is_view;
	bool is_EV_view = graph->EV.is_view;
	bool is_EF_view = graph->EF.is_view;
	bool is_EA_view = graph->EA.is_view;
	bool is_EFA_view = graph->EFA.is_view;
	bool is_EL_view = graph->EL.is_view;
	bool is_EO_view = graph->EO.is_view;
	bool is_FV_view = graph->FV.is_view;
	bool is_FE_view = graph->FE.is_view;
	bool is_FF_view = graph->FF.is_view;
	bool is_FO_view = graph->FO.is_view;
	bool is_PC_view = graph->extensions.PC.is_view;
	bool is_TC_view = graph->extensions.TC.is_view;
	bool is_T2C_view = graph->extensions.T2C.is_view;
	bool is_NC_view = graph->extensions.NC.is_view;
	bool is_MN_view = graph->extensions.MN.is_view;
	bool is_VP_view = graph->extensions.VP.is_view;
	bool is_FT_view = graph->extensions.FT.is_view;
	bool is_FT2_view = graph->extensions.FT2.is_view;
	bool is_FN_view = graph->extensions.FN.is_view;
	bool is_FM_view = graph->extensions.FM.is_view;

	graph->VC.is_view = false;
	graph->VV.is_view = false;
	graph->VE.is_view = false;
	graph->VF.is_view = false;
	graph->EV.is_view = false;
	graph->EF.is_view = false;
	graph->EA.is_view = false;
	graph->EFA.is_view = false;
	graph->EL.is_view = false;
	graph->EO.is_view = false;
	graph->FV.is_view = false;
	graph->FE.is_view = false;
	graph->FF.is_view = false;
	graph->FO.is_view = false;
	graph->extensions.PC.is_view = false;
	graph->extensions.TC.is_view = false;
	graph->extensions.T2C.is_view = false;
	graph->extensions.NC.is_view = false;
	graph->extensions.MN.is_view = false;
	graph->extensions.VP.is_view = false;
	graph->extensions.FT.is_view = false;
	graph->extensions.FT2.is_view = false;
	graph->extensions.FN.is_view = false;
	graph->extensions.FM.is_view = false;

	Error result = fold_graph_validate(graph, is, errors);

	graph->VC.is_view = is_VC_view;
	graph->VV.is_view = is_VV_view;
	graph->VE.is_view = is_VE_view;
	graph->VF.is_view = is_VF_view;
	graph->EV.is_view = is_EV_view;
	graph->EF.is_view = is_EF_view;
	graph->EA.is_view = is_EA_view;
	graph->EFA.is_view = is_EFA_view;
	graph->EL.is_view = is_EL_view;
	graph->EO.is_view = is_EO_view;
	graph->FV.is_view = is_FV_view;
	graph->FE.is_view = is_FE_view;
	graph->FF.is_view = is_FF_view;
	graph->FO.is_view = is_FO_view;
	graph->extensions.PC.is_view = is_PC_view;
	graph->extensions.TC.is_view = is_TC_view;
	graph->extensions.T2C.is_view = is_T2C_view;
	graph->extensions.NC.is_view = is_NC_view;
	graph->extensions.MN.is_view = is_MN_view;
	graph->extensions.VP.is_view = is_VP_view;
	graph->extensions.FT.is_view = is_FT_view;
	graph->extensions.FT2.is_view = is_FT2_view;
	graph->extensions.FN.is_view = is_FN_view;
	graph->extensions.FM.is_view = is_FM_view;

	return result;
}