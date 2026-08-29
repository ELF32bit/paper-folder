local command = {}
local FoldFile = require("fold.file_C")

command.name = "convert"
command.aliases = {}
command.description = "from and to FOLD, SVG, OBJ files"
command.help_message = [=[
usage: paper-folder convert <file> <output> [options]]=]

local function fold_from_json_file(file)
	local fold, fold_error = FoldFile.new(), nil
	fold, fold_error = fold:from_json_file(file)
	if fold == nil then error(fold_error) end
	return fold, fold_error
end

local function fold_from_svg_file(file)
	local fold, fold_error = FoldFile.new(), nil
	fold, fold_error = fold:frame_from_svg_file(0, file)
	if fold == nil then error(fold_error) end
	return fold, fold_error
end

local function fold_from_obj_file(file)
	local fold, fold_error = FoldFile.new(), nil
	fold, fold_error = fold:from_obj_file(file)
	if fold == nil then error(fold_error) end
	return fold, fold_error
end

function command.convert_fold_to_svg(file, output_file, arg)
	local fold, fold_error = fold_from_json_file(file)
	fold, fold_error = fold:frame_to_svg_file(0, output_file)
	if fold == false then error(fold_error) end
end

function command.convert_svg_to_fold(file, output_file, arg)
	local fold, fold_error = fold_from_svg_file(file)
	fold, fold_error = fold:to_json_file(output_file, true)
	if fold == false then error(fold_error) end
end

function command.convert_fold_to_obj(file, output_file, arg)
	local fold, fold_error = fold_from_json_file(file)
	fold, fold_error = fold:to_obj_file(output_file)
	if fold == false then error(fold_error) end
end

function command.convert_obj_to_fold(file, output_file, arg)
	local fold, fold_error = fold_from_obj_file(file)
	fold, fold_error = fold:to_json_file(output_file, true)
	if fold == false then error(fold_error) end
end

local function get_file_extension(file)
	local extension = file:match("%.([^.]+)$")
	return extension and extension:lower() or nil
end

local function convert(file, output_file, arg)
	local from = get_file_extension(file)
	local to = get_file_extension(output_file)
	local c = ("convert_%s_to_%s"):format(from, to)
	local converter = command[c]
	if converter ~= nil then
		return converter(file, output_file, arg)
	end
	print(("paper-folder: converting %s to %s not supported")
		:format(from, to))
end

function command.execute(arg)
	local file = arg[2]
	local output_file = arg[3]
	if type(file) ~= "string" or
		type(output_file) ~= "string" or
		file == "-h" or file == "--help" then
		return print(command.help_message)
	end
	convert(file, output_file, arg)
end

return command
