local command = { name = "convert", group = "Conversion Commands" }
local convert_from = require("commands.convert.from")
local convert_to = {
	["fold"] = require("commands.convert.fold"),
	["svg"] = require("commands.convert.svg"),
	["obj"] = require("commands.convert.obj"),
}

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from JSON, SVG, OBJ file to FOLD file")

	convert:argument("input_file")
		:description("path to the convertible source")
	convert:argument("output_file")
		:description("path to the destination")

	return convert
end

function command.execute(args)
	convert_from.execute_without_converting(args)
	local to = convert_from.get_file_extension(args.output_file)
	local converter = convert_to[to]
	if converter ~= nil then return converter.execute(args) end
	error(("converting to %s not supported"):format(to), 0)
end

return command
