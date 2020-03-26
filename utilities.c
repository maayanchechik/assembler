#define _GNU_SOURCE

#include "lists.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define OPCODE_LENGTH 16
#define MAX_12_BITS 4095
#define MIN_12_BITS -4096
#define MAX_15_BITS 32767
#define MIN_15_BITS -32768

/************************************************************************
                              String utilities 
 ************************************************************************/

/* This function receives a string and returns FALSE(0) if the string
   has any non-blank character: (space or tab), otherwise returns
   TRUE(1).*/
int string_blank_or_empty(char* s) { 
  int i;
  if (s == NULL || strlen(s) == 0) {
    return TRUE;
  }
  for (i = 0; s[i] != '\0'; i++) {
    if (s[i] != ' ' && s[i]!= '\t') {
      return FALSE;
    }
  }
  return TRUE;
}

/* Returns TRUE if the received string has blanks, otherwise returns
   FALSE. */
int token_has_blanks(char* str) { 
  int i;
  for (i = 0; str[i] != '\0'; i++) { 
    if (str[i] == '\t' || str[i] == ' ')
      return TRUE;
  }
  return FALSE;
}  

/* Returns TRUE if received char is not a digit, otherwise returns
   FALSE*/
int not_a_digit(char c) {
  return (c<'0' || c>'9') ? TRUE : FALSE;
}

/* Returns TRUE if received char is not a numeric symbol (digit or + -
   sign), otherwise returns FALSE*/
int not_numeric(char c) {
  return (not_a_digit(c) && c != '+' && c!= '-');
}

/* Returns TRUE if received char is a letter or a digit, otherwise
   returns FALSE*/
int is_alpha_numeric(char c) {
  return (((c>='a') && (c<='z')) ||
	  ((c>='A') && (c<='Z')) ||
	  ((c>='0') && (c<='9'))
	  );
}

/* Returns TRUE if received string is not a number, otherwise returns
   FALSE*/
int not_a_number(char* str) { 
  int i;
  if (not_numeric(str[0])) { 
    return TRUE;
  }   
  for (i = 1; str[i] != '\0'; i++) { 
    if (not_a_digit(str[i])) {
      return TRUE;
    }
  }
  return FALSE;
}

/* This function retruns whether there is a comma after the text of
   the given string.*/
int string_ends_with_comma_and_blanks(char* s) {
  int i;
  if (s == NULL) 
    return 0;
  for (i=strlen(s)-1; i>=0 ; i--) {
    if (s[i] == ',') 
      return 1;
    if (s[i] != ' ' && s[i] != '\t')
      return 0;
  }
  return 0;
}

/* This function receives a pointer to a token and removes all blanks
   from the beginning and end of the token (but not in between the
   text).*/
void remove_blank_from_token(char** token) { 
  int i;
  remove_beginning_blank_from_token(token);
  
  if ((*token) == NULL || strlen(*token) == 0) { 
    return;
  }
  /* Remove blanks from end of token. */
  i = strlen(*token)-1;
  while ((*token)[i] == ' '||(*token)[i] == '\t') { 
    (*token)[i] = '\0';
    i--;
  }
}

/* This function receives a pointer to a token and removes all blanks
   from the beginning of the token until the text starts. */
void remove_beginning_blank_from_token(char** token) { 
  if ((*token) == NULL || strlen(*token) == 0) { 
    return;
  }
  /* Remove blanks from beggining of token. */
  while ((**token) == ' '||(**token) == '\t') { 
    (*token)++;
  }
}

/* Returns TRUE if the first char of the received string is ';',
   otherwise FALSE.*/
int is_comment(char* s) { 
  return (s[0]==';');
}

/* This function returns the index of the first non blank char in the
   received string.*/
int index_not_blank(char* str) { 
  int i;
  /* The string is not supposed to be only blanks. */
  for(i = 0; (str[i] == ' ')||(str[i] == '\t'); i++);
  return i;
}


/************************************************************************
                              Assembler utilities 
 ************************************************************************/

/* This function recieves a command string and returns the command's
   enum command type code.*/
int extract_command(char* command) { 
  if (!strcmp(command, ".entry"))
    return ENTRY;
  if (!strcmp(command, ".extern"))
    return EXTERN;
  if (is_instructive(command) != NOT_INSTRUCTIVE)
    return INSTRUCTIVE;
  if ((!strcmp(command, ".data")) || (!strcmp(command,".string")))
    return DIRECTIVE;
  return ILLEGAL_COMMAND_TYPE;
}

/* This function receives a command string. If the command is one of
   the instructive commands, returns the opcode of the command
   according to the enum opcode. Otherwise, returns NOT_INSTRUCTIVE.*/
int is_instructive(char *str) { 
  char* instructions[OPCODE_LENGTH] = {"mov", "cmp", "add", "sub", "lea", "clr",
			    "not", "inc", "dec", "jmp", "bne", "red",
			    "prn", "jsr", "rts", "stop"};
  int i;
  for(i = 0; i<OPCODE_LENGTH; i++) { 
    if (!strcmp(str,instructions[i])) { 
      return i;
    }
  }
  return NOT_INSTRUCTIVE;
}


/* If there is a ':' then the function checks whether there is the
   required blank after the ':'. If not, the function returns an
   errorcode that is different from 0 and 1, to indicate there is an
   illegal label. Otherwise, the function puts in label the string
   before the ':' without the beggining blanks. */
int has_label(char** str, char** label) { 
  int i;
  for(i = 0; (*str)[i] != '\0'; i++) { 
    if ((*str)[i] == ':') {
      /* (*str)[i+1] is always safe because at the end is '\0'*/
      if((*str)[i+1] != ' ' && (*str)[i+1] != '\t')
	return ERR_NO_BLANK_AFTER_LABEL;
      (*label) = strsep(str, ":");
      remove_beginning_blank_from_token(label);
      return 1;
    }
  }
  return 0;
}

int is_reg(char* token){
  if (token == NULL || strlen(token) < 2 ){
    return 0;
  }
  if (token[0] == 'r' && token[1] >= '0' && token[1] <= '7'
      && (token[2] == ' ' || token[2] == '\t' || token[2] == '\0') ){
    return 1;
  }
  return 0;
}

int symbol_is_illegal(char* symbol) { 
  int i;
  printf("symbol_is_illegal: symbol = [%s]\n", symbol);
  if (string_blank_or_empty(symbol)){
    return ERR_SYMBOL_EMPTY;
  }
  for (i = 0; symbol[i] != '\0'; i++) {
    if (!is_alpha_numeric(symbol[i])) {
      return ERR_SYMBOL_ILLEGAL_CHAR;
    }
  }
  if (i>31){
    return ERR_SYMBOL_TOO_LONG;
  }
  if ((!strcmp(symbol, "data")) || (!strcmp(symbol, "string")) ||
      (is_instructive(symbol)!=NOT_INSTRUCTIVE) || is_reg(symbol))
    return ERR_SYMBOL_RESERVED_WORD;
  return 0;
}


int extract_addr(char* arg) {
  int temp;
  if (legal_immediate_addr(arg)) { 
    return ADDR_IMMEDIATE;
  }
  if (arg[0] == '*'){
    if(is_reg(arg+1))
       return ADDR_REG_INDIRECT;
  }
  if (is_reg(arg)) { 
    return ADDR_REG_DIRECT;
  }
  temp = symbol_is_illegal(arg);
  printf("symbol_is_illegal(arg) = %d\n", temp);
  if (!temp){
    return ADDR_DIRECT;
  }
  return ADDR_ILLEGAL;
}

int legal_immediate_addr(char* arg) { 
  int num;
  char* non_digit;
  if (arg[0] != '#')
    return 0;
  arg++;
  if (not_a_number(arg)) {
    return 0;
  }
  num = strtol(arg, &non_digit, 10);
  if (num>MAX_12_BITS || num< MIN_12_BITS)
    return 0;
  return 1;
}

int extract_one_argument(char* line_buf, char** arg1) { 
  /* If the string ends with a comma it is an error that the following
     loop will not catch it becuase it seperates with the commas.*/
  if (string_ends_with_comma_and_blanks(line_buf))
    return ERR_INSTR_ENDS_WITH_COMMA;  
  *arg1 = strsep(&line_buf, ",");
  if (arg1 == NULL || (*arg1) == NULL || string_blank_or_empty(*arg1))
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
  if (arg1 == NULL || (*arg1) == NULL || string_blank_or_empty(*arg1))
    return ERR_INSTR_ARG_EMPTY;

  *arg2 = strsep(&line_buf, ",");
  if (arg2 == NULL || (*arg1) == NULL || string_blank_or_empty(*arg2))
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

      if(addr1 == ADDR_ILLEGAL || addr2 == ADDR_ILLEGAL)
	error = ERR_INSTR_ILLEGAL_OPERAND_TYPE;
      if (opcode!=CMP && addr2 == ADDR_IMMEDIATE)
	error = ERR_INSTR_DESTINATION_IMMEDIATE;
      if (opcode == LEA && addr1 != ADDR_DIRECT)
	error = ERR_INSTR_LEA_SOURCE_NOT_DIRECT;
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
      if(addr1 == ADDR_ILLEGAL)
	error = ERR_INSTR_ILLEGAL_OPERAND_TYPE;
      if (opcode!=PRN && addr1 == ADDR_IMMEDIATE)
	error = ERR_INSTR_DESTINATION_IMMEDIATE;
      if (((opcode == JMP)||(opcode == BNE)||(opcode == JSR))&&(addr1 == ADDR_REG_DIRECT))
	error = ERR_INSTR_ILLEGAL_REG_DIRECT;
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
  case NOT_INSTRUCTIVE:
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
  /* If the string starts with a comma the following loop will
     mistakingly identify the error as a double comma.*/
  remove_blank_from_token(&rest);
  if (rest[0] == ','){
    error = ERR_DATA_BEGINS_WITH_COMMA;
    return error;
  }
  while ((token = strsep(&rest, ","))) { 
    remove_blank_from_token(&token);
    /* Handle missing commas, e.g. input="5,4 3,2" */
    if (token_has_blanks(token) ) {
      error = ERR_DATA_NO_COMMA;
      return error;
    }
    /* Handle double commas, e.g. input="5,4,,3" */
    if (string_blank_or_empty(token)) { 
      error = ERR_DATA_DOUBLE_COMMA;
      return error;
    }
    /* Handle illegal characters, e.g. input="3,2 j" */
    if (not_a_number(token) ) {
      error = ERR_DATA_ILLEGAL_CHAR;
      return error;
    }

    num = strtol(token, &non_digit, 10);

    /* Handle numbers out of range */
    if (num < MIN_15_BITS || num > MAX_15_BITS) {
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
    if (line_buf[i]<32 && line_buf[i]!='\t')
      return ERR_STRING_ILLEGAL_CHAR;
    append_info(info_table, (*DC), line_buf[i]);
  }
  append_info(info_table, (*DC), 0);
  (*DC)++;
  return 0;
}

int illegal_quotation_marks(char* line_buf) { 
  int index_last = strlen(line_buf)-1;
  if (line_buf[0] != '"')
    return 1;
  if (line_buf[index_last]!='"')
    return 1;
  return 0;
}


char* error_message(int error_code) { 
  char* message;
  switch(error_code) {
  case CORRECT:
    message = "there is no error!\n.";
    break;
  case ERR_SYMBOL_EMPTY:
    message = "there is an empty symbol.\n";
    break;
  case ERR_SYMBOL_ILLEGAL_CHAR:
    message = "the symbol includes an illegal character.\n";
    break;
  case ERR_SYMBOL_RESERVED_WORD:
    message = "the symbol is a reserved word.\n";
    break;
  case ERR_SYMBOL_TOO_LONG:
    message = "the symbol is over 31 characters.\n";
    break;
  case ERR_SYMBOL_THEN_BLANK:
    message = "there is no text after the symbol.\n";
    break;
  case ERR_SYMBOL_THEN_ENTRY:
    message = "there is a [.entry] command after the symbol.\n";
    break;
  case ERR_SYMBOL_THEN_EXTERN:
    message = "there is a [.extern] command after the symbol.\n";
    break;
  case ERR_INSTR_ARG_EMPTY:
    message = "there is an empty argument.\n";
    break;
  case ERR_INSTR_DESTINATION_IMMEDIATE:
    message = "the distination argument is of an immediate type.\n";
    break;
  case ERR_INSTR_EXCESSIVE_TEXT:
    message = "there is excessive text after the arguments.\n";
    break;
  case ERR_INSTR_ENDS_WITH_COMMA:
    message = "the line ends with a comma.\n";
    break;
  case ERR_INSTR_ILLEGAL_OPCODE:
    message = "the instructive command is unknown.\n";
    break;
  case ERR_INSTR_ILLEGAL_OPERAND_TYPE:
    message = "a passed operand is not valid for any type.\n";
    break;    
  case ERR_STRING_ILLEGAL_QUOTATION:
    message = "the string given to [.string] has illegal quotation\n marks (there should be two quotation marks surrounding the given string).\n";
    break;
  case ERR_STRING_ILLEGAL_CHAR:
    message = "the string includes an illegal character.\n";
    break;
  case ERR_DATA_EMPTY:
    message = "[.data] is not given any arguments.\n";
    break;
  case ERR_DATA_BEGINS_WITH_COMMA:
    message = "the arguments given to data begins with a comma.\n";
    break;    
  case ERR_DATA_ENDS_WITH_COMMA:
    message = "the arguments given to data end with a comma.\n";
    break;
  case ERR_DATA_NO_COMMA:
    message = "there is a missing comma between .data 's arguments.\n";
    break;
  case ERR_DATA_ILLEGAL_CHAR:
    message = "[.data] is given an illegal character.\n";
    break;
  case ERR_DATA_DOUBLE_COMMA:
    message = "there is a double comma in .data 's arguments.\n";
    break;
  case ERR_DATA_OUT_OF_RANGE:
    message = "an argument given to .data is out of range.\n";
    break;
  case ERR_COMMAND_UNKNOWN:
    message = "the command is unknown.\n";
    break;
  case ERR_LINE_TOO_LONG:
    message = "the command line is too long(over 80 characters).\n";
    break;
  case ERR_INSTR_LEA_SOURCE_NOT_DIRECT:
    message = "the source argument of the instructive command lea is not of direct type.\n";
    break;
  case ERR_INSTR_ILLEGAL_REG_DIRECT:
    message = "the argument is a direct resiter.\n";
    break;
  case ERR_UNKNOWN_ENTRY_SYMBOL:
    message = "an unknown symbol is given to entry.\n";
    break;
  case ERR_UNKNOWN_SYMBOL:
    message = "an unknown symbol passed as an argument.\n";
    break;
  case ERR_EXTERNAL_ENTRY_SYMBOL:
    message = "an external symbol is given to entry.\n";
    break;
  case ERR_NO_BLANK_AFTER_LABEL:
    message = "there is no blank after the label.\n";
    break;
  case ERR_REPEATING_SYMBOL:
    message = "this symbol is already defined.\n";
    break;
  default:
    message = "error unknown.\n";
  }
  return message;
}
