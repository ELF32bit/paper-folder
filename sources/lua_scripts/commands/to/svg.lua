local command = { name = "to-svg", group = "Conversion Commands" }
local from_fold = require("commands.from.fold")

local COLORS = require("colors")
local BOLD = COLORS.BOLD
local RED = COLORS.RED
local RESET = COLORS.RESET

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from FOLD file to SVG file")

	convert:argument("input"):args("?")
		:description("path to the FOLD source")
	convert:option("-o --output")
		:description("path to the SVG destination")

	convert:option("-f --frame"):argname("INDEX")
		:description("FOLD frame index")
		:convert(tonumber)
		:default("0")
		:show_default(true)

	return convert
end

function command.fold_to_svg_file(fold, args)
	local svg, svg_error =
		fold:frame_to_svg_file(args.output, args.frame)
	if svg == false then error(svg_error, 0) end
	return svg, svg_error
end

function command.execute(args)
	local fold, fold_error =
		from_fold.fold_from_input_args(args)

	local fold_frames_size = fold:get_frames_size()
	if args.frame < 0 or args.frame >= fold_frames_size then
		print(("%s%sERROR%s: %sframe %s is not within 0-%s range%s")
			:format(BOLD, RED, RESET, RED, args.frame,
				fold_frames_size - 1, RESET))
		return fold, fold_error
	end

	command.fold_to_svg_file(fold, args)
	return fold, fold_error
end

return command
