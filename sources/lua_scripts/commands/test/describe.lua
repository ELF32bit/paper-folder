local command = { name = "test-describe", group = "Other Commands" }
local describe = require("commands.describe")

function command.register(parser)
	local test = parser:command(command.name)
		:description("run describe tests")

	test:option("-f --frame"):argname("INDEX")
		:description("FOLD frame index")
		:convert(tonumber)
		:count("*")

	test:flag("-i --inherit")
		:description("inherit frames")

	test:flag("-a --all")
		:description("describe all frames")

	test:flag("-r --reverse")
		:description("reverse frames description")

	return test
end

function command.execute(args)
	local assets = require("assets")
	for _, file in ipairs(assets.FOLD_FILES) do
		args.input = file
		describe.execute(args)
	end
	args.input = nil
end

return command
