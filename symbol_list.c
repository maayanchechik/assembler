#include "symbol_list.h"
#include "utilities.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* This file implements functions to handle the symbol table. 

   The file includes Getter functions because the symbol struct is
   incapsulated and is not accessible to the rest of the code. 
*/

struct Symbol {
  char* symbol_name;
  int address;
  int is_before_directive;
  int is_external;
  int entry;
  struct Symbol *next;
};

/* This function receives a pointer to a symbol node and returns
   whether the given symbol is external. */
int get_symbols_is_external(Symbol_node *ptr) {
  return ptr->is_external;
}

/* This function receives a pointer to a symbol node and returns the
   given symbol's address. */
int get_symbols_address(Symbol_node *ptr) {
  return ptr->address;
}

/* This function creates a new symbol node. It initializes its fields
   using the passed input argmuments: (1) symbol name (2) address (3)
   before directive flag(4) external flag. The function initializes
   the node "next" field to NULL. 
   If malloc fails, returns NULL, otherwise returns a pointer to teh
   new node. */
Symbol_node* create_symbol(char* symbol_name, int address,
			   int is_before_directive, int is_external) {
  Symbol_node* ptr = (Symbol_node*) malloc(sizeof(Symbol_node));
  int length;
  if (ptr == NULL) {
    printf("Malloc failed allocating memory for %s", symbol_name);
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

/* Append a symbol to a symbol list. This function receives a pointer
   to the head of a symbol table and a name of a new symbol to be
   appended at the end of the list. If the symbol is already present
   in the table, the function returns an error
   ERR_REPEATING_SYMBOL. Otherwise, returns an appropriate error
   code. */
int append_symbol(Symbol_node** head, char* symbol_name, int address,
		   int is_before_directive, int is_external) {
  Symbol_node* ptr;
  Symbol_node* new_symbol = create_symbol(symbol_name, address,
					  is_before_directive, is_external);
  if (new_symbol == NULL) {
    return ERR_MALLOC_FAILED;
  }
  
  if (*head == NULL) {
    *head = new_symbol;
    return NO_ERROR;
  }
  ptr = *head;
  while (ptr->next != NULL) {
    if (!strcmp((ptr->symbol_name), symbol_name)) {
      free(new_symbol);
      return ERR_REPEATING_SYMBOL;
    }
    ptr = ptr->next;
  }
  if (!strcmp((ptr->symbol_name),symbol_name)) {
    free(new_symbol);
    return ERR_REPEATING_SYMBOL;
  }
  ptr->next = new_symbol;
  return NO_ERROR;
}

/* This function receives the head of the symbol table and a number,
   and it updates the symbol table so that the address of every symbol
   that is before a directive will be increased by num. */
void increase_symbol_table_addresses(Symbol_node* head, int num) {
  while (head != NULL) {
    if (head-> is_before_directive == 1)
      head->address += num;
    head = head -> next;
  }
}

/* This local function receives a pointer to a symbol_node and prints
   the symbol in a human readable way for debugging purposes. */
void print_symbol_node(Symbol_node* ptr) {
  char* symbol_name;
  int address;
  char* is_before_directive;
  char* is_external;
  char* entry;
  if (ptr == NULL) {
    printf("symbol is null\n");
    return;
  }
  symbol_name = ptr -> symbol_name;
  address = ptr -> address;
  if (!(ptr -> is_before_directive)) {
    is_before_directive = "no";
  } else  if ((ptr -> is_before_directive) == 1) {
    is_before_directive = "yes";
  } else if ((ptr -> is_before_directive) == -1) {
    is_before_directive = "unknown";
  } else {
    is_before_directive = "error";
  }
  is_external = ((ptr -> is_external)? "yes":"no");
  entry = ((ptr -> entry)? "yes":"no");
  printf("symbol name: %s, address: %d, before directive: %s",
	 symbol_name, address, is_before_directive);
  printf(", is external: %s, is entry: %s\n", is_external, entry);
}

/* This function recieves the head of the symbol table and prints the
   symbol table for debugging purpouses. */
void print_symbol_list(Symbol_node* head) {
  if (head == NULL) {
    printf("The symbol list is empty.\n");
    return;
  }
  printf("Symbol list:\n");
  while (head != NULL) {
    printf("\t");
    print_symbol_node(head);
    head = head -> next;
  }
}

/* This function receives a pointer to the head of the symbol table
   and a symbol name. If there is a symbol node in the symbol table
   with the received name, the function returns a pointer to that
   symbol node, otherwise, the function returns NULL. */
Symbol_node* find_symbol(Symbol_node* head, char* symbol_name) {
  if (head == NULL)
    return NULL;
  
  while (head != NULL) {
    if (!strcmp((head->symbol_name), symbol_name))
      return head;
    head = head->next;
  }
  return head;
}

/* Free the memory allocated for an symbol list, given a pointer to its
   head. */
void free_symbol_list(Symbol_node* head) {
  Symbol_node *curr;
  while (head != NULL) {
    curr = head;
    head = head->next;
    free(curr);
  }
}

