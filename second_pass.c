#define _GNU_SOURCE

#include "lists.h"
#include "second_pass.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int open_all_files(char* filename, FILE** fp, FILE** fp_ob,
		   FILE** fp_ent, FILE** fp_ext){

  char *duplicate_filename;
  duplicate_filename = (char *) malloc(strlen(filename)+3);

  
  strcpy(duplicate_filename, filename);
  (*fp) = fopen(strcat(duplicate_filename, ".as"), "r");
  if ((*fp) == NULL){
    printf("Error: the file %s.as cannot be opened for writing.\n", filename);
    return 1;
  }
  
  strcpy(duplicate_filename, filename);
  (*fp_ob) = fopen(strcat(duplicate_filename, ".ob"), "w");
  if ((*fp_ob) == NULL){
    printf("Error: the file %s.ob cannot be opened for writing.\n", filename);
    return 1;
  }

  strcpy(duplicate_filename, filename);  
  (*fp_ent) = fopen(strcat(duplicate_filename, ".ent"), "w");
  if ((*fp_ent) == NULL){
    printf("Error: the file %s.ent cannot be opened for writing.\n", filename);
    return 1;
  }

  strcpy(duplicate_filename, filename);
  (*fp_ext) = fopen(strcat(duplicate_filename, ".ext"), "w");
  if ((*fp_ext) == NULL){
    printf("Error: the file %s.ent cannot be opened for writing.\n", filename);
    return 1;
  }
  return 0;
}


int second_pass(char* filename, int IC, int DC, Symbol_node* symbol_head, Info_node* info_head){
  char* line_buf = NULL;
  size_t line_buf_size = 0;
  int line_size = 1;
  int line_num = 1;
  int current_ob_line = 100;
  int error;

  FILE* fp;
  FILE* fp_ob;
  FILE* fp_ent;
  FILE* fp_ext;
  error = open_all_files(filename, &fp, &fp_ob, &fp_ent, &fp_ext);
  if (error > 0) {
    return 1;
  }


  fprintf(fp_ob,"   %d %d\n",IC-100,DC);
  while (line_size>=0){
    line_size = getline(&line_buf, &line_buf_size, fp);
    if ((line_size>0) && (!string_blank_or_empty(line_buf))
	&& (!is_comment(line_buf))&&line_buf[0]!='\n'){
      if (line_buf[line_size-1] == '\n') {
	line_buf[line_size-1] = '\0';
      }
      printf("\nsp: line_num = %d, line_size = %d, line_buf = %s",
	     line_num, line_size, line_buf);

      if (second_process_line(line_buf, symbol_head, fp_ob, fp_ent,
			      fp_ext, line_num, &current_ob_line))
	return 1;
    }
    line_num++;
  }
  append_info_table_to_fp_ob(fp_ob, info_head, &current_ob_line);
  printf("\n");

  fclose(fp);
  fclose(fp_ob);
  fclose(fp_ent);
  fclose(fp_ext);  
  return 0;
}


int second_process_line(char* line_buf, Symbol_node* symbol_head,
			FILE* fp_ob, FILE* fp_ent, FILE* fp_ext,
			int line_num, int* current_ob_line){
  char* label;
  char* command;
  int command_type;
  int error = 0;
  /* If the line has a label, has_label() puts in line_buf the rest of
     the command line.*/
  has_label(&line_buf, &label);
  
  command = strtok_r(line_buf, " \t", &line_buf);
  command_type = extract_command(command);
  
  switch(command_type){
  case ENTRY:
    error = second_handle_entry(line_buf, symbol_head, fp_ent, line_num);
    break;
  case EXTERN:
    break;    
  case INSTRUCTIVE:
    error = second_handle_instructive(line_num, command, line_buf, fp_ob, fp_ext, symbol_head, current_ob_line);
    break;
  case DIRECTIVE:
    break;   
  default:
    /* Otherwise, the first word is unfamiliar*/
    printf("process_line: Error: line #%d starts with an unknown word %s\n",line_num, command);
    error = ERR_COMMAND_UNKNOWN;
  }
  printf("spl: line_num: %d\n", line_num);
  return error;
}

int second_handle_entry(char* line_buf, Symbol_node* symbol_head,
			FILE* fp_ent, int line_num){
  char* symbol = line_buf;
  Symbol_node* ptr;
  remove_blank_from_token(&symbol);
  ptr = find_symbol(symbol_head, symbol);
  if (ptr == NULL) {
    printf("second_handle_entry: Error: line#%d %s", line_num,
	   error_message(ERR_UNKNOWN_ENTRY_SYMBOL));
    return ERR_UNKNOWN_ENTRY_SYMBOL;
  }
  if (get_symbols_is_external(ptr)){
    printf("second_handle_entry: Error: line#%d %s", line_num,
	   error_message(ERR_EXTERNAL_ENTRY_SYMBOL));
    return ERR_EXTERNAL_ENTRY_SYMBOL;
  }
  fprintf(fp_ent,"%s %d\n",symbol, get_symbols_address(ptr));
  return 0;
}

void memory_word_to_fp_ob(int memory_word, FILE* fp_ob,
			  int* current_ob_line){
  print_memory_word(memory_word);
  /* The file is writen in 8 base. Before printing, we only keep the
     15 least-significant bits, and zero all the higher bits, by
     xomputing the bit-wise AND with 2^15-1, which has its 15 lower
     bits on and all other bits off.  */  
  fprintf(fp_ob,"%04d  %05o\n", *current_ob_line, memory_word & ((1<<15)-1));
  (*current_ob_line)++;
}

int second_handle_instructive(int line_num, char* instruction,
			      char* line_buf, FILE* fp_ob, FILE* fp_ext,
			      Symbol_node* symbol_head,
			      int* current_ob_line) { 
  int error = 0;
  int opcode = is_instructive(instruction)-1;
  char* arg1;
  char* arg2;
  int addr1 = -1;
  int addr2 = -1;
  int memory_word1 = 0;
  int memory_word2 = 0;
  int memory_word3 = 0;
  int is_destination = 0;
  int both_reg = 0;
  printf("\nshi: line_buf = [%s]\n",line_buf);
  arg1 = strsep(&line_buf, ",");
  arg2 = strsep(&line_buf, ",");
  /*  printf("shi: line_buf = %s, arg1 = %s, arg2 = %s\n",line_buf, arg1, arg2);*/
  
  remove_blank_from_token(&arg1);
  remove_blank_from_token(&arg2);
  if (arg1!=NULL && !string_blank_or_empty(arg1)){
    addr1 = extract_addr(arg1);
    printf("shi:arg1 = [%s], address #1 type = %d\n",arg1,addr1);
  }
  if (arg2!=NULL && !string_blank_or_empty(arg2)){
    addr2 = extract_addr(arg2);
    printf("shi:arg2 = [%s], address #2 type = %d\n",arg2,addr2);
  }
  both_reg = ((addr1 == 2 || addr1 == 3)&&(addr2 == 2 || addr2 == 3));
  /* Print the first word in ps.ob*/
  incode_opcode(&memory_word1, opcode);
  incode_addr(&memory_word1, addr1,addr2);
  incode_A_field(&memory_word1);
  memory_word_to_fp_ob(memory_word1, fp_ob, current_ob_line);
  /* Print the second word in ps.ob, if the command has a first argument.*/
  if (addr1 != -1){
    if (addr2 == -1)
      is_destination = 1;
    error = error || incode_arg(&memory_word2, arg1, addr1, symbol_head,
				line_num, is_destination, fp_ext, *current_ob_line);
    if (!both_reg)
      memory_word_to_fp_ob(memory_word2, fp_ob, current_ob_line);
  }
  /* Print the third word in ps.ob, if the command has a second argument.*/
  if (addr2 != -1){
    is_destination = 1;
    if (!both_reg){
    error = error || incode_arg(&memory_word3, arg2, addr2, symbol_head,
				line_num, is_destination, fp_ext, *current_ob_line);
    memory_word_to_fp_ob(memory_word3, fp_ob, current_ob_line);
    }else{
      error = error || incode_arg(&memory_word2, arg2, addr2, symbol_head,
				  line_num, is_destination, fp_ext, *current_ob_line);
      memory_word_to_fp_ob(memory_word2, fp_ob, current_ob_line);      
    }
  }
  return error;
}

void incode_opcode(int *memory_word, int opcode){
  /* Set bits 11-14 of memory_word to be 4 bits of opcode. Assume
     these bits are zero. */
  (*memory_word) |= (opcode <<11);
}
  
void incode_addr(int *memory_word, int addr1, int addr2){
  /* Assume that bits 4 to 12 of memory word are zero. */
  /* addr1 and addr2 have values in [0,3], and signify which bit
     should be turned on. */
  if (addr1 == -1){
    /* command has no 1st argument */ 
    return;
  }

  if (addr2 == -1){
    /* command has no 2nd argument. Addr1 is the destination. */
    (*memory_word) |= (1 << (addr1+3));
  } else {
    (*memory_word) |= (1 << (addr2+3));
    (*memory_word) |= (1 << (addr1+7));
  }
}

void incode_A_field(int *memory_word){
  (*memory_word) |= 1 << 2;
}  
void incode_R_field(int *memory_word){
  (*memory_word) |= 1 << 1;
}  
void incode_E_field(int *memory_word){
  (*memory_word) |= 1 << 0;
}  

int twos_complement(int num) {
  if (num > 0) {
    return num;
  } else {
    return (~(-num)) + 1;
  }
}

int incode_arg(int *memory_word,char* arg,int addr,
	       Symbol_node* symbol_head,
	       int line_num, int is_destination,
	       FILE* fp_ext, int current_ob_line){
  int num;
  char* non_digit;
  Symbol_node* ptr;
  int error = 0;

  switch(addr){
    
  case ADDR_IMMEDIATE:
    arg++;
    num = strtol(arg, &non_digit, 10);
    /* encode num as two's comlemnt then into bits 3-14 */
    (*memory_word) |= twos_complement(num) << 3;
    incode_A_field(memory_word);
    break;
    
  case ADDR_DIRECT:
    ptr = find_symbol(symbol_head, arg);
    if (ptr == NULL){
      printf("incode_arg: Error: line#%d %s",line_num, error_message(ERR_UNKNOWN_ENTRY_SYMBOL));
      error = ERR_UNKNOWN_ENTRY_SYMBOL;
    }
    num = get_symbols_address(ptr);
    /* put num into bits 3-14 */
    (*memory_word) |= num << 3;

    if (get_symbols_is_external(ptr)) {
      incode_E_field(memory_word);
      fprintf(fp_ext, "%-3s %04d\n",arg,current_ob_line);
    } else {
      incode_R_field(memory_word);
    }
    break;
    
  case ADDR_REG_INDIRECT:
    arg++;
  case ADDR_REG_DIRECT:
    incode_A_field(memory_word);
    arg++;
    num = strtol(arg, &non_digit, 10);
    if (is_destination){
      /* put num into bits 3-5 */
      (*memory_word) |= num << 3;
    }else{
      /* put num into bits 6-8 */
      (*memory_word) |= num << 6;
    }
    break;

  default:
    error = ERR_UNKNOWN;
    printf("arg = %s, address type = %d\n", arg, addr);
    printf("incode_arg: Error: line #%d %s\n", line_num, error_message(ERR_UNKNOWN));
  }
  return error;
}


void append_info_table_to_fp_ob(FILE* fp_ob, Info_node* info_head,
				int* current_ob_line){
  int memory_word = 0;
  int num;
  while(info_head != NULL){
    num = get_info_value(info_head);
    num = twos_complement(num);
    memory_word = num;
    memory_word_to_fp_ob(memory_word, fp_ob, current_ob_line);
    info_head = get_info_next(info_head);
  }
}


void print_memory_word(int memory_word){
  int i;
  printf("memory_word: %8d  ", memory_word);
  for (i = 0; i<15; i++){
    printf("%d", ((memory_word & (1<<14)) >>14));
    memory_word = memory_word << 1;
  }
  printf("\n");
}
