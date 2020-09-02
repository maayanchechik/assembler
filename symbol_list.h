#ifndef __SYMBOL_LIST_H__
#define __SYMBOL_LIST_H__

typedef struct Symbol Symbol_node;

/* Functions for Symbol_node */
void increase_symbol_table_addresses(Symbol_node* head, int num);
int append_symbol(Symbol_node** head, char* symbol_name, int address,
		   int is_before_directive, int is_external);
void print_symbol_list(Symbol_node* head);
Symbol_node* find_symbol(Symbol_node* head, char* symbol_name);
int get_symbols_is_external(Symbol_node *ptr);
int get_symbols_address(Symbol_node *ptr);
int get_symbols_is_entry(Symbol_node *ptr);
void set_symbols_is_entry(Symbol_node *ptr);
void free_symbol_list(Symbol_node* head);

#endif
