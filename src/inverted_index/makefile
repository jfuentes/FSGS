CC=g++
CFLAGS=--std=c++11

all: tests

TEST_SOURCES= test/all_tests.cc test/test_hashindex.cc test/test_index_attr.cc
tests: $(TEST_SOURCES)
	$(CC) $(CFLAGS) $(TEST_SOURCES) -o bin/tests
