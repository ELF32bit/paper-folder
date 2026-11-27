local command = { name = "clear", group = "Generation Commands" }
local from_fold = require("commands.from.fold")
local annotate = require("commands.annotate")
local to_fold = require("commands.to.fold")
local _FoldFrame_ = require("fold.frame_C")
local _FoldGraph_ = require("fold.graph_C")

function command.register(parser)
	local clear = parser:command(command.name)
		:description("clear graph arrays")
		:help_description_margin(40)

	clear:argument("input"):args("?")
		:description("path to the FOLD source")

	clear:group("Options",
		clear:option("-o --output"):argname("<file>")
		:description("path to the FOLD destination"),

		clear:option("-f --frame"):argname("<int>")
		:description("FOLD frames (multiple)")
		:convert(tonumber)
		:count("*")
		:target("frames")
	)

	clear:group("Vertices Arrays",
		clear:flag("--VC --vertices_coords")
		:description("clear vertices coordinates")
		:target("VC"),

		clear:flag("--VE --vertices_edges")
		:description("clear vertices edges")
		:target("VE"),

		clear:flag("--VF --vertices_faces")
		:description("clear vertices faces")
		:target("VF")
	)

	clear:group("Vertices Extensions",
		clear:flag("--VP --vertices_paint")
		:description("clear vertices paints")
		:target("VP")
	)

	clear:group("Edges Arrays",
		clear:flag("--EV --edges_vertices")
		:description("clear edges vertices")
		:target("EV"),

		clear:flag("--EF --edges_faces")
		:description("clear edges faces")
		:target("EF"),

		clear:flag("--EA --edges_assignment")
		:description("clear edges assignments")
		:target("EA"),

		clear:flag("--EFA --edges_foldAngle")
		:description("clear edges fold angles")
		:target("EFA"),

		clear:flag("--EL --edges_length")
		:description("clear edges lengths")
		:target("EL"),

		clear:flag("--EO --edgeOrders")
		:description("clear edges orders")
		:target("EO")
	)

	clear:group("Faces Arrays",
		clear:flag("--FV --faces_vertices")
		:description("clear faces vertices")
		:target("FV"),

		clear:flag("--FE --faces_edges")
		:description("clear faces edges")
		:target("FE"),

		clear:flag("--FF --faces_faces")
		:description("clear faces faces")
		:target("FF"),

		clear:flag("--FO --faceOrders")
		:description("clear faces orders")
		:target("FO")
	)

	clear:group("Faces Extensions",
		clear:flag("--FT --faces_uvs")
		:description("clear faces texture")
		:target("FT"),

		clear:flag("--FT2 --faces_uv2s")
		:description("clear faces second texture")
		:target("FT2"),

		clear:flag("--FN --faces_normals")
		:description("clear faces normals")
		:target("FN"),

		clear:flag("--FM --faces_materials")
		:description("clear faces materials")
		:target("FM")
	)

	clear:group("Extensions Arrays",
		clear:flag("--PC --paints_color")
		:description("clear paints colors")
		:target("PC"),

		clear:flag("--TC --uvs_coords")
		:description("clear texture coordinates")
		:target("TC"),

		clear:flag("--T2C --uv2s_coords")
		:description("clear second texture coordinates")
		:target("T2C"),

		clear:flag("--NC --normals_coords")
		:description("clear normals coordinates")
		:target("NC"),

		clear:flag("--MN --materials_name")
		:description("clear materials names")
		:target("MN")
	)

	return clear
end

function command.print_fold_frame_range_error(index, size)
	return annotate.print_fold_frame_range_error(index, size)
end

function command.fold_graph_clear(graph, args)
	if args.VC then graph:VC_recreate() end
	if args.VE then graph:VE_recreate() end
	if args.VF then graph:VF_recreate() end

	if args.EV then graph:EV_recreate() end
	if args.EF then graph:EF_recreate() end
	if args.EA then graph:EA_recreate() end
	if args.EFA then graph:EFA_recreate() end
	if args.EL then graph:EL_recreate() end
	if args.EO then graph:EO_recreate() end

	if args.FV then graph:FV_recreate() end
	if args.FE then graph:FE_recreate() end
	if args.FF then graph:FF_recreate() end
	if args.FO then graph:FO_recreate() end

	if args.PC then graph:PC_recreate() end
	if args.TC then graph:TC_recreate() end
	if args.T2C then graph:T2C_recreate() end
	if args.NC then graph:NC_recreate() end
	if args.MN then graph:MN_recreate() end
	if args.VP then graph:VP_recreate() end
	if args.FT then graph:FT_recreate() end
	if args.FT2 then graph:FT2_recreate() end
	if args.FN then graph:FN_recreate() end
	if args.FM then graph:FM_recreate() end
end

function command.execute(args)
	local fold, fold_error =
		from_fold.fold_from_input_args(args)
	local fold_frames_size = fold:get_frames_size()

	if type(args.frames) == "table" and (#args.frames > 0) then
		local has_frames = true
		for _, index in ipairs(args.frames) do
			if index < 0 or index >= fold_frames_size then
				has_frames = false
				command.print_fold_frame_range_error(
					index, fold_frames_size)
			end
		end

		if not has_frames then
			return fold, fold_error
		end

		for _, index in ipairs(args.frames) do
			local frame = fold:get_frame(index)
			local graph = frame:get_graph()
			command.fold_graph_clear(graph, args)
		end
	else
		for index = 0, (fold_frames_size - 1) do
			local frame = fold:get_frame(index)
			local graph = frame:get_graph()
			command.fold_graph_clear(graph, args)
		end
	end

	to_fold.fold_to_fold_file(fold, args)
	return fold, fold_error
end

return command
