local command = { name = "from-svg", group = "Conversion From Commands" }
local from_obj = require("commands.from.obj")
local to_fold = require("commands.to.fold")
local FoldFile = require("fold.file_C")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from SVG file to FOLD file")

	convert:argument("input"):args("?")
		:description("path to the SVG source")

	convert:group("Options",
		convert:option("-o --output"):argname("<file>")
		:description("path to the FOLD destination"),

		convert:option("-s --scale"):argname("<float>")
		:description("scale coordinates")
		:convert(tonumber)
		:default("1.0")
		:show_default(true),

		convert:option("-f"):argname("<float>")
		:description("curve flatness")
		:convert(tonumber)
		:default("0.5")
		:show_default(true)
		:target("flatness"),

		convert:option("-l"):argname("<float>")
		:description("min segment length")
		:convert(tonumber)
		:default("1.0")
		:show_default(true)
		:target("min_segment_length"),

		convert:flag("-t --triangulate")
		:description("triangulate by intersecting paths")
	)

	convert:group("Crease Colors",
		convert:option("--B"):argname("<html>")
		:description("boundary crease color")
		:default("#000000ff")
		:show_default(true)
		:target("B_color"),

		convert:option("--M"):argname("<html>")
		:description("mountain crease color")
		:default("#ff0000ff")
		:show_default(true)
		:target("M_color"),

		convert:option("--V"):argname("<html>")
		:description("valley crease color")
		:default("#0000ffff")
		:show_default(true)
		:target("V_color"),

		convert:option("--F"):argname("<html>")
		:description("flat crease color")
		:default("#808080ff")
		:show_default(true)
		:target("F_color"),

		convert:option("--U"):argname("<html>")
		:description("unknown crease color")
		:default("#ffff00ff")
		:show_default(true)
		:target("U_color"),

		convert:option("--C"):argname("<html>")
		:description("cut crease color")
		:default("#00ff00ff")
		:show_default(true)
		:target("C_color"),

		convert:option("--J"):argname("<html>")
		:description("join crease color")
		:default("#00000040")
		:show_default(true)
		:target("J_color")
	)

	convert:group("Image Options",
		convert:option("-u --units"):argname("<string>")
		:description("SVG pt|pc|mm|cm|in units")
		:default("px")
		:show_default(true)
		:target("image_units"),

		convert:option("-d --dpi"):argname("<float>")
		:description("SVG dots per inch")
		:default("96.0")
		:show_default(true)
		:target("image_dpi")
	)

	return convert
end

function command.fold_from_svg_file(args)
	local fold, fold_error = FoldFile.new()
	if fold == nil then command.ERROR(fold_error) end

	fold, fold_error =
		fold:frame_from_svg_file(args.input, args.frame,
			args.image_units, args.image_dpi,
			args.flatness,
			args.min_segment_length)

	if fold == nil then command.ERROR(fold_error) end
	return fold, fold_error
end

function command.fold_from_svg_string(svg, args)
	local fold, fold_error = FoldFile.new()
	if fold == nil then command.ERROR(fold_error) end

	fold, fold_error =
		fold:frame_from_svg_string(svg, args.frame,
			args.image_units, args.image_dpi,
			args.flatness,
			args.min_segment_length)

	if fold == nil then command.ERROR(fold_error) end
	return fold, fold_error
end

function command.read_stdin_stream()
	return from_obj.read_stdin_stream()
end

function command.__create_stdin_file(io_file)
	return from_obj.__create_stdin_file(io_file)
end

function command.__fold_from_svg_stdin_file(io_file, args)
	args.input = command.__create_stdin_file(io_file)

	local success, fold, fold_error =
		pcall(command.fold_from_svg_file, args)
	os.remove(args.input)
	args.input = nil

	if success == false then command.ERROR(fold) end
	return fold, fold_error
end

function command.fold_from_input_args(args)
	if args.input == nil then
		local io_file, _ = command.read_stdin_stream()
		return command.fold_from_svg_string(io_file, args)
	else
		return command.fold_from_svg_file(args)
	end
end

function command.execute(args)
	local fold, fold_error =
		command.fold_from_input_args(args)
	to_fold.fold_to_fold_file(fold, args)
	return fold, fold_error
end

return command
