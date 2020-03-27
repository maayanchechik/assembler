#ifndef __INFO_LIST_H__
#define __INFO_LIST_H__

typedef struct info Info_node;

/* Functions for Info_node */
int append_info(Info_node** head, int address, int value);
void print_info_list(Info_node* head);
int get_info_value(Info_node* ptr);
Info_node* get_info_next(Info_node* ptr);
void free_info_list(Info_node* head);

#endif
