TARGET = paper-folder
SOURCES_DIRECTORY = sources
THIRDPARTY_DIRECTORY = thirdparty
EXE = $(if $(COMSPEC),.exe,)

CC = gcc
CFLAGS = -std=c11 \
	-I. -I$(SOURCES_DIRECTORY) \
	-I$(THIRDPARTY_DIRECTORY)/xoshiro \
	-I$(THIRDPARTY_DIRECTORY)/lua \
	-I$(THIRDPARTY_DIRECTORY)/yyjson \
	-I$(THIRDPARTY_DIRECTORY)/nanosvg \
	-I$(THIRDPARTY_DIRECTORY)/tinyobjloader \
	-Wall -Wextra
#CFLAGS += -DNDEBUG
CFLAGS += -O2

LDFLAGS =
LDLIBS = -lm

LUA_MODULES_DIRECTORY = $(SOURCES_DIRECTORY)/lua_modules
LUA_MODULES_HEADER = $(SOURCES_DIRECTORY)/lua_modules.h
LUA_MODULES_CODE = $(SOURCES_DIRECTORY)/lua_modules.c

LUA_SCRIPTS_DIRECTORY = $(SOURCES_DIRECTORY)/lua_scripts
LUA_SCRIPTS_HEADER = $(SOURCES_DIRECTORY)/lua_scripts.h
LUA_SCRIPTS_CODE = $(SOURCES_DIRECTORY)/lua_scripts.c

LUA_MODULES = $(wildcard \
	$(LUA_MODULES_DIRECTORY)/*.h \
	$(LUA_MODULES_DIRECTORY)/*/*.h \
	$(LUA_MODULES_DIRECTORY)/*/*/*.h \
	$(LUA_MODULES_DIRECTORY)/*/*/*/*.h)

LUA_SCRIPTS = $(wildcard \
	$(LUA_SCRIPTS_DIRECTORY)/*.lua \
	$(LUA_SCRIPTS_DIRECTORY)/*/*.lua \
	$(LUA_SCRIPTS_DIRECTORY)/*/*/*.lua \
	$(LUA_SCRIPTS_DIRECTORY)/*/*/*/*.lua)

C_SOURCES_ALL = $(wildcard \
	$(SOURCES_DIRECTORY)/*.c \
	$(SOURCES_DIRECTORY)/*/*.c \
	$(SOURCES_DIRECTORY)/*/*/*.c \
	$(SOURCES_DIRECTORY)/*/*/*/*.c)

C_SOURCES_ALL += $(wildcard \
	$(THIRDPARTY_DIRECTORY)/*.c \
	$(THIRDPARTY_DIRECTORY)/*/*.c \
	$(THIRDPARTY_DIRECTORY)/*/*/*.c \
	$(THIRDPARTY_DIRECTORY)/*/*/*/*.c)

C_SOURCES = $(filter-out \
	$(THIRDPARTY_DIRECTORY)/xoshiro/xoshiro256starstar.c \
	$(THIRDPARTY_DIRECTORY)/xoshiro/f2x.c \
	$(THIRDPARTY_DIRECTORY)/lua/lua.c \
	$(THIRDPARTY_DIRECTORY)/lua/luac.c \
	$(LUA_MODULES_CODE) \
	$(LUA_SCRIPTS_CODE), \
	$(C_SOURCES_ALL))

C_OBJECTS = $(C_SOURCES:.c=.o)

.PHONY: all make_lua lua_compile clean

all: make_lua lua_compile $(TARGET)$(EXE)

make_lua:
	@$(MAKE) -C $(THIRDPARTY_DIRECTORY)/lua CC='$(CC)' \
		--no-print-directory >/dev/null

lua_compile:
	@$(THIRDPARTY_DIRECTORY)/lua/lua$(EXE) compile.lua \
		'$(LUA_MODULES_DIRECTORY)' \
		'$(LUA_MODULES_HEADER)' \
		'$(LUA_MODULES_CODE)' \
		'$(LUA_MODULES)' \
		'$(LUA_SCRIPTS_DIRECTORY)' \
		'$(LUA_SCRIPTS_HEADER)' \
		'$(LUA_SCRIPTS_CODE)' \
		'$(LUA_SCRIPTS)'

$(TARGET)$(EXE): $(C_OBJECTS) $(LUA_MODULES_CODE) $(LUA_SCRIPTS_CODE)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(C_OBJECTS) \
		$(LUA_MODULES_CODE) $(LUA_SCRIPTS_CODE) \
		-o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(MAKE) -C $(THIRDPARTY_DIRECTORY)/lua clean -s --no-print-directory
	@$(if $(COMSPEC), \
		del /Q /F $(subst /,\\,$(C_OBJECTS)) $(TARGET)$(EXE) \
			$(subst /,\\,$(LUA_MODULES_HEADER)) \
			$(subst /,\\,$(LUA_MODULES_CODE)) \
			$(subst /,\\,$(LUA_SCRIPTS_HEADER)) \
			$(subst /,\\,$(LUA_SCRIPTS_CODE)) 2>nul, \
		rm -f $(C_OBJECTS) $(TARGET)$(EXE) \
			$(LUA_MODULES_HEADER) \
			$(LUA_MODULES_CODE) \
			$(LUA_SCRIPTS_HEADER) \
			$(LUA_SCRIPTS_CODE))
