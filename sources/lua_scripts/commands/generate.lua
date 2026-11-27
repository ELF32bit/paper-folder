local command = { name = "generate", group = "Experimental" }
local FoldGeneration = require("fold.generation_C")
local to_svg = require("commands.to.svg")

function command.register(parser)
	local generate = parser:command(command.name)
		:description("locally flat-foldable crease pattern")

	generate:usage_max_width(80)
	generate:usage_margin(24)

	generate:option("-b"):argname("MIN")
		:description("min number of boundary vertices")
		:convert(tonumber)
		:default("4")
		:show_default(true)
		:target("min_boundary_vertices")

	generate:option("-B"):argname("MAX")
		:description("max number of boundary vertices")
		:convert(tonumber)
		:default("4")
		:show_default(true)
		:target("max_boundary_vertices")

	generate:option("-i"):argname("MIN")
		:description("min number of interior vertices")
		:convert(tonumber)
		:default("0")
		:show_default(true)
		:target("min_interior_vertices")

	generate:option("-I"):argname("MAX")
		:description("max number of interior vertices")
		:convert(tonumber)
		:default("0")
		:show_default(true)
		:target("max_interior_vertices")

	generate:option("-o"):argname("PERCENT")
		:description("min boundary offset distance")
		:convert(tonumber)
		:default("0.015625")
		:show_default(true)
		:target("boundary_offset")

	generate:option("-c"):argname("PERCENT")
		:description("cell size for 3x3 vertex merging")
		:convert(tonumber)
		:default("0.015625")
		:show_default(true)
		:target("cell_size")

	generate:option("-e"):argname("DEGREES")
		:description("max Kawasaki theorem deviation")
		:convert(tonumber)
		:default("1.0")
		:show_default(true)
		:target("kawasaki_error")

	generate:option("-s"):argname("SEED")
		:description("seed for random generator")
		:convert(tonumber)
		:default("0")
		:show_default(true)
		:target("seed")

	return generate
end

local function prepare_arguments(args)
	args.min_boundary_vertices = math.max(args.min_boundary_vertices, 4)
	args.max_boundary_vertices = math.max(args.max_boundary_vertices, 4)

	args.max_boundary_vertices =
		math.max(args.min_boundary_vertices, args.max_boundary_vertices)
	args.max_interior_vertices =
		math.max(args.min_interior_vertices, args.max_interior_vertices)

	args.boundary_offset = math.max(0.0, math.min(0.5, args.boundary_offset))
	args.cell_size = math.max(1.0 / 1024.0, math.min(0.25, args.cell_size))
	args.kawasaki_error = math.max(args.kawasaki_error, 0.0)
end

function command.execute(args)
	prepare_arguments(args)
	local fold, fold_error =
		FoldGeneration.locally_flat_foldable(
			args.min_boundary_vertices,
			args.max_boundary_vertices,
			args.min_interior_vertices,
			args.max_interior_vertices,
			args.boundary_offset,
			args.cell_size,
			args.kawasaki_error,
			args.seed)
	if fold == nil then
		command.ERROR(fold_error)
	end

	args.output =
		("b=%s;B=%s;i=%s;I=%s;o=%s;c=%s;e=%s;s=%s.svg")
		:format(
			args.min_boundary_vertices,
			args.max_boundary_vertices,
			args.min_interior_vertices,
			args.max_interior_vertices,
			args.boundary_offset,
			args.cell_size,
			args.kawasaki_error,
			args.seed)

	to_svg.fold_to_svg_file(fold, args)
end

return command
