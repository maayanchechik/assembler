#ifndef __FIRST_PASS_H__
#define __FIRST_PASS_H__

#include "symbol_list.h"
#include "info_list.h"
#include "utilities.h"
#include <stdio.h>

int first_pass(char * filename, int* IC, int* DC, Symbol_node** symbol_table,
	       Info_node** info_table);
int process_line(char* line_buf, int* IC, int* DC, Symbol_node** symbol_table,
		 Info_node** info_table, int line_num);
int handle_entry(int line_has_label, int line_num, char* line_buf);
int handle_extern(int line_has_lable, int line_num, char* line_buf,
		  Symbol_node** symbol_table);
int  handle_instructive(int line_has_lable, int line_num, char* instruction,
			char* line_buf, char* label, Symbol_node** symbol_table,
			int *IC);
int handle_directive(int line_has_label, int line_num,  char* direction,
		     char* line_buf, char* label, Symbol_node** symbol_table,
		     Info_node** info_table, int* DC);

#endif
