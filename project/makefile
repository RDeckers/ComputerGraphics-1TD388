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
	LIB_SUFFIX =.dll
	EXE_SUFFIX = .exe
else
	REPORT = @echo -e "\e[4;1;37m$1\033[0m"
	CHK_DIR_EXISTS = test -d $1 || mkdir -p $1
	NUKE = rm -r $1
	COPY_DIR = cp -rv $1 $2
	FIX_PATH = $1
	INSTALL_LIB_DIR := ~/lib/
	INSTALL_BIN_DIR := ~/bin/
	INSTALL_INCLUDE_DIR := ~/include/
	LIB_SUFFIX :=.so
	EXE_SUFFIX :=
endif

PROJECT_DIR :=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OBJ_DIR := $(PROJECT_DIR)obj
BIN_DIR := $(PROJECT_DIR)bin
SRC_DIR := $(PROJECT_DIR)/src
HEADERS_DIR := $(PROJECT_DIR)/headers

CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)

OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_FILES))

ifeq ($(OS),Windows_NT)
	LD_FLAGS +=  -lmingw32 -lglew32 -lopengl32
else
	LD_FLAGS += -lGLEW -lGL
endif

LD_FLAGS +=  -L$(INSTALL_LIB_DIR) -lglu -lSDL2main -lSDL2 -lutilities -lm -lgameframe -lstdc++
C_FLAGS += --std=c++14 -O2 -pipe -march=native -I$(PROJECT_DIR)headers -I$(INSTALL_INCLUDE_DIR)

BINARIES = main
COMMON_OBJ_FILES = $(filter-out $(addsuffix .o, $(addprefix $(OBJ_DIR)/, $(BINARIES))), $(OBJ_FILES))
BINARIES_FULL = $(addprefix $(BIN_DIR)/, $(addsuffix $(EXE_SUFFIX), $(BINARIES)))

.SECONDARY: $(OBJ_FILES)

all : $(BINARIES_FULL) $(COMMON_OBJ_FILES)

$(BIN_DIR)%$(EXE_SUFFIX) : $(OBJ_DIR)%.o $(COMMON_OBJ_FILES)
	$(call REPORT,linking $@)
	$(call CHK_DIR_EXISTS, $(dir $@))
	g++ -o $@ $< $(COMMON_OBJ_FILES) $(LD_FLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS_DIR)/%.hpp
	$(call REPORT,Compiling $@)
	$(call CHK_DIR_EXISTS, $(dir $@))
	g++ $(C_FLAGS) -o "$@" -c "$<"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(call REPORT,Compiling $@)
	$(call CHK_DIR_EXISTS, $(dir $@))
	g++ $(C_FLAGS) -o "$@" -c "$<"

clean:
	$(call REPORT,Cleaning...)
	-$(NUKE) "$(OBJ_DIR)" "$(BIN_DIR)"
