local command = { name = "generate", group = "Generation Commands" }
local convert_svg = require("commands.convert.svg")
local FoldGeneration = require("fold.generation_C")

function command.register(parser)
	local generate = parser:command(command.name)
		:description("locally flat-foldable crease pattern")

	generate:usage_max_width(80)
	generate:usage_margin(24)

	generate:option("-b"):argname("MIN")
		:target("min_boundary_vertices")
		:description("min number of boundary vertices")
		:convert(tonumber)
		:default("4"):show_default(true)
	generate:option("-B"):argname("MAX")
		:target("max_boundary_vertices")
		:description("max number of boundary vertices")
		:convert(tonumber)
		:default("4"):show_default(true)
	generate:option("-i"):argname("MIN")
		:target("min_interior_vertices")
		:description("min number of interior vertices")
		:convert(tonumber)
		:default("0"):show_default(true)
	generate:option("-I"):argname("MAX")
		:target("max_interior_vertices")
		:description("max number of interior vertices")
		:convert(tonumber)
		:default("0"):show_default(true)

	generate:option("-o"):argname("PERCENT")
		:target("boundary_offset")
		:description("min boundary offset distance")
		:convert(tonumber)
		:default("0.015625"):show_default(true)
	generate:option("-c"):argname("PERCENT")
		:target("cell_size")
		:description("cell size for 3x3 vertex merging")
		:convert(tonumber)
		:default("0.015625"):show_default(true)
	generate:option("-e"):argname("DEGREES")
		:target("kawasaki_error")
		:description("max kawasaki theorem deviation")
		:convert(tonumber)
		:default("1.0"):show_default(true)

	generate:option("-s"):argname("SEED")
		:target("seed")
		:description("seed for random generator")
		:convert(tonumber)
		:default("0"):show_default(true)

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
	local fold, fold_error = FoldGeneration.locally_flat_foldable(
		args.min_boundary_vertices,
		args.max_boundary_vertices,
		args.min_interior_vertices,
		args.max_interior_vertices,
		args.boundary_offset,
		args.cell_size,
		args.kawasaki_error,
		args.seed)
	if fold == nil then error(fold_error, 0) end
	args.output_file = ("b=%s;B=%s;i=%s;I=%s;o=%s;c=%s;e=%s;s=%s.svg")
		:format(
			args.min_boundary_vertices,
			args.max_boundary_vertices,
			args.min_interior_vertices,
			args.max_interior_vertices,
			args.boundary_offset,
			args.cell_size,
			args.kawasaki_error,
			args.seed)
	convert_svg.convert_fold_to_svg(fold, args)
end

return command
