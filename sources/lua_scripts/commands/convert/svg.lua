local command = { name = "convert-svg", group = "Conversion Commands" }
local convert_from = require("commands.convert.from")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from any supported file to SVG file")

	convert:usage_max_width(80)

	convert:argument("input_file")
		:description("path to the convertible source")
	convert:argument("output_file")
		:description("optional path to the destination")
		:args("?")

	convert:option("-f --frame"):argname("INDEX")
		:description("FOLD frame index")
		:convert(tonumber)
		:default("0"):show_default(true)

	return convert
end

function command.convert_fold_to_svg(fold, args)
	local svg, svg_error = fold:frame_to_svg_file(args.output_file, args.frame)
	if svg == false then error(svg_error, 0) end
end

function command.execute(args)
	local fold, _ = convert_from.execute(args)
	command.convert_fold_to_svg(fold, args)
end

return command
