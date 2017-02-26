# adapted from http://www.throwtheswitch.org/build/make

MKDIR=mkdir -p
TARGET_EXTENSION=out

PATHU = Unity/src/
PATHS = src/
PATHT = test/
PATHB = build/
PATHO = build/objs/
PATHR = build/results/

BUILD_PATHS = $(PATHB) $(PATHD) $(PATHO) $(PATHR)

SRCT = $(wildcard $(PATHT)*.c)

COMPILE=gcc -c -coverage
LINK=gcc -coverage
DEPEND=gcc -MM -MG -MF
CFLAGS=-Wall -Wno-unused-function -Werror -std=c89 -I. -I$(PATHU) -I$(PATHS) -DTEST -DDEBUG


# ============================== TARGETS ==================================

.PHONY: all
.PHONY: clean
.PHONY: test

all: $(BUILD_PATHS) $(PATHO)Hsmfui.o

# keep some files around after build

.PRECIOUS: $(PATHB)Test%.$(TARGET_EXTENSION)
.PRECIOUS: $(PATHO)%.o
.PRECIOUS: $(PATHR)%.txt

# create build folders

$(PATHB):
	$(MKDIR) $(PATHB)

$(PATHO):
	$(MKDIR) $(PATHO)

$(PATHR):
	$(MKDIR) $(PATHR)

# compile hsmfui

$(PATHO)Hsmfui.o: $(PATHS)Hsmfui.c $(PATHS)Hsmfui.h 
	$(COMPILE) $(CFLAGS) $< -o $@

# compile Unity

$(PATHO)unity.o : $(PATHU)unity.c $(PATHU)unity.h
	$(COMPILE) $(CFLAGS) $< -o $@

# compile tests

$(PATHO)%.o: $(PATHT)%.c $(PATHS)Hsmfui.h
	$(COMPILE) $(CFLAGS) $< -o $@

# link tests

$(PATHB)Test%.$(TARGET_EXTENSION): $(PATHO)Test%.o $(PATHO)%.o $(PATHU)unity.o
	$(LINK) -o $@ $^

# run tests

$(PATHR)%.txt: $(PATHB)%.$(TARGET_EXTENSION)
	-./$< > $@ 2>&1

# show/summarise testresults

RESULTS = $(patsubst $(PATHT)Test%.c,$(PATHR)Test%.txt,$(SRCT))
test: $(BUILD_PATHS) $(RESULTS)
	@cat $(PATHR)*.txt
	@echo
	gcov $(PATHO)Hsmfui.gcno
	@echo

# clean up

clean:
	rm -rf $(PATHB)
	rm *.gcov
