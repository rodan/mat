
PROJ:=fexec
SUBDIRS:= 
OBJ_DIR:=build

################### Linux or Mac OS ####################
################## GCC Root Variable ###################
GCC_DIR     ?= /usr/bin
GCC_INC_DIR ?= /usr/include
DRIVERLIB   := ../../../os-libs
RM          := rm -f

######################################
GCC_BIN_DIR      ?= /usr/local/bin
GCC_INC_DIR      ?= /usr/include
######################################
CC               := $(GCC_BIN_DIR)/gcc
GDB	             := $(GCC_BIN_DIR)/gdb
######################################

# -fdata-sections, -ffunction-sections and -Wl,--gc-sections -Wl,-s
# are used for dead code elimination, see:
# http://gcc.gnu.org/ml/gcc-help/2003-08/msg00128.html

CFLAGS           += -Wall -Wno-switch
CFLAGS		     += -fno-force-addr -finline-limit=1 -fno-schedule-insns
CFLAGS		     += -fshort-enums -Wl,-Map=output.map
CFLAGS_REL	     += -Os -fdata-sections -ffunction-sections -fomit-frame-pointer
LDFLAGS_REL	     += -Wl,-s
CFLAGS_DBG	     += -Og -ggdb
LDFLAGS_DBG	     += 
INCLUDES         := -I $(GCC_INC_DIR) -I $(DRIVERLIB) -I ./ -I .
######################################
#DRIVERLIB_SRC    := $(wildcard $(DRIVERLIB)/*.c)
DRIVERLIB_SRC    := $(DRIVERLIB)/error_functions.c
LOCAL_SRC        := $(wildcard *.c)
PROJ_SRC         := main.c
SRC              := $(PROJ_SRC) $(DRIVERLIB_SRC) $(LOCAL_SRC)

DRIVERLIB_OBJECTS = $(DRIVERLIB_SRC:$(DRIVERLIB)/%.c=$(OBJ_DIR)/%.o)
LOCAL_OBJECTS     = $(LOCAL_SRC:%.c=$(OBJ_DIR)/%.o)

#DRIVERLIB_OBJECTS:= $(patsubst %.c,%.o,$(DRIVERLIB_SRC))
#LOCAL_OBJECTS    := $(patsubst %.c,%.o,$(LOCAL_SRC))
EXOBJECT         := $(OBJ_DIR)/$(PROJ).o
EXOUTPUT         := $(OBJ_DIR)/$(PROJ)
######################################

# Append specific CFLAGS/LDFLAGS
DEBUG := $(shell grep "^\#define CONFIG_DEBUG" config.h)
ifeq ($(DEBUG),)
TARGET	:= RELEASE
CFLAGS	+= $(CFLAGS_REL)
LDFLAGS	+= $(LDFLAGS_REL)
else
TARGET	:= DEBUG
CFLAGS	+= $(CFLAGS_DBG)
LDFLAGS	+= $(LDFLAGS_DBG)
endif

.PHONY: all
.PHONY: clean
.PHONY: install
.PHONY: config
.PHONY: mkdir_od
.PHONY: depend
.PHONY: driverlib
.PHONY: doc
.PHONY: cflags_update
.PHONY: size

all: ename.c.inc depend config.h driverlib $(EXOUTPUT) size

config.h:
	@echo "config.h file is missing" && false

$(OBJ_DIR)/%.o: $(DRIVERLIB)/%.c
	@echo -e " * [drv] $(@F)"
	@$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c
	@echo -e " * $(@F)"
	@$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(EXOUTPUT): $(DRIVERLIB_OBJECTS) $(LOCAL_OBJECTS) $(EXOBJECT)
	@echo -e "\n>> Building $@ as target $(TARGET)"
	@$(CC) $(LDFLAGS) $(DRIVERLIB_OBJECTS) $(LOCAL_OBJECTS) -o $@

mkdir_od:
	@mkdir -p $(OBJ_DIR)

clean:
	$(RM) build/*
	$(RM) tags

# rebuild if CFLAGS changed, as suggested in:
# http://stackoverflow.com/questions/3236145/force-gnu-make-to-rebuild-objects-affected-by-compiler-definition/3237349#3237349
$(OBJ_DIR)/$(PROJ).cflags: cflags_update
	@echo "$(CFLAGS)" | cmp -s - $@ || echo "$(CFLAGS)" > $@

ename.c.inc:
	@../../../tools/build_ename.sh > ename.c.inc

$(OBJ_DIR)/$(PROJ).dep: $(SRC)
	@echo "Generating dependencies.."
	@mkdir -p $(OBJ_DIR)
	@touch $@
	@makedepend $(INCLUDES) -p "$(OBJ_DIR)/" -Y -f $@ $^ &> /dev/null
	@sed -i "s|build/$(DRIVERLIB)/|build/|" $(OBJ_DIR)/$(PROJ).dep
	@grep -v "^\#" $(OBJ_DIR)/$(PROJ).dep | cut -d" " -f2 | sort -u | xargs > $(OBJ_DIR)/$(PROJ).hdep
	@echo "tags: `cat $(OBJ_DIR)/$(PROJ).hdep`" >> $(OBJ_DIR)/$(PROJ).dep
	@rm -f $@.bak

size: $(OBJ_DIR)/$(PROJ)
	@size $(OBJ_DIR)/$(PROJ)

debug: all
	$(GDB) $(OBJ_DIR)/$(PROJ)

$(EXOBJECT): $(PROJ_SRC) $(OBJ_DIR)/$(PROJ).cflags
	@$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

-include $(OBJ_DIR)/$(PROJ).dep

