local command = { name = "from-json", group = "Conversion From Commands" }
local FoldFile = require("fold.file_C")

function command.register(parser)
	local convert = parser:command(command.name)
		:description("from JSON file to FOLD file")

	convert:argument("input"):args("?")
		:description("path to the JSON source")

	convert:group("Options",
		convert:option("-o --output"):argname("<file>")
		:description("path to the FOLD destination"),

		convert:flag("-p --pretty")
		:description("beautify the FOLD output")
	)

	return convert
end

function command.fold_from_json_file(args)
	local fold, fold_error = FoldFile.new()
	if fold == nil then command.ERROR(fold_error) end
	fold, fold_error = fold:from_json_file(args.input)
	if fold == nil then command.ERROR(fold_error) end
	return fold, fold_error
end

function command.fold_from_json_string(json, args)
	local fold, fold_error = FoldFile.new()
	if fold == nil then command.ERROR(fold_error) end
	fold, fold_error = fold:from_json_string(json)
	if fold == nil then command.ERROR(fold_error) end
	return fold, fold_error
end

function command.read_stdin_stream()
	local io_file, io_error = io.read("*a")
	if io_file == nil then command.ERROR(io_error) end
	return io_file, io_error
end

function command.fold_from_input_args(args)
	if args.input == nil then
		local io_file, _ = command.read_stdin_stream()
		return command.fold_from_json_string(io_file, args)
	else
		return command.fold_from_json_file(args)
	end
end

function command._fold_to_json_file(fold, args)
	local json, json_error
	if args.output == nil then
		json, json_error = fold:to_json_string(args.pretty)
		if json == nil then command.ERROR(json_error) end
		print(json)
	else
		json, json_error = fold:to_json_file(args.output, args.pretty)
		if json == false then command.ERROR(json_error) end
	end
end

function command.execute(args)
	local fold, fold_error =
		command.fold_from_input_args(args)
	command._fold_to_json_file(fold, args)
	return fold, fold_error
end

return command
