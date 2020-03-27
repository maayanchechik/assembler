FLAGS = -Wall -ansi -pedantic

all: assem

assem: assembler.o first_pass.o second_pass.o symbol_list.o info_list.o utilities.o
	gcc -o assem assembler.o first_pass.o second_pass.o \
	symbol_list.o info_list.o  utilities.o

assembler.o: assembler.c first_pass.h
	gcc -c $(FLAGS) assembler.c

first_pass.o: first_pass.c first_pass.h
	gcc -c $(FLAGS) first_pass.c

second_pass.o: second_pass.c second_pass.h
	gcc -c $(FLAGS) second_pass.c

symbol_list.o: symbol_list.c symbol_list.h
	gcc -c $(FLAGS) symbol_list.c

info_list.o: info_list.c info_list.h
	gcc -c $(FLAGS) info_list.c	

utilities.o: utilities.c utilities.h
	gcc -c $(FLAGS) utilities.c

clean:
	rm *.o
