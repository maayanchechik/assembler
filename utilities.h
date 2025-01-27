#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include "info_list.h"
#include "symbol_list.h"
#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define UNKNOWN -1

/* This is an enum of the command types*/
enum commands {ENTRY, EXTERN, INSTRUCTIVE, DIRECTIVE, ILLEGAL_COMMAND_TYPE};

/* This is an enum of the opcodes for each instructive command*/
enum opcode_enum {MOV, CMP, ADD, SUB, LEA, CLR, NOT, INC, DEC, JMP,
		  BNE, RED, PRN, JSR, RTS, STOP, NOT_INSTRUCTIVE};

/* This is an enum of the addressing types*/
enum addressing_enum {ADDR_IMMEDIATE,
		      ADDR_DIRECT,
		      ADDR_REG_INDIRECT,
		      ADDR_REG_DIRECT,
		      ADDR_ILLEGAL};

/* This is the enum of the error codes. */
enum errors{NO_ERROR, /* == 0, therefore an error flag can be checked
			 as a boolean for errors.*/
	    ERR_SYMBOL_EMPTY, 
	    ERR_SYMBOL_ILLEGAL_CHAR,
	    ERR_SYMBOL_RESERVED_WORD,
	    ERR_SYMBOL_TOO_LONG,
	    ERR_SYMBOL_THEN_BLANK,
	    ERR_SYMBOL_THEN_ENTRY,
	    ERR_SYMBOL_THEN_EXTERN,
	    ERR_INSTR_ARG_EMPTY,
	    ERR_INSTR_DESTINATION_IMMEDIATE,
	    ERR_INSTR_EXCESSIVE_TEXT,
	    ERR_INSTR_ENDS_WITH_COMMA,
	    ERR_INSTR_ILLEGAL_OPCODE,
	    ERR_INSTR_ILLEGAL_OPERAND_TYPE,
	    ERR_STRING_ILLEGAL_QUOTATION,
	    ERR_STRING_ILLEGAL_CHAR,
	    ERR_DATA_EMPTY,
	    ERR_DATA_BEGINS_WITH_COMMA,
	    ERR_DATA_ENDS_WITH_COMMA,
	    ERR_DATA_NO_COMMA,
	    ERR_DATA_ILLEGAL_CHAR,
	    ERR_DATA_DOUBLE_COMMA,
	    ERR_DATA_OUT_OF_RANGE,
	    ERR_COMMAND_UNKNOWN,
	    ERR_LINE_TOO_LONG,
	    ERR_INSTR_LEA_SOURCE_NOT_DIRECT,
	    ERR_INSTR_ILLEGAL_REG_DIRECT,
	    ERR_UNKNOWN_ENTRY_SYMBOL,
	    ERR_UNKNOWN_SYMBOL,
	    ERR_EXTERNAL_ENTRY_SYMBOL,
	    ERR_NO_BLANK_AFTER_LABEL,
	    ERR_REPEATING_SYMBOL,
	    ERR_REPEATING_EXTERNAL_SYMBOL,
	    ERR_FILE,
	    ERR_MALLOC_FAILED,	    
	    ERR_UNKNOWN};

/* String utilities */
int not_a_digit(char c);
int not_numeric(char c);
int is_alpha_numeric(char c);
int not_a_number(char* str);
int string_blank_or_empty(char* s);
int token_has_blanks(char* str);
int string_ends_with_comma_and_blanks(char* s);
void remove_blank_from_token(char** token);
void remove_beginning_blank_from_token(char** token);

/* File utilities */
int open_one_file(char* prefix, char** file_out, const char* suffix,
		  const char* permission, FILE **fp);

/* Assembler utilities */
int is_comment(char* s);
int index_not_blank(char* str);
int starts_with(char* str, char* wanted);
int which_instructive(char *str);
int is_directive(char* line_buf);
int has_label(char** str, char** label);
int is_reg(char* token);
int symbol_is_illegal(char* symbol);
int extract_addr(char* arg);
int extract_command(char* command);
int legal_immediate_addr(char* arg);
int extract_one_argument(char* line_buf, char** arg1);
int extract_two_arguments(char* line_buf, char** arg1, char** arg2);
int compute_required_length(int opcode, char* line_buf, int *length);
int update_data_info(char* line_buf, int* DC, Info_node** info_table);
int update_string_info(char* line_buf, int* DC, Info_node** info_table);
int illegal_quotation_marks(char* line_buf);
char* error_message(int error_code);

#endif
