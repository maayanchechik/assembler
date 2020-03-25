FLAGS = -Wall -ansi -pedantic

all: assem


assem: main.o first_pass.o second_pass.o lists.o utilities.o
	gcc -o assem main.o first_pass.o second_pass.o lists.o utilities.o

main.o: main.c first_pass.h
	gcc -c $(FLAGS) main.c

first_pass.o: first_pass.c first_pass.h
	gcc -c $(FLAGS) first_pass.c

second_pass.o: second_pass.c second_pass.h
	gcc -c $(FLAGS) second_pass.c

lists.o: lists.c lists.h
	gcc -c $(FLAGS) lists.c	

utilities.o: utilities.c utilities.h
	gcc -c $(FLAGS) utilities.c

clean:
	rm *.o
