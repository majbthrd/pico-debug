include ../tinyusb/tools/top.mk
include ../tinyusb/examples/make.mk

INC += \
	. \
	$(TOP)/hw \

APP_SOURCE += $(wildcard ./*.c)
SRC_C += $(addprefix $(CURRENT_PATH)/, $(APP_SOURCE))

include ../tinyusb/examples/rules.mk
