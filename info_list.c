#include "info_list.h"
#include "utilities.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* This file implements functions to handle the info table. 

   The file includes Getter functions because the info struct is
   incapsulated and is not accessible to the rest of the code. 
*/

struct info {
  int address;
  int value;
  struct info *next;
};

/* This function receives a pointer to an info node and returns the
   value of that node. */
int get_info_value(Info_node* ptr) {
  return ptr->value;
}

/* This function receives a pointer to an info node and returns the
   "next" field of that node, which is a pointer to info_node. */
Info_node* get_info_next(Info_node* ptr) {
  return ptr->next;
}

/* This function creates a new info node. It initializes its value and
    address using the input arguments and also initializes its 'next'
    field to NULL.*/
Info_node* create_info(int address, int value) {
  Info_node* ptr = (Info_node*) malloc(sizeof(Info_node));
  if (ptr == NULL) {
    printf("Malloc failed allocating memory for an info node %d", value);
    return NULL;
  }
  ptr -> address = address;
  ptr -> value = value;
  ptr -> next = NULL;
  return ptr;
}

/* This function receives an info table's head and information for a
   new info node. It appends the info node to the end of the
   table. Returns an error code. */
int append_info(Info_node** head, int address, int value) {
  Info_node* ptr;
  Info_node* new_info;
  if (head == NULL) {
    printf("programmer error: refrence to head of info table is empty.\n");
    return ERR_UNKNOWN;
  }
  new_info = create_info(address, value);
  if (new_info == NULL) {
    return ERR_MALLOC_FAILED;
  }
  
  ptr = *head;
  if (*head == NULL) {
    *head = new_info;
    return NO_ERROR;
  }
  while (ptr->next!=NULL) {
    ptr = ptr->next;
  }
  ptr->next = new_info;
  return NO_ERROR;
}

/* This function prints the content of a given info node for debugging
   purposes. */
void print_info(Info_node* ptr) {
  if (ptr == NULL) {
    printf("pointer to info_node is null.\n");
    return;
  }
  printf("address: %d, value: %d\n", ptr->address, ptr->value);
}

/* This function recieves the head of the info table and prints the
   info table for debugging purposes. */
void print_info_list(Info_node* head) {
  if (head == NULL) {
    printf("The info list is empty.\n");
    return;
  }
  printf("Info list:\n");
  while (head != NULL) {
    printf("\t");
    print_info(head);
    head = head -> next;
  }
}

/* Free the memory allocated for an info list, given a pointer to its
   head. */
void free_info_list(Info_node* head) {
  Info_node *curr;
  while (head != NULL) {
    curr = head;
    head = head->next;
    free(curr);
  }
}
