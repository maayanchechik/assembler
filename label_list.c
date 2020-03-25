#include "assembler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void increase_symbol_table_addresses(Symbol_node* head, int num);
void append_symbol(Symbol_node** head, char* symbol_name, int address,
		   int is_before_directive, int is_external);

struct Symbols{
  char* symbol_name;
  int address;
  int is_before_directive;
  int is_external;
  struct Symbols *next;
};

/* This function creates a new node with the received counter and file
   name, and returns a pointer to the new node.*/
Symbol_node* create_symbol(char* symbol_name, int address, int is_before_directive, int is_external){
  Symbol_node* ptr = (Symbol_node*) malloc(sizeof(Symbol_node));
  int length;
  if(ptr == NULL){
    printf("Malloc has failed to enter %s into the symbol table",symbol_name);
    return NULL;
  }
  ptr -> address = address;
  ptr -> is_before_directive = is_before_directive;
  ptr -> is_external = is_external;
  length = strlen(symbol_name);
  ptr -> symbol_name = (char*) malloc(length);
  strcpy(ptr->symbol_name, symbol_name);
  ptr -> next = NULL;
  return ptr;
}

/* This function receives a symbol tables head and a new symbols info.
   If the symbole is already in the table, the function returns an error.
   Otherwise, the function appends the symbol to the end of the table.*/
void append_symbol(Symbol_node** head, char* symbol_name, int address,
		   int is_before_directive, int is_external){
  Symbol_node* ptr;
  Symbol_node* new_symbol = create_symbol(symbol_name, address, is_before_directive, is_external);
  ptr = *head;
  while(ptr!=NULL){
    if(!strcmp(ptr->symbol_name,symbol_name)){
      printf("Error: the symbol %s appears in the symbol table multipule times.", symbol_name);
      free(new_symbol);
    }
    ptr = ptr->next;
  }
  ptr = new_symbol;
}


void increase_symbol_table_addresses(Symbol_node* head, int num){
  while(head != NULL){
    if(head-> is_before_directive)
      head->address = address+num;
    head = head -> next;
  }
}
