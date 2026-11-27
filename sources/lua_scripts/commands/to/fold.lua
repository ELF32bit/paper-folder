local command = { name = "to", group = "Conversion To" }
local from_fold = require("commands.from.fold")
local from_json = require("commands.from.json")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("to FOLD file (pipeable)")

	convert:argument("input"):args("?")
		:description("path to the FOLD source")

	convert:group("Options",
		convert:option("-o --output"):argname("<file>")
		:description("path to the FOLD destination"),

		convert:flag("-p --pretty")
		:description("beautify the FOLD output")
	)

	return convert
end

function command.fold_to_fold_file(fold, args)
	return from_json._fold_to_json_file(fold, args)
end

function command.execute(args)
	return from_fold.execute(args)
end

return command
