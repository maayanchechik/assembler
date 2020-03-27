#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "first_pass.h"
#include "utilities.h"


/* This function receives the file prefix passed to main's arguments,
   a pointer to the instruction counter and the data counter, a
   pointer to the head of the symbol table and to the head of the info
   table. It opens and reades the .as file, line by line, preforms the
   first pass, creating a symbol table and an info table.  The
   function returns the appropriate error code for the given file. */
int first_pass(char* prefix, int* IC, int* DC, Symbol_node** symbol_table,
	       Info_node** info_table) {
  char* line_buf = NULL;
  size_t line_buf_size = 0;
  int line_size = 1;
  int error = NO_ERROR;
  int line_num = 1;
  FILE * fp;
  char* filename_as = (char *) malloc(strlen(prefix)+3);

  /* open the file */
  strcpy(filename_as, prefix);
  strcat(filename_as, ".as");
  fp = fopen(filename_as, "r");
  if (fp == NULL) {
    printf("Error: the file %s cannot be opened for writing.\n", filename_as);
    return ERR_FILE;
  }
  free(filename_as);

  /* Go through each line in the file.*/
  (*DC) = 0;
  (*IC) = 100; 
  while (line_size>=0) {
    line_size = getline(&line_buf, &line_buf_size, fp);
    if ((line_size>0) && (!string_blank_or_empty(line_buf))
	&& (!is_comment(line_buf))&&line_buf[0]!='\n') {
      printf("\nFirst_pass line #%d is: %s",line_num, line_buf);
      if (line_buf[line_size-1] == '\n') {
	line_buf[line_size-1] = '\0';
      }
      error = process_line(line_buf, IC, DC, symbol_table, info_table,
			   line_num) || error;
      printf("First_pass IC = %d, DC = %d\n", *IC, *DC);
    }
    line_num++;
  }
  printf("\n");
  increase_symbol_table_addresses(*symbol_table, *IC);
  print_symbol_list(*symbol_table);
  print_info_list(*info_table);
  fclose(fp);
  return error;
}

/* This function receives the current line buffer, the current line
   number, a pointer to IC and DC, which are the instruction and data
   counters, a pointer to the head of the symbol table and a pointer
   to the head of the info table. It processes the line, and updates
   the symbol and info table. The function returns the appropriate
   error code for the given line buffer.*/
int process_line(char* line_buf, int* IC, int* DC, Symbol_node** symbol_table,
		 Info_node** info_table, int line_num) {
  int line_has_label = 0;
  char* label;
  char* command;
  int command_type;
  int error = NO_ERROR;
  if (strlen(line_buf)>80) {
    printf("process_line: Error: line #%d, %s", line_num,
	   error_message(ERR_LINE_TOO_LONG));
    return ERR_LINE_TOO_LONG;
  }
  
  /* Check if the line has a label, if so check the validity of the
     label and the following line.*/
  line_has_label = has_label(&line_buf, &label);
  if (line_has_label) {
    if (line_has_label == ERR_NO_BLANK_AFTER_LABEL) {
      printf("process_line: Error: line #%d, illegal label: %s", line_num,
	     error_message(ERR_NO_BLANK_AFTER_LABEL));
      return ERR_NO_BLANK_AFTER_LABEL;
    }
    if ((error = symbol_is_illegal(label))) {
      printf("process_line: Error: line #%d, illegal label: %s", line_num,
	     error_message(error));
      return error;
    }
    if (string_blank_or_empty(line_buf)) {
      printf("process_line: Error: line #%d label is not followed by code.\n",
	     line_num);
      return ERR_SYMBOL_THEN_BLANK;
    }
  }

  /* Parce the line buffer to extract the command and handle the rest
     of the line according to that command type.*/
  command = strtok_r(line_buf, " \t", &line_buf);
  command_type = extract_command(command);
  switch(command_type) {
  case ENTRY:
    error = handle_entry(line_has_label, line_num, line_buf);
    break;
  case EXTERN:
    error = handle_extern(line_has_label, line_num, line_buf, symbol_table);
    break;    
  case INSTRUCTIVE:
    error = handle_instructive(line_has_label, line_num, command, line_buf, 
			       label, symbol_table, IC);    
    break;    
  case DIRECTIVE:
    error = handle_directive(line_has_label, line_num, command, line_buf,
			     label, symbol_table, info_table, DC);
    break;   
  default:
    /* Otherwise, the first word is unfamiliar*/
    printf("process_line: Error: line #%d starts with an unknown word %s\n",
	   line_num, command);
    error = ERR_COMMAND_UNKNOWN;
  }
  
  return error;
}

/* This function is called in case the current line's command is
   [.entry]. It receives the line_has_label flag, the current line
   buffer, without the [.entry] command and the current line number. The
   function processes line_buf and returns the appropriate error
   code. */
int handle_entry(int line_has_label, int line_num, char* line_buf) {
  int error = NO_ERROR;
  if (line_has_label) {
    printf("handle_entry: Error: line #%d has '.entry' after a label\n",
	   line_num);
    return ERR_SYMBOL_THEN_ENTRY;
  }
  remove_blank_from_token(&line_buf);
  if ( (error = symbol_is_illegal(line_buf)) ) {
    printf("handle_entry: Error: line #%d, symbol = [%s]- %s\n",
	   line_num, line_buf,
	   error_message(error));
    return error;
  }
  return NO_ERROR;
}

/* This function is called in case the current line's command is
   [.extern]. It receives the line_has_label flag, the current line
   buffer, without the [.extern] command should include only the
   external symbol, the current line number and a pointer to the head
   of the symbol table. If the external symbol is valid the function
   appends it to the symbol table, otherwise it returns the
   appropriate error code. If the line has a label the function also
   returns an error.*/
int handle_extern(int line_has_label, int line_num, char* line_buf,
		  Symbol_node** symbol_table) {
  int error = NO_ERROR;
  if (line_has_label) {
    printf("handle_extern: Error: line #%d has '.extern' after a label\n",
	   line_num);
    error = ERR_SYMBOL_THEN_EXTERN;
  }
  remove_blank_from_token(&line_buf);
  if ((error = symbol_is_illegal(line_buf))) {
    printf("handle_extern: Error: line #%d, the symbol [%s] is illegal.\n",
	   line_num, line_buf);
    return error;
  }
  if ((error = append_symbol(symbol_table, line_buf, 0, UNKNOWN, TRUE))) {
    printf("handle_extern: Error: line #%d, symbol = [%s], %s",
	   line_num, line_buf, error_message(error));
  }    
  return error;
}

/* This function is called in case the current line's command is an
   instruction. It receives the line_has_label flag, the current line
   number, the instruction, the current line buffer without the
   instruction (should include only the instruction's arguments), the
   label, a pointer to the head of the symbol table and a pointer to
   the instruction counter IC. If the current line has a label, the
   function appends the received label to the symbol table, or returns
   the appropriate error code if the symbol is invalid. If the line is
   valid, the function updates IC according to the needed memory words
   for the current line. Otherwise, it returns the appropriate error
   code.*/
int handle_instructive(int line_has_label, int line_num, char* instruction,
		       char* line_buf, char* label, Symbol_node** symbol_table,
		       int *IC) { 
  int length;
  int opcode = which_instructive(instruction);
  int error;
  if (line_has_label) {
    if ((error = append_symbol(symbol_table, label, *IC, FALSE, FALSE))) {
      printf("handle_instructive: Error: line #%d, label = [%s], %s",
	     line_num, label,
	     error_message(error));
      return error;
    }    
  }
  if((error = compute_required_length(opcode, line_buf, &length))) { 
    printf("handle_instructive: Error: line #%d, illegal instruction: %s",
	   line_num, error_message(error));
    return error;
  }
  (*IC) += length;
  return 0;
}

/* This function is called in case the current line's command is a
   direction. It receives the line_has_label flag, the current line
   number, the direction, the current line buffer without the
   direction (should include only the direction's arguments), the
   label, a pointer to the head of the symbol table, a pointer to the
   head of the instruction table and a pointer to the data counter
   DC. If the current line has a label, the function appends the
   received label to the symbol table, or returns the appropriate
   error code if the symbol is invalid. If the line is valid, the
   function appends the new info to the info table. Otherwise, it
   returns the appropriate error code.*/
int handle_directive(int line_has_label, int line_num,  char* direction,
		     char* line_buf, char* label, Symbol_node** symbol_table,
		     Info_node** info_table, int* DC) { 
  int error = 0;
  int is_data = (direction[1] == 'd');
  if (line_has_label) {
    if ((error = append_symbol(symbol_table, label, *DC, TRUE, FALSE))) {
      printf("handle_directive: Error: line #%d, label = [%s], %s",
	     line_num, label,
	     error_message(error));
      return error;
    }    
  }
  if (is_data) {
    if ((error = update_data_info(line_buf, DC, info_table))) {
      printf("handle_directive: Error: line #%d, illegal directive: %s",
	     line_num, error_message(error));
      return error;
    }
  }else{
    if ((error = update_string_info(line_buf, DC, info_table))) {
      printf("handle_directive: Error: line #%d, illegal directive: %s",
	     line_num, error_message(error));
      return error;
    }
  }
  return 0;
}
