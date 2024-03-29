##### global settings #####

.PHONY: nemu entry testcase kernel run gdb test submit clean count

CC := gcc
LD := ld
CFLAGS := -MMD -Wall -Werror -c

LIB_COMMON_DIR := lib-common
LIBC_INC_DIR := $(LIB_COMMON_DIR)/uclibc/include
LIBC_LIB_DIR := $(LIB_COMMON_DIR)/uclibc/lib
LIBC := $(LIBC_LIB_DIR)/libc.a
FLOAT := obj/$(LIB_COMMON_DIR)/FLOAT/FLOAT.a

include config/Makefile.git
include config/Makefile.build

all: nemu


##### rules for building the project #####

include nemu/Makefile.part
include testcase/Makefile.part
include lib-common/FLOAT/Makefile.part
include kernel/Makefile.part
include game/Makefile.part

nemu: $(nemu_BIN)
testcase: $(testcase_BIN)
kernel: $(kernel_BIN)
game: $(game_BIN)


##### rules for cleaning the project #####

clean-nemu:
	-rm -rf obj/nemu 2> /dev/null

clean-testcase:
	-rm -rf obj/testcase 2> /dev/null

clean-kernel:
	-rm -rf obj/kernel 2> /dev/null

clean-game:
	-rm -rf obj/game 2> /dev/null

clean: clean-cpp
	-rm -rf obj 2> /dev/null
	-rm -f *log.txt entry $(FLOAT) 2> /dev/null

##### some convinient rules #####

USERPROG = obj/testcase/Agri-net
ENTRY := $(USERPROG)
COUNT_NEMU_Lines := $(shell cat `find nemu -name '*.[ch]'` | grep -Ev "^$$" | wc -l)
COUNT_NEMU_Lines_ADD := $(shell expr $(COUNT_NEMU_Lines) - 2973)
COUNT_NEMU_ALLLines := $(shell cat `find nemu -name '*.[ch]'` | wc -l)
COUNT_NEMU_ALLLines_ADD := $(shell expr $(COUNT_NEMU_ALLLines) - 3736)

entry: $(ENTRY)
	objcopy -S -O binary $(ENTRY) entry

run: $(nemu_BIN) $(USERPROG) entry
	$(call git_commit, "run")
	$(nemu_BIN) $(USERPROG)

gdb: $(nemu_BIN) $(USERPROG) entry
	$(call git_commit, "gdb")
	gdb -s $(nemu_BIN) --args $(nemu_BIN) $(USERPROG)

test: $(nemu_BIN) $(testcase_BIN) entry
	$(call git_commit, "test")
	bash test.sh $(testcase_BIN)

submit: clean
	cd .. && zip -r $(STU_ID).zip $(shell pwd | grep -o '[^/]*$$')

##### rules for counting lines of .c/.h in nemu #####
count:
	@echo There are $(COUNT_NEMU_Lines) lines of code in nemu of this branch except empty line
	@echo There are $(COUNT_NEMU_Lines_ADD) lines added into the frame code

countall:
	@echo There are $(COUNT_NEMU_ALLLines) lines of code in nemu of this branch
	@echo There are $(COUNT_NEMU_ALLLines_ADD) lines added into the frame code
	

