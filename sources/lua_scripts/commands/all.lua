local module = {}
local argparse = require("argparse")

local commands = {
	(require("commands.generate")),
	(require("commands.from.fold")),
	(require("commands.from.json")),
	(require("commands.from.svg")),
	(require("commands.from.obj")),
	(require("commands.to.fold")),
	(require("commands.to.json")),
	(require("commands.to.svg")),
	(require("commands.to.obj")),
	(require("commands.describe")),
	(require("commands.validate")),
	(require("commands.test.describe")),
	(require("commands.test.validate")),
}

local function create_parser()
	local parser = argparse("paper-folder")
		:description("powerful origami toolkit")

	parser:require_command(true)

	return parser
end

local function register_commands(parser)
	local groups, order = {}, {}
	for _, command in ipairs(commands) do
		local object = command.register(parser)
		local group_name = command.group or "Other Commands"
		if groups[group_name] == nil then
			groups[group_name] = { object }
			table.insert(order, group_name)
		else
			table.insert(groups[group_name], object)
		end
	end
	for _, group_name in ipairs(order) do
		local objects = groups[group_name]
		parser:group(group_name, table.unpack(objects))
	end
end

local function execute_command(args)
	for _, command in ipairs(commands) do
		if args[command.name] then
			command.execute(args)
			break
		end
	end
end

function module.execute()
	local parser = create_parser()
	register_commands(parser)
	local args = parser:parse()
	execute_command(args)
end

return module
