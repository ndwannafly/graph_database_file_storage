all: test benchmark

test: test.o graph_db.o
	gcc build/test.o build/graph_db.o -o build/test

benchmark: benchmark.o graph_db.o
	gcc build/benchmark.o build/graph_db.o -o build/benchmark

test.o: test/test.c
	gcc -o build/test.o -c test/test.c

graph_db.o: lib/graph_db.c
	gcc -o build/graph_db.o -c lib/graph_db.c

benchmark.o: test/benchmark.c
	gcc -o build/benchmark.o -c test/benchmark.c

clean:
	rm -rf build/*
