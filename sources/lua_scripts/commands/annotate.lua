local command = { name = "annotate", group = "Inspection Commands" }
local from_fold = require("commands.from.fold")
local to_fold = require("commands.to.fold")
local _FoldFrame_ = require("fold.frame_C")

local COLORS = require("colors")
local BOLD = COLORS.BOLD
local RED = COLORS.RED
local RESET = COLORS.RESET

function command.register(parser)
	local annotate = parser:command(command.name)
		:description("annotate FOLD file")

	annotate:argument("input"):args("?")
		:description("path to the FOLD source")
	annotate:option("-o --output"):argname("<file>")
		:description("path to the FOLD destination")

	annotate:option("-f --frame"):argname("<int>")
		:description("FOLD frames (multiple)")
		:convert(tonumber)
		:count("*")
		:target("frames")

	annotate:group("File Options",
		annotate:option("-V --file_version"):argname("<float>")
		:description("set file version")
		:convert(tonumber)
		:default("1.2")
		:show_default(true),

		annotate:option("-C --file_creator"):argname("<string>")
		:description("set file creator"),

		annotate:option("-A --file_author"):argname("<string>")
		:description("set file author"),

		annotate:option("-T --file_title"):argname("<string>")
		:description("set file title"),

		annotate:option("-D --file_description"):argname("<string>")
		:description("set file description"),

		annotate:option("--file_class"):argname("<string>")
		:description("set file classes")
		:count("*")
		:target("file_classes")
	)

	annotate:group("Frame Options",
		annotate:option("-a --frame_author"):argname("<string>")
		:description("set frames author"),

		annotate:option("-t --frame_title"):argname("<string>")
		:description("set frames title"),

		annotate:option("-d --frame_description"):argname("<string>")
		:description("set frames description"),

		annotate:option("--frame_class"):argname("<string>")
		:description("set frames classes")
		:count("*")
		:target("frame_classes"),

		annotate:option("--frame_attribute"):argname("<string>")
		:description("set frames attributes")
		:count("*")
		:target("frame_attributes"),

		annotate:option("-u --frame_unit"):argname("<string>")
		:description("set frames unit")
	)

	return annotate
end

function command.annotate_fold(fold, args)
	if type(args.file_version) == "number" then
		fold:set_version(args.file_version)
	end

	if type(args.file_creator) == "string" then
		local set, set_error = fold:set_creator(args.file_creator)
		if set == false then error(set_error, 0) end
	end

	if type(args.file_author) == "string" then
		local set, set_error = fold:set_author(args.file_author)
		if set == false then error(set_error, 0) end
	end

	if type(args.file_title) == "string" then
		local set, set_error = fold:set_title(args.file_title)
		if set == false then error(set_error, 0) end
	end

	if type(args.file_description) == "string" then
		local set, set_error = fold:set_description(args.file_description)
		if set == false then error(set_error, 0) end
	end

	if type(args.file_classes) == "table" then
		local set, set_error = fold:set_classes(args.file_classes)
		if set == false then error(set_error, 0) end
	end
end

function command.annotate_fold_frame(fold, frame, index, args)
	if type(args.frame_author) == "string" then
		local set, set_error = frame:set_author(args.frame_author)
		if set == false then error(set_error, 0) end
	end

	if type(args.frame_title) == "string" then
		local set, set_error = frame:set_title(args.frame_title)
		if set == false then error(set_error, 0) end
	end

	if type(args.frame_description) == "string" then
		local set, set_error = frame:set_description(args.frame_description)
		if set == false then error(set_error, 0) end
	end

	if type(args.frame_classes) == "table" then
		local set, set_error = frame:set_classes(args.frame_classes)
		if set == false then error(set_error, 0) end
	end

	if type(args.frame_attributes) == "table" then
		local set, set_error = frame:set_attributes(args.frame_attributes)
		if set == false then error(set_error, 0) end
	end

	if type(args.frame_unit) == "string" then
		local set, set_error = frame:set_unit(args.frame_unit)
		if set == false then error(set_error, 0) end
	end
end

function command.execute(args)
	local fold, fold_error =
		from_fold.fold_from_input_args(args)

	command.annotate_fold(fold, args)
	if args.frames and #args.frames ~= 0 then
		local fold_frames = fold:get_frames()

		local has_all_frames = true
		for _, index in ipairs(args.frames) do
			local frame = fold_frames[index + 1]
			if frame == nil then
				has_all_frames = false
				print(("%s%sERROR%s: %sframe %s is not within 0-%s range%s")
					:format(BOLD, RED, RESET, RED, index,
						#fold_frames - 1, RESET))
			end
		end

		if not has_all_frames then
			return fold, fold_error
		end

		for _, index in ipairs(args.frames) do
			local frame = fold_frames[index + 1]
			command.annotate_fold_frame(fold, frame, index, args)
		end
	end

	to_fold.fold_to_fold_file(fold, args)
	return fold, fold_error
end

return command
