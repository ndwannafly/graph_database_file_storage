all: main

main: main.o graph_db.o
	gcc main.o graph_db.o -o main

main.o: main.c
	gcc -c main.c

graph_db.o: graph_db.c
	gcc -c graph_db.c

clean:
	rm -rf *.o;\
	rm main
