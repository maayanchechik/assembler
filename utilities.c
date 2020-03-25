#define _GNU_SOURCE

#include "lists.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* This function receives a string and returns 0 if the string has
   any non-blank character: (space or tab).*/
int string_blank_or_empty(char* s) { 
  int i;
  if (s == NULL) {
    return 1;
  }
  for (i = 0; s[i] != '\0'; i++) {
    if (s[i] != ' ' && s[i]!= '\t') {
      return 0;
    }
  }
  return 1;
}

/* Returns 1 if the token has blanks, otherwise return 0. */
int token_has_blanks(char* str) { 
  int i;
  for (i = 0; str[i] != '\0'; i++) { 
    if (str[i] == '\t' || str[i] == ' ')
      return 1;
  }
  return 0;
}  

int not_a_digit(char c) {
  return (c<'0' || c>'9') ? 1 : 0;
}

int not_numeric(char c) {
  return (not_a_digit(c) && c != '+' && c!= '-');
}

int is_alpha_numeric(char c) {
  return (((symbol[i]>='a') && (symbol[i]<='z')) ||
	  ((symbol[i]>='A') && (symbol[i]<='Z')) ||
	  ((symbol[i]>='0') && (symbol[i]<='9'))
	  );
}




/* Check if the token contains illegal characters. Return 1 if it
   does, otherwise returns 0 .*/
int not_a_number(char* str) { 
  int i;
  if (not_numeric(str[0])) { 
    return 1;
  }   
  for (i = 1; str[i] != '\0'; i++) { 
    if (not_a_digit(str[i])) {
      return 1;
    }
  }
  return 0;
}

/* This function goes over the end of an input string and retruns
   whether there is a comma after the text.*/
int string_ends_with_comma_and_blanks(char* s) { 
  int i;
  /* If the string is empty or null it cannot end with a comma.*/
  if (s == NULL|| strlen(s) == 0) { 
    return 0;
  }
  i = strlen(s) -1;
  /* While the char is an end or a space or a tab or a comma, check comma.*/
  while ((i>=0) &&
	 ((s[i] == ' ')||(s[i] == '\t')||(s[i] == ','))) {
    if (s[i] == ',')
      return 1;
    i--;
  }
  return 0;
}

/* This function receives a pointer to a token and removes all 
   blanks from the beginning and end of the token.*/
void remove_blank_from_token(char** token) { 
  int i;
  if ((*token) == NULL || strlen(*token) == 0) { 
    return;
  }
  /* Remove blanks from end of token*/
  /*MMM memory leak? free?*/
  i = strlen(*token)-1;
  while((*token)[i] == ' '||(*token)[i] == '\t') { 
    (*token)[i] = '\0';
    i--;
  }
  /* Remove blanks from beggining of token*/
  while((**token) == ' '||(**token) == '\t') { 
    (*token)++;
  }
}
/* This function receives a pointer to a token and removes all 
   blanks from the beginning and end of the token.*/
void remove_beginning_blank_from_token(char** token) { 
  if ((*token) == NULL || strlen(*token) == 0) { 
    return;
  }
  /* Remove blanks from beggining of token*/
  while((**token) == ' '||(**token) == '\t') { 
    (*token)++;
  }
}

int is_comment(char* s) { 
  return (s[0]==';');
}

int index_not_blank(char* str) { 
  int i;
  /*the string is not supposed to be only blanks*/
  for(i = 0; (str[i] == ' ')||(str[i] == '\t'); i++);
  return i;
}

int starts_with(char* str, char* wanted) { 
  int i = index_not_blank(str);
  int j;
  for(j = 0; (wanted[j] != '\0') && (str[i] != '\0') && (str[i] != ' ') && (str[i] != '\t'); i++, j++) { 
    if (str[i] != wanted[j])
      return 0;
  }
  /* wanted = "abc", str = "ab"*/
  if (wanted[j] != '\0') { 
    return 0;
  }else{
    /* wanted = "abc", str = "abcd"*/    
    if ( (str[i] != '\0') && (str[i] != ' ') && (str[i] != '\t') )
      return 0;
  }  
  return i;
}

/************************************************************************/

int extract_command(char* command) { 
  if (!strcmp(command, ".entry"))
    return ENTRY;
  if (!strcmp(command, ".extern"))
    return EXTERN;
  if (is_instructive(command))
    return INSTRUCTIVE;
  if ((!strcmp(command, ".data"))||(!strcmp(command,".string")))
    return DIRECTIVE;
  return ILLEGAL_COMMAND_TYPE;
}

/************************************************************************/

int is_instructive(char *str) { 
  char* instructions[16] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "in
c", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
  int i;
  for(i = 0; i<16; i++) { 
    if (!strcmp(str,instructions[i])) { 
      return i+1;
    }
  }
  return 0;
}

int is_directive(char* line_buf) { 
  return starts_with(line_buf, ".data") ||
    starts_with(line_buf,".string");
}



// TODO(maayan) add ': ' after label 
int has_label(char** str, char** label) { 
  int i;
  for(i = 0; (*str)[i] != '\0'; i++) { 
    if ((*str)[i] == ':') { 
      (*label) = strtok_r(*str, ":", str);
      remove_beginning_blank_from_token(label);
      return 1;
    }
  }
  return 0;
}

int symbol_is_illegal(char* symbol) { 
  int i;
  if (string_blank_or_empty(symbol))
    return ERR_SYMBOL_EMPTY;
  for(i = 0; symbol[i] != '\0'; i++) {
    if (!is_alpha_numeric(str[i]) {
      return ERR_SYMBOL_ILLEGAL_CHAR;
  }
  if ((!strcmp(symbol,"data"))||(!strcmp(symbol,"string"))||
     is_instructive(symbol))
    return ERR_SYMBOL_SAVED_WORD;
  if (i>31)
    return ERR_SYMBOL_TOO_LONG;
  return 0;
}


int extract_addr(char* arg) {
  /* For now, if the address type is not legal to any other then it is
     ADDR_DIRRECT, even if it is not legal to ADDR_DIRRECT*/
  if (legal_immediate_addr(arg)) { 
    return ADDR_IMMEDIATE;
  }
  if (arg[0] == '*' && arg[1] == 'r' && (arg[2]>='0' && arg[2]<='7')
      && arg[3] == '\0') { 
    return ADDR_REG_INDIRECT;
  }
  if (arg[0] == 'r' && (arg[1]>='0' && arg[1]<='7') && arg[2] == '\0') { 
    return ADDR_REG_DIRECT;
  }
  return ADDR_DIRECT;
} 

int legal_immediate_addr(char* arg) { 
  int i;
  int num;
  char* non_digit;
  /* MMM check legal size*/
  if (arg[0] != '#')
    return 0;
  arg++;
  if (not_a_number(arg)) {
    return 0;
  }
  num = strtol(arg, &non_digit, 10);
  if (num>4095 || num<-4096)
    return 0;
  return 1;
}

int extract_one_argument(char* line_buf, char** arg1) { 
  /* If the string ends with a comma it is an error that the following
     loop will not catch it becuase it seperates with the commas.*/
  if (string_ends_with_comma_and_blanks(line_buf))
    return ERR_INSTR_ENDS_WITH_COMMA;  
  *arg1 = strsep(&line_buf, ",");
  if (arg1 == NULL || string_blank_or_empty(*arg1))
    return ERR_INSTR_ARG_EMPTY;
  if (!string_blank_or_empty(line_buf))
    return ERR_INSTR_EXCESSIVE_TEXT;
  remove_blank_from_token(arg1);
  return 0;
}

int extract_two_arguments(char* line_buf, char** arg1, char** arg2) { 
  /* If the string ends with a comma it is an error that the following
     loop will not catch it becuase it seperates with the commas.*/
  if (string_ends_with_comma_and_blanks(line_buf))
    return ERR_INSTR_ENDS_WITH_COMMA;  
  *arg1 = strsep(&line_buf, ",");
  if (arg1 == NULL || string_blank_or_empty(*arg1))
    return ERR_INSTR_ARG_EMPTY;
  *arg2 = strsep(&line_buf, ",");
  if (arg2 == NULL || string_blank_or_empty(*arg2))
    return ERR_INSTR_ARG_EMPTY;
  if (!string_blank_or_empty(line_buf))
    return ERR_INSTR_EXCESSIVE_TEXT;
  remove_blank_from_token(arg1);
  remove_blank_from_token(arg2);
  return 0;
}

int compute_required_length(int opcode, char* line_buf, int *length) {
  int addr1, addr2;  
  char *arg1, *arg2;
  int error = 0;
  switch (opcode) { 
  case MOV:
  case CMP:    
  case ADD:
  case SUB:
  case LEA:
    error = extract_two_arguments(line_buf, &arg1, &arg2);
    if (!error) { 
      addr1 = extract_addr(arg1);
      addr2 = extract_addr(arg2);
      if (opcode!=CMP && addr2 == ADDR_IMMEDIATE)
	error = ERR_INSTR_DESTINATION_IMMEDIATE;
      if (opcode == LEA && addr1 != ADDR_DIRECT)
	error = ERR_INSTR_LEA_ORIGIN_NOT_DIRECT;
      if ( ( (addr1 == ADDR_REG_DIRECT) || (addr1 == ADDR_REG_INDIRECT) ) &&
	   ( (addr2 == ADDR_REG_DIRECT) || (addr2 == ADDR_REG_INDIRECT) ) ) { 
	*length = 2;
      } else {
	*length = 3;
      }
    }
    break;
  case CLR:
  case NOT:
  case INC:
  case DEC:
  case JMP:
  case BNE:
  case RED:
  case PRN:
  case JSR:
    error = extract_one_argument(line_buf, &arg1);
    if (!error) { 
      arg2 = NULL;
      addr1 = extract_addr(arg1);
      if (opcode!=PRN && addr1 == ADDR_IMMEDIATE)
	error = ERR_INSTR_DESTINATION_IMMEDIATE;
      if (((opcode == JMP)||(opcode == BNE)||(opcode == JSR))&&(addr1 == ADDR_REG_INDIRECT))
	error = ERR_INSTR_ILLEGAL_REG_INDIRECT;
      addr2 = ADDR_ILLEGAL;
      *length = 2;
    }
    break;
  case RTS:
  case STOP:    
    if (!string_blank_or_empty(line_buf))
      error = ERR_INSTR_EXCESSIVE_TEXT;
    addr1 = ADDR_ILLEGAL;
    addr2 = ADDR_ILLEGAL;
    *length = 1;
    break;
  case ILLEGAL_OPCODE:
    error = ERR_INSTR_ILLEGAL_OPCODE;
  }
  
  return error;    
}

int update_data_info(char* line_buf, int* DC, Info_node** info_table) { 
  char* token; 
  char* rest = line_buf; 
  int num;
  char *non_digit;
  int error = 0;
  /* If the string is empty then there is no data passed to .data*/
  if (string_blank_or_empty(line_buf)) { 
    error = ERR_DATA_EMPTY;
    return error;
  }
  /* If the string ends with a comma it is an error that the following
     loop will not catch it becuase it seperates with the commas.*/
  if (string_ends_with_comma_and_blanks(line_buf)) { 
    error = ERR_DATA_ENDS_WITH_COMMA;
    return error;
  }
  while ((token = strsep(&rest, ","))) { 
    remove_blank_from_token(&token);
    /* Handle missing commas, e.g. input="5,4 3,2" */
    if (token_has_blanks(token) ) {
      error = ERR_DATA_NO_COMMA;
      return error;
    }
    /* Handle illegal characters, e.g. input="3,2 j" */
    if (not_a_number(token) ) {
      error = ERR_DATA_ILLEGAL_CHAR;
      return error;
    }
    /* Handle double commas, e.g. input="5,4,,3" */
    if (string_blank_or_empty(token)) { 
      error = ERR_DATA_DOUBLE_COMMA;
      return error;
    }

    num = strtol(token, &non_digit, 10);

    /* Handle numbers out of range */
    if (num < -16383 || num > 16383) {
      error = ERR_DATA_OUT_OF_RANGE;
      return error;
    }

    append_info(info_table, (*DC), num);
    (*DC)++;
  }
  
  return 0;
}

int update_string_info(char* line_buf, int* DC, Info_node** info_table) { 
  int i;
  remove_blank_from_token(&line_buf);
  if (illegal_quotation_marks(line_buf)) { 
    return ERR_STRING_ILLEGAL_QUOTATION;
  }
  for(i = 1; line_buf[i] != '"'; i++, (*DC)++) { 
    if (line_buf[i]<32)
      return ERR_STRING_ILLEGAL_CHAR;
    append_info(info_table, (*DC), line_buf[i]);
  }
  append_info(info_table, (*DC), 0);
  (*DC)++;
  return 0;
}

int illegal_quotation_marks(char* line_buf) { 
  int i;
  if (line_buf[0] != '"')
    return 1;
  for (i = 1; line_buf[i] != '\0'; i++) { 
    if ((line_buf[i] == '"') && (line_buf[i+1] != '\0'))
      return 1;
  }
  return 0;
}


char* error_message(int error_code) { 
  char* message;
  switch(error_code) { 
  case ERR_SYMBOL_EMPTY:
    message = "there is an empty symbol\n";
    break;
  case ERR_SYMBOL_ILLEGAL_CHAR:
    message = "the symbol includes an illegal character\n";
    break;
  case ERR_SYMBOL_SAVED_WORD:
    message = "the symbol is a saved word\n";
    break;
  case ERR_SYMBOL_TOO_LONG:
    message = "the symbol is over 31 characters\n";
    break;
  case ERR_SYMBOL_THEN_BLANK:
    message = "there is no text after the symbol\n";
    break;
  case ERR_SYMBOL_THEN_ENTRY:
    message = "there is a .entry command after the symbol\n";
    break;
  case ERR_SYMBOL_THEN_EXTERN:
    message = "there is a .extern command after the symbol\n";
    break;
  case ERR_INSTR_ARG_EMPTY:
    message = "there is an empty argument\n";
    break;
  case ERR_INSTR_DESTINATION_IMMEDIATE:
    message = "the distination argument is of an immediate type\n";
    break;
  case ERR_INSTR_EXCESSIVE_TEXT:
    message = "there is excessive text after the arguments\n";
    break;
  case ERR_INSTR_ENDS_WITH_COMMA:
    message = "the line ends with a comma\n";
    break;
  case ERR_INSTR_ILLEGAL_OPCODE:
    message = "the instructive command is unknown\n";
    break;
  case ERR_STRING_ILLEGAL_QUOTATION:
    message = "the string given to .string has illegal quotation marks\n";
    break;
  case ERR_STRING_ILLEGAL_CHAR:
    message = "the string includes un illegal character\n";
    break;
  case ERR_DATA_EMPTY:
    message = ".data is not given any arguments\n";
    break;
  case ERR_DATA_ENDS_WITH_COMMA:
    message = "the arguments given to data end with a comma\n";
    break;
  case ERR_DATA_NO_COMMA:
    message = "there is a missing comma between .data 's arguments\n";
    break;
  case ERR_DATA_ILLEGAL_CHAR:
    message = ".data is given an illegal character\n";
    break;
  case ERR_DATA_DOUBLE_COMMA:
    message = "there is a double comma in .data 's arguments\n";
    break;
  case ERR_DATA_OUT_OF_RANGE:
    message = "an argument given to .data is out of range\n";
    break;
  case ERR_COMMAND_UNKNOWN:
    message = "the command is unknown\n";
    break;
  case ERR_LINE_TOO_LONG:
    message = "the command line is too long(over 80 characters)\n";
    break;
  case ERR_INSTR_LEA_ORIGIN_NOT_DIRECT:
    message = "the origin argument of the instructive command lea is not of direct type\n";
    break;
  case ERR_INSTR_ILLEGAL_REG_INDIRECT:
    message = "the argument is an indirect resiter\n";
    break;
  case ERR_UNKNOWN_ENTRY_SYMBOL:
    message = "an unknown symbol is given to entry\n";
    break;
  case ERR_EXTERNAL_ENTRY_SYMBOL:
    message = "an external symbol is given to entry\n";
    break;
  default:
    message = "error unknown\n";
  }
  return message;
}
