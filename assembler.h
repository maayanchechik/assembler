#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H_
#include "lists.h"_
#include "utilities.h"


/********first*pass**************************************************************/
enum commands {ENTRY, EXTERN, INSTRUCTIVE, DIRECTIVE, ILLEGAL_COMMAND_TYPE};

int first_pass(FILE* fp, int* IC, int* DC, Symbol_node** symbol_table,
	       Info_node** info_table);
int process_line(char* line_buf, int* IC, int* DC, Symbol_node** symbol_table,
		 Info_node** info_table, int line_num);
int extract_command(char* command);
int handle_entry(int line_has_label, int line_num);
int handle_extern(int line_has_lable, int line_num, char* line_buf, int* DC,
		  Symbol_node** symbol_table);
int  handle_instructive(int line_has_lable, int line_num, char* instruction,
			char* line_buf, char* label, Symbol_node** symbol_table,
			int *IC);
int handle_directive(int line_has_lable, int line_num, char* line_buf,
		     char* label, Symbol_node** symbol_table,
		     Info_node** info_table, int *DC, char direction);


#endif
