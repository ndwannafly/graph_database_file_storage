all: main

main: main.o graph_db.o data_helper.o
	gcc main.o graph_db.o data_helper.o -o main

main.o: main.c
	gcc -c main.c

graph_db.o: graph_db.c
	gcc -c graph_db.c

data_helper.o: data_helper.c
	gcc -c data_helper.c

clean:
	rm -rf *.o;\
	rm main
