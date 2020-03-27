#ifndef __SECOND_PASS_H__
#define __SECOND_PASS_H__

#include "info_list.h"
#include "symbol_list.h"
#include "utilities.h"
#include <stdio.h>

int second_pass(char *filename, int IC, int DC,
		Symbol_node* symbol_head, Info_node* info_head);
int open_all_files(char* filename, FILE** fp, FILE** fp_ob,
		   FILE** fp_ent, FILE** fp_ext, char** filename_ob,
		   char** filename_ent, char** filename_ext);
int extract_command(char* command);
int second_process_line(char* line_buf, Symbol_node* symbol_head, FILE* fp_ob,
			FILE* fp_ent, FILE* fp_ext, int line_num,
			int* current_ob_line);
int second_handle_entry(char* line_buf, Symbol_node* symbol_head, FILE* fp_ent,
			int line_num);
void memory_word_to_fp_ob(int memory_word, FILE* fp_ob, int* current_ob_line);
int second_handle_instructive(int line_num, char* instruction, char* line_buf,
			      FILE* fp_ob, FILE* fp_ext,
			      Symbol_node* symbol_head, int* current_ob_line);
void encode_opcode(int *memory_word, int opcode);
void encode_addr(int *memory_word, int addr1, int addr2);
void encode_A_field(int *memory_word);
void encode_R_field(int *memory_word);
void encode_E_field(int *memory_word);
int encode_arg(int *memory_word,char* arg,int addr, Symbol_node* symbol_head,
	       int line_num, int is_destination, FILE* fp_ext,
	       int current_ob_line);
void append_info_table_to_fp_ob(FILE* fp_ob,Info_node* info_head,
				int* current_ob_line);
void print_memory_word(int memory_word);

#endif
