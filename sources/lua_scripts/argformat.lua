local module = {}

local COLORS = require("colors")
local BOLD = COLORS.BOLD
local RESET = COLORS.RESET

local function format_custom_usage_message(message)
	return message:gsub("^Usage:", ("%sUsage%s:\n ")
		:format(BOLD, RESET))
end

function module.enable_custom_usage_message(parser)
	parser:usage(
		format_custom_usage_message(
			parser:get_usage()))
end

local function format_custom_help_message(parser)
	local help = parser:get_help()

	help = help:gsub("\n   %f[%S]", "\n  ")
	help = help:gsub("\n\n([^\n]*)\n\n",
		"\n\nDescription:\n  %1\n\n")

	local help_prefix = help:match(
		"\n([^\n]+)show this help message and exit")
	local HELP_DESCRIPTION_MARGIN = (help_prefix
		and #help_prefix or (25 - 1))

	help = help:gsub(
		"\n%s+(-%S[^\n]*)," ..
		"%s*([^%s%-][^\n]*)\n" ..
		"%s+(--%S[^\n]*) [^\n]*",
		function(short, description, long)
			local left = ("  %s, %s"):format(short, long)
			local pad = math.max(2, HELP_DESCRIPTION_MARGIN - #left)
			return ("\n%s%s%s"):format(left, string.rep(" ", pad),
				description)
		end)

	help = help:gsub("\n(%S[^\n]*):",
		("\n%s%%1%s:"):format(BOLD, RESET))

	return help
end

function module.enable_custom_help_message(parser)
	parser:add_help(false)
	parser:flag("-h --help", "show this help message and exit")
		:action(function()
			print(format_custom_help_message(parser))
			os.exit(0, true)
		end)
end

return module
