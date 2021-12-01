CC = gcc
CFLAGS = -g -std=c11 -Wall -Wshadow -Wvla -Werror -pedantic -Wno-unused-function
CFLAGS_GCOV = $(CFLAGS) -fprofile-arcs -ftest-coverage
BASE_NAME = json
SRC_C = $(BASE_NAME).c
TEST_C = test_$(SRC_C)
ASG_NICKNAME = HW12
SRC_H = $(BASE_NAME).h clog.h miniunit.h Makefile
TEST_EXPECTED = expected.txt
SUBMIT_FILES = $(TEST_C) $(TEST_EXPECTED) $(SRC_C) $(SRC_H)
TEST_ACTUAL = actual.txt
EXECUTABLE = test_$(BASE_NAME)
EXECUTABLE_DBG = $(EXECUTABLE)_dbg
EXECUTABLE_GCOV = $(EXECUTABLE)_gcov
SHELL = /bin/bash

$(EXECUTABLE): $(SRC_C) $(TEST_C) $(SRC_H)
	$(CC) -o $(EXECUTABLE) $(SRC_C) $(TEST_C) $(CFLAGS)

test: $(EXECUTABLE) $(TEST_EXPECTED)
	@if diff -a -B <("./$(EXECUTABLE)") $(TEST_EXPECTED) &> /dev/null ; then \
		if [ -t 2 ] ; then \
			echo -e -n "\x1b[32m" >> /dev/stderr; \
		fi; \
		echo "Test passed: output of $(EXECUTABLE) matches $(TEST_EXPECTED)" >> /dev/stderr; \
		if [ -t 2 ] ; then \
			echo -e -n "\x1b[0m" >> /dev/stderr; \
		fi \
	else \
		if [ -t 2 ] ; then \
			echo -e -n "\x1b[31m" >> /dev/stderr; \
		fi; \
		echo "Test failed: output of $(EXECUTABLE) does NOT match $(TEST_EXPECTED)" >> /dev/stderr; \
		if [ -t 2 ] ; then \
			echo -e -n "\x1b[0m" >> /dev/stderr; \
		fi; \
	fi
	valgrind ./$(EXECUTABLE)

submit: clean $(EXECUTABLE)
	264submit $(ASG_NICKNAME) $(SUBMIT_FILES)

pretest: submit
	264test $(ASG_NICKNAME)

coverage: $(SRC_C) $(TEST_C) 
	$(CC) -o $(EXECUTABLE_GCOV) $(SRC_C) $(TEST_C) $(CFLAGS_GCOV) -DNDEBUG
	./$(EXECUTABLE_GCOV)
	gcov -f $(SRC_C)

debug: $(SRC_C) $(TEST_C) $(SRC_H)
	$(CC) -o $(EXECUTABLE_DBG) $(SRC_C) $(TEST_C) $(CFLAGS) -DDEBUG

clean: 
	rm -f -v $(SRC_C).gcov $(BASE_NAME).gcda $(BASE_NAME).gcno test_$(BASE_NAME).gcda test_$(BASE_NAME).gcno $(TEST_ACTUAL) $(EXECUTABLE) $(EXECUTABLE) $(EXECUTABLE_GCOV) $(EXECUTABLE_DBG)

.PHONY: submit test pretest coverage debug clean
