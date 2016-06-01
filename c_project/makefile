rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *, %,$2),$d))
directories=$(sort $(dir $(wildcard $/*)))
ifeq ($(OS),Windows_NT)
FIX_PATH = $(subst /,\,$1)
	REPORT = @echo $1
	CHK_DIR_EXISTS = if not exist "$(strip $1)" mkdir "$(strip $1)"
	NUKE = rmdir /s /q
	COPY_DIR = xcopy $(call FIX_PATH,$1 $2) /E /H /Y
	COPY_CONTENT = xcopy /s /Y $(call FIX_PATH,$1 $2)
	COPY = xcopy $(call FIX_PATH,$1 $2) /Y
	INSTALL_LIB_DIR := Z:/lib/
	INSTALL_BIN_DIR := Z:/bin/
	INSTALL_INCLUDE_DIR := Z:/include/
	EXE_SUFFIX = .exe
	LIB_SUFFIX =.dll
else
	REPORT = @echo -e "\e[4;1;37m$1\033[0m"
	CHK_DIR_EXISTS = test -d $1 || mkdir -p $1
	NUKE = rm -r $1
	COPY_DIR = cp -rv $1 $2
	FIX_PATH = $1
	INSTALL_LIB_DIR := ~/lib/
	INSTALL_BIN_DIR := ~/bin/
	INSTALL_INCLUDE_DIR := ~/include/
	EXE_SUFFIX =
	LIB_SUFFIX :=.so
endif

PROJECT_DIR :=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OBJ_DIR := $(PROJECT_DIR)obj
BIN_DIR := $(PROJECT_DIR)bin
SRC_DIR := $(PROJECT_DIR)/src

C_FILES := $(wildcard $(SRC_DIR)/*.c)
SCRIPTS := $(wildcard $(SCRIPT_DIR)/*.sh)
BINARIES := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%$(EXE_SUFFIX),$(C_FILES))

COMMON_C_FILES := $(wildcard $(SRC_DIR)/common/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/common/%.c,$(OBJ_DIR)/%.o,$(COMMON_C_FILES))

ifeq ($(OS),Windows_NT)
	LD_FLAGS +=  -lmingw32 -lglew32 -lopengl32
else
	LD_FLAGS += -lGLEW -lGL
endif

LD_FLAGS += -lutilities -lglu -lSDL2main -lSDL2 -L$(INSTALL_LIB_DIR)
C_FLAGS += --std=gnu99 -march=native -Ofast -flto -pipe -g -I$(PROJECT_DIR)headers -I$(INSTALL_INCLUDE_DIR)


.PRECIOUS: $(OBJ_FILES) $(DATA_FILES)

all: binaries

binaries: $(BINARIES)

$(BIN_DIR)%$(EXE_SUFFIX) : $(SRC_DIR)/%.c $(OBJ_FILES)
	$(call REPORT,Building $@)
	$(call CHK_DIR_EXISTS, $(dir $@))
	gcc $(C_FLAGS) -o "$@" "$<" $(OBJ_FILES) $(LD_FLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/common/%.c
	$(call REPORT,Compiling $@)
	$(call CHK_DIR_EXISTS, $(dir $@))
	@gcc $(C_FLAGS) -o "$@" -c "$<"

clean:
	$(call REPORT,Cleaning...)
	-$(NUKE) "$(OBJ_DIR)" "$(BIN_DIR)"
