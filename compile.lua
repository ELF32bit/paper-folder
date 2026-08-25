EMBED_LUA_SCRIPTS_AS_BYTECODE = true

local function parse_arguments(string)
	local arguments = {}
	for argument in string:gmatch("%S+") do
		table.insert(arguments, argument)
	end
	return arguments
end

local modules_directory = arg[1]
local modules_header = arg[2]
local modules_code = arg[3]
local modules = parse_arguments(arg[4])

local scripts_directory = arg[5]
local scripts_header = arg[6]
local scripts_code = arg[7]
local scripts = parse_arguments(arg[8])

local function parse_lua_module_c_function_name(line)
	local LFN1 = "^%s*int%s+([%a_][%w_]*)%s*%((.*)%)"
	local LFN2 = "^%s*static%s+inline%s+int%s+([%a_][%w_]*)%s*%((.*)%)"
	local LFP1 = "^%s*const%s+lua_State%s*%*%s*([%a_][%w_]*)%s*$"
	local LFP2 = "^%s*lua_State%s*%*%s*([%a_][%w_]*)%s*$"
	local name, parameters = line:match(LFN1)
	if not name then name, parameters = line:match(LFN2) end
	if name and parameters then
		if parameters:match(LFP1) then return name end
		if parameters:match(LFP2) then return name end
	end
end

local function write_lua_modules_header(path)
	local file = assert(io.open(path, "w"))
	file:write("#ifndef LUA_MODULES_H\n")
	file:write("#define LUA_MODULES_H\n\n")
	file:write("#include <stddef.h>\n\n")
	file:write("typedef struct LuaModule {\n")
	file:write("\tconst char* name;\n")
	file:write("\tvoid* open_function;\n")
	file:write("} LuaModule;\n\n")
	file:write("extern const LuaModule g_lua_modules[];\n\n")
	file:write("#endif\n")
	file:close()
end

local function write_lua_modules(directory, path)
	local file = assert(io.open(path, "w"))
	file:write('#include "lua_modules.h"\n\n')
	file:write("#include <lua.h>\n")
	file:write("#include <lauxlib.h>\n\n")

	-- including C modules from the project directory
	for _, module_path in ipairs(modules) do
		file:write(('#include "%s"\n'):format(module_path))
	end
	file:write("\n")

	-- generating module names with _C suffix added
	local module_prefix = ("^%s/"):format(directory)
	for index, module_path in ipairs(modules) do
		local module = module_path:gsub(".h$", "")
		module = module:gsub(module_prefix, "")
		modules[index] = {
			path = module_path,
			name = (module:gsub("/", "_")),
			c_name = (module:gsub("/", "_")) .. "_C",
			lua_name = (module:gsub("/", ".")) .. "_C",
		}
	end

	-- finding Lua functions in C modules
	local functions = {}
	for _, module in ipairs(modules) do
		local module_file = assert(io.open(module.path, "r"))
		local module_functions = { static = {}, meta = {} }
		table.insert(functions, module_functions)
		for line in module_file:lines() do
			local func = parse_lua_module_c_function_name(line)
			if func then
				local lua_name = func:gsub("^l_", "")
				lua_name = lua_name:gsub(("^%s_"):format(module.name), "")
				local function_type = module_functions.static
				if func:match("_meta$") then
					lua_name = lua_name:gsub("_meta$", "")
					function_type = module_functions.meta
				end
				table.insert(function_type, {
					lua_name = lua_name,
					c_name = func,
				})
			end
		end
		module_file:close()
	end

	-- generating static functions from C modules
	for index, module in ipairs(modules) do
		file:write(("static const luaL_Reg %s[] = {\n"):format(module.c_name))
		for _, func in ipairs(functions[index].static) do
			file:write(('\t{ "%s", %s }, \n'):format(func.lua_name, func.c_name))
		end
		file:write("\t{ NULL, NULL }\n};\n\n")
	end

	-- generating registration functions for C modules
	for index, module in ipairs(modules) do
		file:write(("static int luaopen_%s(lua_State* L) {\n"):format(module.c_name))
		if #functions[index].meta > 0 then
			file:write(('\tluaL_newmetatable(L, "%s.instance");\n'):format(module.c_name))
			for _, func in ipairs(functions[index].meta) do
				file:write(("\tlua_pushcfunction(L, %s);\n"):format(func.c_name));
				file:write(('\tlua_setfield(L, -2, "%s");\n'):format(func.lua_name));
			end
			file:write(("\tlua_pop(L, 1);\n"))
		end
		file:write(("\tluaL_newlib(L, %s);\n"):format(module.c_name))
		file:write("\treturn 1;\n}\n\n")
	end

	-- generating C modules registration table
	file:write("const LuaModule g_lua_modules[] = {\n")
	for _, module in ipairs(modules) do
		local m = ("luaopen_%s"):format(module.c_name)
		file:write(('\t{ "%s", %s }, \n'):format(module.lua_name, m))
	end
	file:write("\t{ NULL, NULL }\n};\n")

	file:close()
end

local function string_to_formatted_bytes(data)
	local bytes = {}
	local data_size = data:len()
	for index = 1, data_size do
		if index % 12 == 1 then
			table.insert(bytes, (index == 1) and "\t" or "\n\t")
		end
		table.insert(bytes, ("0x%02x"):format(data:byte(index)))
		table.insert(bytes, (index ~= data_size) and ", " or "\n")
	end
	return table.concat(bytes)
end

local function lua_script_to_formatted_bytes(path)
	local file = assert(io.open(path, "rb"))
	local data = file:read("*a")
	if EMBED_LUA_SCRIPTS_AS_BYTECODE then
		local _load51 = loadstring or load
		data = string.dump(assert(_load51(data)))
	end
	file:close()
	return string_to_formatted_bytes(data)
end

local function write_lua_scripts_header(path)
	local file = assert(io.open(path, "w"))
	file:write("#ifndef LUA_SCRIPTS_H\n")
	file:write("#define LUA_SCRIPTS_H\n\n")
	file:write("#include <stddef.h>\n\n")
	file:write("typedef struct LuaScript {\n")
	file:write("\tconst char* name;\n")
	file:write("\tconst char* data;\n")
	file:write("\tsize_t data_size;\n")
	file:write("} LuaScript;\n\n")
	file:write("extern const LuaScript g_lua_scripts[];\n\n")
	file:write("#endif\n")
	file:close()
end

local function write_lua_scripts(directory, path)
	local file = assert(io.open(path, "w"))
	file:write('#include "lua_scripts.h"\n\n')

	-- finding module names for Lua scripts
	local scripts_prefix = ("^%s/"):format(directory)
	for index, script_path in ipairs(scripts) do
		local module = script_path:gsub(".lua$", "")
		module = module:gsub(scripts_prefix, "")
		scripts[index] = { script_path, module }
	end

	-- generating Lua scripts data
	for _, value in ipairs(scripts) do
		local script_path, module = value[1], value[2]
		local s = ("script_%s"):format(module:gsub("/", "_"))
		file:write(("static const char %s[] = {\n"):format(s))
		file:write(lua_script_to_formatted_bytes(script_path))
		file:write("};\n\n")
	end

	-- generating registration table for Lua scripts
	file:write("const LuaScript g_lua_scripts[] = {\n")
	for _, value in ipairs(scripts) do
		local module, m = value[2], value[2]:gsub("/", ".")
		local s = ("script_%s"):format(module:gsub("/", "_"))
		file:write(('\t{ "%s", %s, sizeof(%s) }, \n'):format(m, s, s))
	end
	file:write("\t{ NULL, NULL, 0 }\n};\n")

	file:close()
end

-- generating Lua modules from C sources
write_lua_modules_header(modules_header)
write_lua_modules(modules_directory, modules_code)

-- embedding Lua scripts into C sources
write_lua_scripts_header(scripts_header)
write_lua_scripts(scripts_directory, scripts_code)
