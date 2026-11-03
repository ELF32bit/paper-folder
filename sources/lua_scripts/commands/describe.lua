local command = { name = "describe", group = "Inspection Commands" }
local from_fold = require("commands.from.fold")

local COLORS = require("colors")
local BOLD = COLORS.BOLD
local RED = COLORS.RED
local BLUE = COLORS.BLUE
local WHITE = COLORS.WHITE
local GRAY = COLORS.GRAY
local RESET = COLORS.RESET

function command.register(parser)
	local describe = parser:command(command.name)
		:description("describe FOLD file")

	describe:argument("input"):args("?")
		:description("path to the FOLD source")

	describe:option("-f --frame"):argname("INDEX")
		:description("FOLD frame index")
		:convert(tonumber)
		:count("*")

	describe:flag("-a --all")
		:description("describe all frames")

	describe:flag("-r --reverse")
		:description("reverse description")

	return describe
end

function command.print_fold_description(fold, args)
	print(('%s%sFOLD%s: %s%s"%s"%s'):format(
		BOLD, BLUE, RESET, BOLD, WHITE,
		args.input or "STDIN", RESET))

	local fold_version = fold:get_version()
	print(("%sfile_version%s: %s%s%s"):format(
		BLUE, RESET, BOLD, fold_version, RESET))

	local fold_creator = fold:get_creator()
	if fold_creator ~= "" then
		print(("%sfile_creator%s: %s%s%s"):format(
			BLUE, RESET, BOLD, fold_creator, RESET))
	end

	local fold_author = fold:get_author()
	if fold_author ~= "" then
		print(("%sfile_author%s: %s%s%s"):format(
			BLUE, RESET, BOLD, fold_author, RESET))
	end

	local fold_title = fold:get_title()
	if fold_title ~= "" then
		print(("%sfile_title%s: %s%s%s"):format(
			BLUE, RESET, BOLD, fold_title, RESET))
	end

	local fold_description = fold:get_description()
	if fold_description ~= "" then
		print(("%sfile_description%s: %s%s%s"):format(
			BLUE, RESET, BOLD, fold_description, RESET))
	end

	local fold_classes = fold:get_classes()
	if fold_classes[1] ~= nil then
		local fold_classes_size = #fold_classes
		io.write(("%sfile_classes%s: "):format(BLUE, RESET))
		for i, fold_class in ipairs(fold_classes) do
			io.write(BOLD)
			io.write(fold_class)
			io.write(RESET)
			if i ~= fold_classes_size then
				io.write(", ")
			end
		end
		io.write("\n")
	end

	local fold_frames = fold:get_frames()
	if #fold_frames > 1 then
		print(("%sfile_frames%s: %s%s%s"):format(
			BLUE, RESET, BOLD, #fold_frames, RESET))
	end

	return fold_frames
end

function command.print_fold_frame_description(fold, frame, index, args)
	print(("%s%sframe:%d%s"):format(
		BOLD, GRAY, index - 1, RESET))

	local frame_author = fold:get_frame_author(frame)
	if frame_author ~= "" then
		print(("%sframe_author%s: %s%s%s"):format(
			GRAY, RESET, BOLD, frame_author, RESET))
	end

	local frame_title = fold:get_frame_title(frame)
	if frame_title ~= "" then
		print(("%sframe_title%s: %s%s%s"):format(
			GRAY, RESET, BOLD, frame_title, RESET))
	end

	local frame_description = fold:get_frame_description(frame)
	if frame_description ~= "" then
		print(("%sframe_description%s: %s%s%s"):format(
			GRAY, RESET, BOLD, frame_description, RESET))
	end

	local frame_classes = fold:get_frame_classes(frame)
	if frame_classes[1] ~= nil then
		local frame_classes_size = #frame_classes
		io.write(("%sframe_classes%s: "):format(GRAY, RESET))
		for i, frame_class in ipairs(frame_classes) do
			io.write(BOLD)
			io.write(frame_class)
			io.write(RESET)
			if i ~= frame_classes_size then
				io.write(", ")
			end
		end
		io.write("\n")
	end

	local frame_attributes = fold:get_frame_attributes(frame)
	if frame_attributes[1] ~= nil then
		local frame_attributes_size = #frame_attributes
		io.write(("%sframe_attributes%s: "):format(GRAY, RESET))
		for i, frame_attribute in ipairs(frame_attributes) do
			io.write(BOLD)
			io.write(frame_attribute)
			io.write(RESET)
			if i ~= frame_attributes_size then
				io.write(", ")
			end
		end
		io.write("\n")
	end

	local frame_unit = fold:get_frame_unit(frame)
	if frame_unit ~= "" then
		print(("%sframe_unit%s: %s%s%s"):format(
			GRAY, RESET, BOLD, frame_unit, RESET))
	end

	local frame_graph = fold:get_frame_graph(frame)
	local VC = fold:get_frame_graph_VC_size(frame_graph)
	local VV = fold:get_frame_graph_VV_size(frame_graph)
	local VE = fold:get_frame_graph_VE_size(frame_graph)
	local VF = fold:get_frame_graph_VF_size(frame_graph)
	local EV = fold:get_frame_graph_EV_size(frame_graph)
	local EF = fold:get_frame_graph_EF_size(frame_graph)
	local EA = fold:get_frame_graph_EA_size(frame_graph)
	local EFA = fold:get_frame_graph_EFA_size(frame_graph)
	local EL = fold:get_frame_graph_EL_size(frame_graph)
	local EO = fold:get_frame_graph_EO_size(frame_graph)
	local FV = fold:get_frame_graph_FV_size(frame_graph)
	local FE = fold:get_frame_graph_FE_size(frame_graph)
	local FF = fold:get_frame_graph_FF_size(frame_graph)
	local FO = fold:get_frame_graph_FO_size(frame_graph)
	local PC = fold:get_frame_graph_PC_size(frame_graph)
	local TC = fold:get_frame_graph_TC_size(frame_graph)
	local T2C = fold:get_frame_graph_T2C_size(frame_graph)
	local NC = fold:get_frame_graph_NC_size(frame_graph)
	local MN = fold:get_frame_graph_MN_size(frame_graph)
	local VP = fold:get_frame_graph_VP_size(frame_graph)
	local FT = fold:get_frame_graph_FT_size(frame_graph)
	local FT2 = fold:get_frame_graph_FT2_size(frame_graph)
	local FN = fold:get_frame_graph_FN_size(frame_graph)
	local FM = fold:get_frame_graph_FM_size(frame_graph)

	local __ = ""
	if VC ~= 0 then __ = __ .. ("%sVC%s, "):format(BOLD, RESET) end
	if VV ~= 0 then __ = __ .. ("%sVV%s, "):format(BOLD, RESET) end
	if VE ~= 0 then __ = __ .. ("%sVE%s, "):format(BOLD, RESET) end
	if VF ~= 0 then __ = __ .. ("%sVF%s, "):format(BOLD, RESET) end
	if EV ~= 0 then __ = __ .. ("%sEV%s, "):format(BOLD, RESET) end
	if EF ~= 0 then __ = __ .. ("%sEF%s, "):format(BOLD, RESET) end
	if EA ~= 0 then __ = __ .. ("%sEA%s, "):format(BOLD, RESET) end
	if EFA ~= 0 then __ = __ .. ("%sEFA%s, "):format(BOLD, RESET) end
	if EL ~= 0 then __ = __ .. ("%sEL%s, "):format(BOLD, RESET) end
	if EO ~= 0 then __ = __ .. ("%sEO%s, "):format(BOLD, RESET) end
	if FV ~= 0 then __ = __ .. ("%sFV%s, "):format(BOLD, RESET) end
	if FE ~= 0 then __ = __ .. ("%sFE%s, "):format(BOLD, RESET) end
	if FF ~= 0 then __ = __ .. ("%sFF%s, "):format(BOLD, RESET) end
	if FO ~= 0 then __ = __ .. ("%sFO%s, "):format(BOLD, RESET) end
	if PC ~= 0 then __ = __ .. ("%sPC%s, "):format(BOLD, RESET) end
	if TC ~= 0 then __ = __ .. ("%sTC%s, "):format(BOLD, RESET) end
	if T2C ~= 0 then __ = __ .. ("%sT2C%s, "):format(BOLD, RESET) end
	if NC ~= 0 then __ = __ .. ("%sNC%s, "):format(BOLD, RESET) end
	if MN ~= 0 then __ = __ .. ("%sMN%s, "):format(BOLD, RESET) end
	if VP ~= 0 then __ = __ .. ("%sVP%s, "):format(BOLD, RESET) end
	if FT ~= 0 then __ = __ .. ("%sFT%s, "):format(BOLD, RESET) end
	if FT2 ~= 0 then __ = __ .. ("%sFT2%s, "):format(BOLD, RESET) end
	if FN ~= 0 then __ = __ .. ("%sFN%s, "):format(BOLD, RESET) end
	if FM ~= 0 then __ = __ .. ("%sFM%s, "):format(BOLD, RESET) end

	if #__ > 0 then
		print(("%sframe_graph%s: %s%s%s"):format(
			GRAY, RESET, BOLD, __:sub(1, -3), RESET))
	end

	local frame_parent = fold:get_frame_parent(frame)
	if frame_parent ~= nil then
		print(("%sframe_parent%s: %s%s%s"):format(
			GRAY, RESET, BOLD, frame_parent, RESET))
	end

	local frame_inherit = fold:get_frame_inherit(frame)
	if frame_inherit == true then
		print(("%sframe_inherit%s: %strue%s"):format(
			GRAY, RESET, BOLD, RESET))
	end
end

function command.execute(args)
	local fold, fold_error =
		from_fold.fold_from_input_args(args)

	if not args.reverse then
		local fold_frames =
			command.print_fold_description(fold, args)
		if args.all then
			for index, frame in ipairs(fold_frames) do
				command.print_fold_frame_description(
					fold, frame, index, args)
			end
		elseif args.frame and #args.frame ~= 0 then
			for _, index in ipairs(args.frame) do
				local frame = fold_frames[index + 1]
				if frame ~= nil then
					command.print_fold_frame_description(
						fold, frame, index + 1, args)
				else
					print(("%s%sERROR%s: %sframe %s%s%s%s is not within %s0-%s%s%s range%s")
						:format(BOLD, RED, RESET, RED, BOLD, index, RESET, RED,
							BOLD, #fold_frames - 1, RESET,
							RED, RESET))
				end
			end
		end
	else
		local fold_frames = fold:get_frames()
		if args.all then
			for index = #fold_frames, 1, -1 do
				local frame = fold_frames[index]
				command.print_fold_frame_description(
					fold, frame, index, args)
			end
		elseif args.frame and #args.frame ~= 0 then
			for i = #args.frame, 1, -1 do
				local index = args.frame[i]
				local frame = fold_frames[index + 1]
				if frame ~= nil then
					command.print_fold_frame_description(
						fold, frame, index + 1, args)
				else
					print(("%s%sERROR%s: %sframe %s%s%s%s is not within %s0-%s%s%s range%s")
						:format(BOLD, RED, RESET, RED, BOLD, index, RESET, RED,
							BOLD, #fold_frames - 1, RESET,
							RED, RESET))
				end
			end
		end
		command.print_fold_description(fold, args)
	end

	return fold, fold_error
end

return command
