#ifndef __LISTS_H__
#define __LISTS_H__

typedef struct Symbols Symbol_node;
typedef struct info Info_node;

/* Functions for Symbol_node */
void increase_symbol_table_addresses(Symbol_node* head, int num);
void append_symbol(Symbol_node** head, char* symbol_name, int address,
		   int is_before_directive, int is_external);
void print_symbol_list(Symbol_node* head);
Symbol_node* find_symbol(Symbol_node* head, char* symbol_name);
int get_symbols_is_external(Symbol_node *ptr);
int get_symbols_address(Symbol_node *ptr);

/* Functions for Info_node */
void append_info(Info_node** head, int address, int value);
void print_info_list(Info_node* head);
int get_info_value(Info_node* ptr);
Info_node* get_info_next(Info_node* ptr);

#endif
