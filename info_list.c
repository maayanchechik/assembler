#include "assembler.h"
#include <stdio.h>

struct info{
  int address;
  int value;
  struct info *next;
};

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
  ptr = *head;
  while(ptr!=NULL){
    ptr = ptr->next;
  }
  ptr = new_info;
}

void increase_info_table_addresses(Info_node* head, int num){
  while(head != NULL){
    head->address = address+num;
    head = head -> next;
  }
}
