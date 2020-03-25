#include "first_pass.h"
#include "second_pass.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int process_file(char* file_name){
  int IC;
  int DC;
  Symbol_node* symbol_head = NULL;
  Info_node* info_head = NULL;
  
  if (first_pass(file_name, &IC, &DC, &symbol_head, &info_head)){
    return 1;
  }
  printf("\n\n\n**** %s **** \n\n\n", file_name);
  if (second_pass(file_name, IC, DC, symbol_head, info_head)) { 
    return 1;
  }

  printf("process_file: IC = %d ,DC = %d\n", IC, DC);
  return 0;
}


int main(int argc, char *argv[]){
  int i;
  if(argc == 1){
    printf("Error: no files provided.\n");
    return 1;
  }
  for (i = argc-1; i>0; i--){
    printf("Process the input file [%s].\n", argv[i]);
    if (process_file(argv[i])){
      return 1;
    }
  }
  return 0;
}
