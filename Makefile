# =========================================================================== #
# Configuration                                                               #
# =========================================================================== #

CC = gcc
TARGET = paper-folder
EXE = $(if $(COMSPEC),.exe,)

CFLAGS = -std=c11
CFLAGS += -DREAL_TYPE=double
CFLAGS += -Wall -Wextra
#CFLAGS += -DNDEBUG -s
CFLAGS += -O2

# =========================================================================== #
# Dependencies                                                                #
# =========================================================================== #

SOURCES_DIRECTORY = sources
CFLAGS += -I. -I$(SOURCES_DIRECTORY)

THIRDPARTY_DIRECTORY = thirdparty
CFLAGS += \
	-I$(THIRDPARTY_DIRECTORY)/lua \
	-I$(THIRDPARTY_DIRECTORY)/yyjson \
	-I$(THIRDPARTY_DIRECTORY)/nanosvg \
	-I$(THIRDPARTY_DIRECTORY)/triangle \
	-I$(THIRDPARTY_DIRECTORY)/tiny_obj \
	-I$(THIRDPARTY_DIRECTORY)/xoshiro

LDFLAGS =
LDLIBS = -lm

ifeq ($(EXE),)
$(THIRDPARTY_DIRECTORY)/lua/%.o: \
	CFLAGS += -DLUA_USE_POSIX
endif

# WARNING: Triangle library is designed for double precision
$(THIRDPARTY_DIRECTORY)/triangle/triangle.o: \
	CFLAGS += -DTRILIBRARY -DREAL=double -DANSI_DECLARATORS -DNO_TIMER -w

$(THIRDPARTY_DIRECTORY)/tiny_obj/%.o: \
	CFLAGS += -std=c11 -w

# =========================================================================== #
# Lua Embedding                                                               #
# =========================================================================== #

LUA = $(THIRDPARTY_DIRECTORY)/lua/lua$(EXE)

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

LUA_REQUIRED = compile.lua \
	$(LUA_MODULES) $(LUA_SCRIPTS)

LUA_GENERATED = \
	$(LUA_MODULES_HEADER) \
	$(LUA_MODULES_CODE) \
	$(LUA_SCRIPTS_HEADER) \
	$(LUA_SCRIPTS_CODE)

# =========================================================================== #
# Sources                                                                     #
# =========================================================================== #

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

C_SOURCES_ALL_WITHOUT_LUA = $(filter-out \
	$(THIRDPARTY_DIRECTORY)/lua/lua.c \
	$(THIRDPARTY_DIRECTORY)/lua/luac.c \
	$(LUA_MODULES_CODE) \
	$(LUA_SCRIPTS_CODE), \
	$(C_SOURCES_ALL))

C_SOURCES_ALL_WITHOUT_XOSHIRO = $(filter-out \
	$(THIRDPARTY_DIRECTORY)/xoshiro/%, \
	$(C_SOURCES_ALL_WITHOUT_LUA))

C_SOURCES = $(C_SOURCES_ALL_WITHOUT_XOSHIRO)
C_OBJECTS = $(C_SOURCES:.c=.o)

# =========================================================================== #
# Compiling                                                                   #
# =========================================================================== #

.PHONY: all clean

all: $(TARGET)$(EXE)

$(LUA):
	@$(MAKE) -C $(THIRDPARTY_DIRECTORY)/lua CC='$(CC)' --silent

$(LUA_GENERATED): $(LUA) $(LUA_REQUIRED)
	@$(THIRDPARTY_DIRECTORY)/lua/lua$(EXE) compile.lua \
		'$(LUA_MODULES_DIRECTORY)' \
		'$(LUA_MODULES_HEADER)' \
		'$(LUA_MODULES_CODE)' \
		'$(LUA_MODULES)' \
		'$(LUA_SCRIPTS_DIRECTORY)' \
		'$(LUA_SCRIPTS_HEADER)' \
		'$(LUA_SCRIPTS_CODE)' \
		'$(LUA_SCRIPTS)'

$(C_OBJECTS): | \
	$(LUA_MODULES_HEADER) \
	$(LUA_SCRIPTS_HEADER)

$(TARGET)$(EXE): $(C_OBJECTS) $(LUA_MODULES_CODE) $(LUA_SCRIPTS_CODE)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(C_OBJECTS) \
		$(LUA_MODULES_CODE) $(LUA_SCRIPTS_CODE) \
		-o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@$(MAKE) -C $(THIRDPARTY_DIRECTORY)/lua clean --silent
	@rm -f $(C_OBJECTS) $(LUA_GENERATED) $(TARGET)$(EXE)
	@rm -rf "test-output"
