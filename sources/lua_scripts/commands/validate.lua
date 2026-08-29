local command = {}
local FoldFile = require("fold.file_C")
local COLORS = require("definitions.colors_C")

command.name = "validate"
command.aliases = {}
command.description = "validate a specific FOLD file"
command.help_message = [[
usage: ./paper-folder validate <file>]]

local BOLD = COLORS.BOLD()
local RED = COLORS.RED()
local GREEN = COLORS.GREEN()
local YELLOW = COLORS.YELLOW()
local WHITE = COLORS.WHITE()
local GRAY = COLORS.GRAY()
local RESET = COLORS.RESET()

function command.print_fold_validation_errors(file, is_valid, errors)
	if is_valid then
		print(('%s%sVALID%s: %s%s"%s"%s'):format(
			BOLD, GREEN, RESET, BOLD, WHITE, file, RESET))
		return
	else
		print(('%s%sINVALID%s: %s%s"%s"%s'):format(
			BOLD, RED, RESET, BOLD, WHITE, file, RESET))
	end
	local frame = 0
	for line in errors:gmatch("[^\n]+") do
		if line:sub(1, 7) == "FRAME: " then
			frame = line:sub(8)
		elseif line:sub(1, 7) == "ERROR: " then
			print(("%s%sframe:%d%s: %s%s%s"):format(
				BOLD, GRAY, frame, RESET,
				RED, line:sub(8), RESET))
		elseif line:sub(1, 9) == "WARNING: " then
			print(("%s%sframe:%d%s: %s%s%s"):format(
				BOLD, GRAY, frame, RESET,
				YELLOW, line:sub(10), RESET))
		end
	end
end

function command.validate_fold_file(file)
	local fold, fold_error = FoldFile.new(), nil
	fold, fold_error = fold:from_json_file(file)
	if fold == nil then error(fold_error) end
	command.print_fold_validation_errors(
		file, fold:validate())
end

function command.execute(arg)
	local file = arg[2]
	if type(file) ~= "string" or
		file == "-h" or file == "--help" then
		return print(command.help_message)
	end
	if type(arg[3]) == "string" then
		print(("paper-folder: unrecognized option '%s'")
			:format(arg[3]))
		return print(command.help_message)
	end
	command.validate_fold_file(file)
end

return command
