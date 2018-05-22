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

export CPP := clang++
export CC := clang
export AR := ar
export MAKE := make -s
export HOME := $(abspath ../../../)
export FRAMEWORK := $(abspath $(HOME)/EasyCpp)
export PACKAGES := $(abspath $(HOME)/Packages)
export TEMPLATE := $(abspath $(HOME)/EasyCpp/Make/Linux/Template.mk)
export NULL := /dev/null
ifneq '$(PREFIX)' ''
export OUT := $(abspath $(PREFIX)/out)
else
export OUT := $(abspath $(HOME)/out)
endif

export NO_COLOR = \033[0m
export RED      = \033[91m
export GREEN    = \033[92m
export YELLOW   = \033[93m
export BLUE     = \033[94m
export PURPLE   = \033[95m
export CYAN     = \033[96m
export WHITE    = \033[97m

export FLAGS :=  \
  -Wall \
  -Wextra \
  -Wno-long-long \
  -Wno-variadic-macros \
  -g \
  -O2 \
  -fPIC \
  -fstack-check \
  -std=c++14 \
  -I $(FRAMEWORK)/Inc \
  -I $(FRAMEWORK)/Inc/Platform/Linux \
  -DDEFAULT_MAX_CLIENT=32 \
  -DDEFAULT_SOCKET_PATH=\"/tmp/\" \
  -DENABLE_TRACE_ERROR \
  -DENABLE_TRACE_INFO

export LD_FLAGS = \
  -Wl,-export-dynamic \
  -lpthread \
  -lrt \
  -ldl

# Common packages
-include $(PACKAGES)/*.mk

# Interfaces
-include $(PACKAGES)/Interface/*.mk

# Platform related packages
-include $(PACKAGES)/Platform/Linux/*.mk

.PHONY: EasyCpp
EasyCpp:
	@$(MAKE) -f EasyCpp/Makefile all

.PHONY: Test
Test: TEST_CASES=$(shell make -pn | grep "^\w*.Test:" | awk -F ':' '{print $$1}')
Test:
	@$(MAKE) $(TEST_CASES)

.PHONY: clean
clean:
	@rm $(OUT) -rf

