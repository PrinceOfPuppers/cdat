CC = gcc

SRC = src

# Where the .a and .o files are compiled to
LIB = lib

PROGRAM = cdat
CFLAGS= -Wall -std=gnu99

INC = -Iinclude

FILES = $(wildcard $(SRC)/*.c)
FILENAMES = $(basename $(notdir $(wildcard $(SRC)/*.c) ))

# Testing exclusive
TESTDIR = testing
TESTFILES = $(wildcard $(TESTDIR)/$(SRC)/*.c)
TESTBUILD = build
TESTPROGRAM = test
TESTLIB = -L . -l:lib/cdat.a


all: compile

# compile but with debug flag
debug: CFLAGS += -g
debug: compile

compile:
	@mkdir -p $(LIB)

	for path in $(FILENAMES); do \
		$(CC) $(CFLAGS) -c $(SRC)/$$path.c $(INC) -o $(LIB)/$$path.o; \
	done

	ar rcs $(LIB)/$(PROGRAM).a $(addprefix $(LIB)/, $(addsuffix .o,$(FILENAMES)))

test: debug

	@mkdir -p $(TESTDIR)/$(TESTBUILD)
	$(CC) $(CFLAGS) $(TESTFILES) $(TESTLIB) $(INC) -o $(TESTDIR)/$(TESTBUILD)/$(TESTPROGRAM)
	./$(TESTDIR)/$(TESTBUILD)/$(TESTPROGRAM)

clean:
	rm -rf $(LIB)/*
	rm -rf $(TESTDIR)/$(TESTBUILD)/*
