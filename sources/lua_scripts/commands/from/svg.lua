local command = { name = "from-svg", group = "Conversion Commands" }
local from_obj = require("commands.from.obj")
local to_fold = require("commands.to.fold")
local FoldFile = require("fold.file_C")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from SVG file to FOLD file")

	convert:argument("input"):args("?")
		:description("path to the SVG source")
	convert:option("-o --output")
		:description("path to the FOLD destination")

	convert:option("-f"):argname("UNITS")
		:target("flatness")
		:description("curve flatness")
		:convert(tonumber)
		:default("0.5")
		:show_default(true)

	convert:option("-l"):argname("UNITS")
		:target("min_segment_length")
		:description("min segment length")
		:convert(tonumber)
		:default("1.0")
		:show_default(true)

	return convert
end

function command.fold_from_svg_file(args)
	local fold, fold_error = FoldFile.new()
	if fold == nil then error(fold_error, 0) end

	fold, fold_error =
		fold:frame_from_svg_file(args.input, args.frame,
			args.flatness, args.min_segment_length)

	if fold == nil then error(fold_error, 0) end
	return fold, fold_error
end

function command.__create_stdin_file(stdin)
	return from_obj.__create_stdin_file(stdin)
end

function command.fold_from_input_args(args)
	local fold, fold_error
	if args.input == nil then
		local io_svg, io_error = io.read("*a")
		if io_svg == nil then error(io_error, 0) end
		local io_path = command.__create_stdin_file(io_svg)

		local success
		args.input = io_path
		success, fold, fold_error = pcall(
			command.fold_from_svg_file, args)
		os.remove(io_path)
		args.input = nil

		if success == false then error(fold, 0) end
	else
		fold, fold_error =
			command.fold_from_svg_file(args)
	end
	return fold, fold_error
end

local function prepare_arguments(args)
	args.flatness = math.max(args.flatness, 0.0)
	args.min_segment_length = math.max(args.min_segment_length, 0.0)
end

function command.execute(args)
	prepare_arguments(args)
	local fold, fold_error =
		command.fold_from_input_args(args)
	to_fold.fold_to_fold_file(fold, args)
	return fold, fold_error
end

return command
