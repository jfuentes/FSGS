CC=g++
CFLAGS=--std=c++11

all: tests prof main

TEST_SOURCES= test/all_tests.cc test/test_query.cc test/test_blocks_vector.cc test/test_radix_tree.cc
tests: $(TEST_SOURCES)
	$(CC) $(CFLAGS) $(TEST_SOURCES) -o bin/tests

prof: query.h blocks_vector.h radix_tree.h profiling.cc
	$(CC) $(CFLAGS) profiling.cc -o bin/profiling -L/usr/lib/x86_64-linux-gnu -lboost_timer -lboost_filesystem -O3

main: query.h blocks_vector.h radix_tree.h main.cc
		$(CC) $(CFLAGS) main.cc -o bin/main -L/usr/lib/x86_64-linux-gnu -O3 -pthread
