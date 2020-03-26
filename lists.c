#include "lists.h"
#include "utilities.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct Symbols {
  char* symbol_name;
  int address;
  int is_before_directive;
  int is_external;
  int entry;
  struct Symbols *next;
};

int get_symbols_is_external(Symbol_node *ptr) {
  return ptr->is_external;
}
int get_symbols_address(Symbol_node *ptr) {
  return ptr->address;
}



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
  ptr -> entry = 0;
  length = strlen(symbol_name);
  ptr -> symbol_name = (char*) malloc(length);
  strcpy(ptr->symbol_name, symbol_name);
  ptr -> next = NULL;
  return ptr;
}

/* This function receives a symbol tables head and a new symbols info.
   If the symbole is already in the table, the function returns an
   error.  Otherwise, the function appends the symbol to the end of
   the table and returns 0.*/
int append_symbol(Symbol_node** head, char* symbol_name, int address,
		   int is_before_directive, int is_external){
  Symbol_node* ptr;
  Symbol_node* new_symbol = create_symbol(symbol_name, address, is_before_directive, is_external);
  ptr = *head;
  if(*head == NULL){
    *head = new_symbol;
    return 0;
  }
  while(ptr->next!=NULL){
    if(!strcmp((ptr->symbol_name),symbol_name)){
      free(new_symbol);
      return ERR_REPEATING_SYMBOL;
    }
    ptr = ptr->next;
  }
  if(!strcmp((ptr->symbol_name),symbol_name)){
    free(new_symbol);
    return ERR_REPEATING_SYMBOL;
  }
  ptr->next = new_symbol;
  return 0;
}


void increase_symbol_table_addresses(Symbol_node* head, int num){
  while(head != NULL){
    if(head-> is_before_directive == 1)
      head->address += num;
    head = head -> next;
  }
}

void print_symbol(Symbol_node* ptr){
  char* symbol_name;
  int address;
  char* is_before_directive;
  char* is_external;
  char* entry;
  if(ptr == NULL){
    printf("symbol is null\n");
    return;
  }
  symbol_name = ptr -> symbol_name;
  address = ptr -> address;
  if(!(ptr -> is_before_directive)){
    is_before_directive = "no";
  }else{
    if((ptr -> is_before_directive) == 1){
      is_before_directive = "yes";
    }else{
      if((ptr -> is_before_directive) == -1){
	is_before_directive = "unknown";
      }else{
	is_before_directive = "error";
      }
    }
  }  
  is_external = ((ptr -> is_external)? "yes":"no");
  entry = ((ptr -> entry)? "yes":"no");
  printf("symbol name: %s, address: %d, before directive: %s, is external: %s, is entry: %s\n", symbol_name, address, is_before_directive, is_external, entry);
}

void print_symbol_list(Symbol_node* head){
  if(head == NULL){
    printf("The symbol list is empty.\n");
    return;
  }
  printf("Symbol list:\n");
  while(head != NULL){
    printf("\t");
    print_symbol(head);
    head = head -> next;
  }
}

Symbol_node* find_symbol(Symbol_node* head, char* symbol_name){
  if (head == NULL)
    return NULL;
  
  while(head != NULL){
    if (!strcmp((head->symbol_name), symbol_name))
      return head;
    head = head->next;
  }
  return head;
}
  
/* ================================== */ 


struct info {
  int address;
  int value;
  struct info *next;
};

int get_info_value(Info_node* ptr) {
  return ptr->value;
}
Info_node* get_info_next(Info_node* ptr) {
  return ptr->next;
}



Info_node* create_info(int address, int value){
  Info_node* ptr = (Info_node*) malloc(sizeof(Info_node));
  if(ptr == NULL){
    printf("Malloc has failed to enter %d into the info table",value);
    return NULL;
  }
  ptr -> address = address;
  ptr -> value = value;
  ptr -> next = NULL;
  return ptr;
}

void append_info(Info_node** head, int address, int value){
  Info_node* ptr;
  Info_node* new_info = create_info(address, value);
  if(head == NULL){
    printf("prog error: refrence to head of info table is empty.\n");
    return;
  }
  ptr = *head;
  if(*head == NULL){
    *head = new_info;
    return;
  }
  while(ptr->next!=NULL){
    ptr = ptr->next;
  }
  ptr->next = new_info;
}

void increase_info_table_addresses(Info_node* head, int num){
  while(head != NULL){
    head->address += num;
    head = head -> next;
  }
}

void print_info(Info_node* ptr){
  int address;
  int value;
  if(ptr == NULL){
    printf("info is null\n");
    return;
  }
  address = ptr -> address;
  value = ptr -> value;
  printf("address: %d, value: %d\n", address, value);
}

void print_info_list(Info_node* head){
  if(head == NULL){
    printf("The info list is empty.\n");
    return;
  }
  printf("Info list:\n");
  while(head != NULL){
    printf("\t");
    print_info(head);
    head = head -> next;
  }
}
