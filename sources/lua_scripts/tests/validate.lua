local TEST_VALIDATE = {}
local TEST_FILES = require("tests.files")

FOLD = require("fold.file_C")

ANSI_COLORS = require("definitions.colors_C")
local BOLD = ANSI_COLORS.BOLD()
local RED = ANSI_COLORS.RED()
local GREEN = ANSI_COLORS.GREEN()
local YELLOW = ANSI_COLORS.YELLOW()
local WHITE = ANSI_COLORS.WHITE()
local GRAY = ANSI_COLORS.GRAY()
local RESET = ANSI_COLORS.RESET()

local function format_validation_errors(file, is_valid, errors)
	if is_valid then
		io.write(('%s%sVALID%s: %s%s"%s"%s\n'):format(
			BOLD, GREEN, RESET, BOLD, WHITE, file, RESET))
		return
	else
		io.write(('%s%sINVALID%s: %s%s"%s"%s\n'):format(
			BOLD, RED, RESET, BOLD, WHITE, file, RESET))
	end

	local frame = 0
	for line in errors:gmatch("[^\n]+") do
		if line:sub(1, 7) == "FRAME: " then
			frame = line:sub(8)
		elseif line:sub(1, 7) == "ERROR: " then
			io.write(("%s%sframe:%d%s: %s%s%s\n"):format(
				BOLD, GRAY, frame, RESET,
				RED, line:sub(8), RESET))
		elseif line:sub(1, 9) == "WARNING: " then
			io.write(("%s%sframe:%d%s: %s%s%s\n"):format(
				BOLD, GRAY, frame, RESET,
				YELLOW, line:sub(10), RESET))
		end
	end
end

function TEST_VALIDATE.run_file(file)
	local fold, fold_error = FOLD.new(), nil
	fold, fold_error = fold:from_json_file(file)
	if (fold == nil) then error(fold_error) end
	io.write(format_validation_errors(
		file, fold:validate()))
end

function TEST_VALIDATE.run_all()
	for _, file in ipairs(TEST_FILES.FOLD_FILES) do
		TEST_VALIDATE.run_file(file)
	end
end

return TEST_VALIDATE
