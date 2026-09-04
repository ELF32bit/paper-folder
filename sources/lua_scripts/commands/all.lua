local module = {}
local argparse = require("thirdparty.argparse")

local commands = {
	(require("commands.generate")),
	(require("commands.convert")),
	(require("commands.convert.fold")),
	(require("commands.convert.svg")),
	(require("commands.convert.obj")),
	(require("commands.validate")),
	(require("commands.test")),
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
