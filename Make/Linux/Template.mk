# Copyright (c) 2018 Guo Xiang
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

STATIC_LIBS := $(addprefix $(OUT)/,$(addsuffix .a,$(SLIBS)))
DYNAMIC_LIBS := $(addprefix -l,$(DLIBS))
_SRC := $(addprefix $(PKG_PATH)/,$(SRC))

DEPS := $(abspath $(addprefix $(OUT)/, $(addsuffix .d, $(basename $(_SRC)))))
OBJS := $(abspath $(addprefix $(OUT)/, $(addsuffix .o, $(basename $(_SRC)))))

LIBS := $(OBJS) $(STATIC_LIBS) -L$(OUT) $(DYNAMIC_LIBS)

.PHONY: all
ifeq '$(I_AM_APP)' ''
all: $(DEPS) $(OBJS)
	@echo "$(YELLOW)Creating library$(NO_COLOR): $(PKG_NAME)"
	$(PRE_COMPILE_CMD)
	@$(AR) -r $(OUT)/$(PKG_NAME).a $(OBJS) $(STATIC_LIBS) > $(NULL) 2>&1
	@$(CPP) -shared -o $(OUT)/lib$(PKG_NAME).so $(LIBS)
	$(POST_COMPILE_CMD)
else
all: $(DEPS) $(OBJS)
	@echo "$(GREEN)Creating executable$(NO_COLOR): $(PKG_NAME)"
	$(PRE_COMPILE_CMD)
	@$(CPP) $(LD_FLAGS) -Wl,-Map,$(OUT)/$(PKG_NAME).map -o $(OUT)/$(PKG_NAME) $(LIBS)
	$(POST_COMPILE_CMD)
endif

sinclude $(DEPS)

# To generate deps
$(DEPS): $(abspath $(OUT))/%.d: $(HOME)/%.cpp
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
		$(CPP) $< $(FLAGS) -MM > $@.$$$$; \
		sed 's,\(^.*\)\.o[ :]*,$@ $(abspath $(OUT)/$(dir $<))/\1.o : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

# To generate objs
$(OBJS): $(abspath $(OUT))/%.o: $(HOME)/%.cpp
	@mkdir -p $(dir $@)
	@echo "$(CYAN)Compiling$(NO_COLOR): $<"
	@$(CPP) -c $(FLAGS) $< -o $@

