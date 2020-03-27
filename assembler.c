#include "first_pass.h"
#include "second_pass.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int process_file(char* file_name) {
  int IC;
  int DC;
  Symbol_node* symbol_head = NULL;
  Info_node* info_head = NULL;
  int error;
  if ((error = first_pass(file_name, &IC, &DC, &symbol_head, &info_head))) {
    free_symbol_list(symbol_head);
    free_info_list(info_head);
    return error;
  }
  second_pass(file_name, IC, DC, symbol_head, info_head);

  free_symbol_list(symbol_head);
  free_info_list(info_head);
  printf("DBG process_file: IC = %d ,DC = %d\n", IC, DC);
  return NO_ERROR;
}


int main(int argc, char *argv[]) {
  int i, error;
  if (argc == 1) {
    printf("Error: no files provided.\n");
    return 1;
  }
  for (i = 1; i<argc; i++) {
    printf("Processing input file [%s].\n", argv[i]);
    if ((error = process_file(argv[i]))) {
      printf("\tFailed processing input file [%s.as].\n", argv[i]);
      if (error == ERR_MALLOC_FAILED) {
	break;
      }
    } else {
      printf("\tSuccessfully processed input file [%s].\n", argv[i]);
    }
  }
  return 0;
}