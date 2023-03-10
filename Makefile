all: main

main: main.o graph_db.o data_helper.o
	gcc build/main.o build/graph_db.o build/data_helper.o -o build/main

main.o: test/main.c
	gcc -o build/main.o -c test/main.c

graph_db.o: code/graph_db.c
	gcc -o build/graph_db.o -c code/graph_db.c

data_helper.o: code/data_helper.c
	gcc -o build/data_helper.o -c code/data_helper.c

clean:
	rm -rf build/*.o;\
	rm build/main
