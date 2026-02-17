ifeq($(filter debug,$(MAKECMDGOALS)),debug)
	Q = @
else
	Q =

INCDIRS=./devlib/include
#all possible source directories for the .c files
SRCDIRS=. ./devlib ./devlib/Src

# all .c files
CFILES=$(foreach folders,$(SRCDIRS),$(wildcard $folders/*.c))


# --- TARGETS ---
EXECUTABLES=$(foreach f,calculator.exe bin/calculator_Core.exe,user/$(f))
TARGETDIRS=lib bin

# setting .dll source and target directories
FOLDERSRC=./devlib/Src

define getDLLS
	$(patsubst $(FOLDERSRC)/Src$(1)/%.c,user/$(1)/%.dll,$(wildcard $(FOLDERSRC)/Src$(1)/*.c))
endef

DLLDIRS_LIB=$(call getDLLS,lib)
DLLDIRS_BIN=$(call getDLLS,bin)

ALL_DLLS=$(DLLDIRS_LIB) $(DLLDIRS_BIN)

WARNINGFLAGS=-Wall -Wextra
CFLAGS=$(foreach dir,$(INCDIRS),-I$(dir)) $(WARNINGFLAGS) -L./devlib
HCHECK=-MD -MP -MF devlib/Dfiles/$(basename $(@F)).d

Q = @ # for silent command execution


DLLSETTINGS=-shared -Wl,--out-implib=devlib/lib

# --- Lib depencity ---
DLLNAME = CustomErrorcheck CustomMath corelogic
define get_dep
	$(foreach idx,$(1),$(word $(idx),$(DLLNAME)))
endef
DEP_calculator_Core =$(call get_dep,1 2 3)
DEP_CustomMath =$(call get_dep,1)
DEP_corelogic =$(call get_dep,1 2)

DEPLIBS =$(foreach dll,$(DEP_$(basename $(@F))),-l$(dll))








# --- RULES ---
all : $(EXECUTABLES)

debug : all
	@echo "Debug mode: showing all commands during compilation"

# buiding the launcher
$(word 1,$(EXECUTABLES)) : launcher.c
	@echo Compiling $@ with dependencies
	$(Q)gcc $(WARNINGFLAGS) $(HCHECK) -o $@ $<

# building the logic for calculator
$(word 2,$(EXECUTABLES)) : main.c $(ALL_DLLS)
	@echo Compiling $@ with dependencies...
	@echo "Dependencies: $(DEP_$(basename $(@F)))"
	$(Q)gcc $(CFLAGS) $(HCHECK) $< -o $@ $(DEPLIBS)

#general DLL building template
#gets all folder in TARGETDIRS and put corresponding .dll to user/indicated folder/...
define getDLLS_fromNAME
	$(foreach dll,$(1),$(filter %/$(dll).dll,$(ALL_DLLS)))
endef


define DLL_TEMPLATE
user/$(1)/%.dll : $(FOLDERSRC)/Src$(1)/%.c $$(call getDLLS_fromNAME,DEP_$$(basename $$(not dir %.dll)))
	@echo buiding dll: $$(@F)
	$(Q)gcc $$< $(DLLSETTINGS)$$(*F).lib $$(HCHECK) $(CFLAGS) $$(DEPLIBS) -o $$@ 
endef

$(foreach subfolder,$(TARGETDIRS),$(eval $(call DLL_TEMPLATE,$(subfolder))))

clean : 
	@echo Cleaning up...
	@echo "| Removing exe..."
	@rm -f user/*.exe $(foreach subfolder,$(TARGETDIRS),user/$(subfolder)/$(EXECUTABLES))
	@echo "| Removing dll..."
	@rm -f $(ALL_DLLS)
	@echo "| Removing .d files..."
	@rm -f $(wildcard devlib/Dfiles/*.d)
	@echo "| removing .lib files..."
	@rm -f ./devlib/*.lib
	@echo "cleaned up successfully!"

-include $(wildcard devlib/Dfiles/*.d)