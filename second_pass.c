#define _GNU_SOURCE

#include "info_list.h"
#include "symbol_list.h"
#include "second_pass.h"
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_NUM_BITS 15

/* This function receives a filename prefix, a pointer to an output
   filename, the suffix, the permition and a pointer to a file
   pointer. The function updates the file_out to start with the prefix
   and end with the suffix. It also opens file_out and makes fp the
   pointer to that file. The function returns the appropriate error
   code.*/
int open_one_file(char* prefix, char** file_out, const char* suffix,
		  const char* permission, FILE **fp) {
  
  /* First, build the specific filename from prefix and suffix */
  (*file_out) = (char *) malloc(strlen(prefix) + strlen(suffix));
  if (*file_out == 0) {
    printf("Failed allocating memory fo file name %s\n", suffix);
    return ERR_FILE;
  }
  strcpy((*file_out), prefix);
  strcat((*file_out), suffix);

  (*fp) = fopen((*file_out), permission);
  if ((*fp) == NULL) {
    printf("Error: the file %s%s cannot be opened for %s.\n",
	   prefix, suffix, permission);
    return ERR_FILE;
  }
  return NO_ERROR;
}

/* This function receives a filename prefix, pointers to file pointers
   of the .ob, .ent and .ext files, and pointers to the filenames of
   the .ob, .ent and .ext files. It opens each file and updates their
   file pointers and names. The function returns */
int open_all_files(char* prefix, FILE** fp_as, FILE** fp_ob,
		   FILE** fp_ent, FILE** fp_ext, char** filename_ob,
		   char** filename_ent, char** filename_ext) {
  /* It is important to dynamically allocate memory for the filenames
     because strcat does not allocate memory. */
  char* filename_as;
  if (open_one_file(prefix, &filename_as, ".as", "r", fp_as)) {
    return ERR_FILE;
  }
  if (open_one_file(prefix, filename_ob, ".ob", "w", fp_ob)) {
    return ERR_FILE;
  }
  if (open_one_file(prefix, filename_ent, ".ent", "w", fp_ent)) {
    return ERR_FILE;
  }
  if (open_one_file(prefix, filename_ext, ".ext", "w", fp_ext)) {
    return ERR_FILE;
  } 
  return NO_ERROR;
}

/* This function receives the file prefix passed to main's arguments,
   a pointer to the instruction counter and the data counter, a
   pointer to the head of the symbol table and to the head of the info
   table. It opens and reades the .as file, line by line, preforms the
   second pass, updating the .ob, .ent and .ext files according to the
   code file. The function returns the appropriate error code for the
   given file, and if an error is found the function deletes the
   output files.*/
int second_pass(char* prefix, int IC, int DC, Symbol_node* symbol_head,
		  Info_node* info_head) {
  char* line_buf = NULL;
  size_t line_buf_size = 0;
  int line_size = 1;
  int line_num = 1;
  int current_ob_line = 100;
  int error = 0;
  char* filename_ob;
  char* filename_ent;
  char* filename_ext;
  FILE* fp_as;
  FILE* fp_ob;
  FILE* fp_ent;
  FILE* fp_ext;
  error = open_all_files(prefix, &fp_as, &fp_ob, &fp_ent, &fp_ext, &filename_ob,
			 &filename_ent, &filename_ext);
  if (error > 0) {
    return error;
  }

  fprintf(fp_ob,"   %d %d\n",IC-100,DC);
  while (line_size>=0) {
    line_size = getline(&line_buf, &line_buf_size, fp_as);
    if ((line_size>0) && (!string_blank_or_empty(line_buf))
	&& (!is_comment(line_buf))&&line_buf[0]!='\n') {
      if (line_buf[line_size-1] == '\n') {
	line_buf[line_size-1] = '\0';
      }
      printf("\nsp: line_num = %d, line_size = %d, line_buf = %s",
	     line_num, line_size, line_buf);

      error = second_process_line(line_buf, symbol_head, fp_ob, fp_ent,
				  fp_ext, line_num, &current_ob_line) || error;
    }
    line_num++;
  }
  append_info_table_to_fp_ob(fp_ob, info_head, &current_ob_line);

  printf("\n");

  fclose(fp_as);
  fclose(fp_ob);
  fclose(fp_ent);
  fclose(fp_ext);

  if (error) {
    if (remove(filename_ob))
      printf("Unable to delete the file ps.ob\n");
    if (remove(filename_ent))
      printf("Unable to delete the file ps.ent\n");
    if (remove(filename_ext))
      printf("Unable to delete the file ps.ext\n");
  }
  return error;
}

/* This function receives the current line buffer, a pointer to the
   head of the symbol table, a pointer to each output file, the
   current line number in the .as file and the current line number in
   the .ob file. It processes the line and updates current_ob_line and
   the output files according to the received line_buf. The function
   returns the appropriate error code for the given line buffer.*/
int second_process_line(char* line_buf, Symbol_node* symbol_head,
			FILE* fp_ob, FILE* fp_ent, FILE* fp_ext,
			int line_num, int* current_ob_line) {
  char* label;
  char* command;
  int command_type;
  int error = 0;
  /* If the line has a label, has_label() puts in line_buf the rest of
     the command line.*/
  has_label(&line_buf, &label);
  
  command = strtok_r(line_buf, " \t", &line_buf);
  command_type = extract_command(command);
  
  switch(command_type) {
  case ENTRY:
    error = second_handle_entry(line_buf, symbol_head, fp_ent, line_num);
    break;
  case EXTERN:
    break;    
  case INSTRUCTIVE:
    error = second_handle_instructive(line_num, command, line_buf, fp_ob,
				      fp_ext, symbol_head, current_ob_line);
    break;
  case DIRECTIVE:
    break;   
  default:
    /* Otherwise, the first word is unfamiliar*/
    printf("process_line: Error: line #%d starts with an unknown word %s\n",
	   line_num, command);
    error = ERR_COMMAND_UNKNOWN;
  }
  printf("spl: line_num: %d\n", line_num);
  return error;
}

/* This function is called in case the current line's command is
   [.entry]. It receives the current line buffer, the symbol head, a
   pointer to the .ent file and the current line number. The function
   processes line_buf, updates the .ent file according to the symbol
   in line_buf and returns the appropriate error code. */
int second_handle_entry(char* line_buf, Symbol_node* symbol_head,
			FILE* fp_ent, int line_num) {
  char* symbol = line_buf;
  Symbol_node* ptr;
  remove_blank_from_token(&symbol);
  ptr = find_symbol(symbol_head, symbol);
  if (ptr == NULL) {
    printf("second_handle_entry: Error: line#%d %s", line_num,
	   error_message(ERR_UNKNOWN_ENTRY_SYMBOL));
    return ERR_UNKNOWN_ENTRY_SYMBOL;
  }
  if (get_symbols_is_external(ptr)) {
    printf("second_handle_entry: Error: line#%d %s", line_num,
	   error_message(ERR_EXTERNAL_ENTRY_SYMBOL));
    return ERR_EXTERNAL_ENTRY_SYMBOL;
  }
  fprintf(fp_ent,"%s %d\n",symbol, get_symbols_address(ptr));
  return 0;
}

/* This function receives a memory word, the pointer to the .ob file
   and the curent line in the .ob file. The function updates the .ob
   file with the memory word, and apdates current_ob_line. */
void memory_word_to_fp_ob(int memory_word, FILE* fp_ob,
			  int* current_ob_line) {
  print_memory_word(memory_word);
  /* The file is writen in 8 base. Before printing, we only keep the
     15 least-significant bits, and zero all the higher bits, by
     xomputing the bit-wise AND with 2^15-1, which has its 15 lower
     bits on and all other bits off.  */  
  fprintf(fp_ob,"%04d  %05o\n", *current_ob_line, memory_word & ((1<<15)-1));
  (*current_ob_line)++;
}

/* This function is called in case the current line's command is an
   instruction. It receives the current line number, the instruction,
   the current line buffer without the instruction, a pointer to the
   .ob and .ext files, the head of the symbol table and the current
   line in the .ob file. The function encodes the current line in the
   .ob file and updates the .ext file. If it encounters an error, it
   returns the appropriate error code.*/
int second_handle_instructive(int line_num, char* instruction,
			      char* line_buf, FILE* fp_ob, FILE* fp_ext,
			      Symbol_node* symbol_head,
			      int* current_ob_line) { 
  int error = NO_ERROR;
  int opcode = which_instructive(instruction);
  char* arg1;
  char* arg2;
  int addr1 = -1;
  int addr2 = -1;
  /* Memory words are implemented in this program using the least
     significant bits of an integer. */
  int memory_word1 = 0;
  int memory_word2 = 0;
  int memory_word3 = 0;
  int is_destination = 0;
  int both_reg = 0;
  printf("\nshi: line_buf = [%s]\n",line_buf);
  arg1 = strsep(&line_buf, ",");
  arg2 = strsep(&line_buf, ",");
  
  remove_blank_from_token(&arg1);
  remove_blank_from_token(&arg2);
  if (arg1!=NULL && !string_blank_or_empty(arg1)) {
    addr1 = extract_addr(arg1);
    printf("shi:arg1 = [%s], address #1 type = %d\n",arg1,addr1);
  }
  if (arg2!=NULL && !string_blank_or_empty(arg2)) {
    addr2 = extract_addr(arg2);
    printf("shi:arg2 = [%s], address #2 type = %d\n",arg2,addr2);
  }
  both_reg = ((addr1 == 2 || addr1 == 3)&&(addr2 == 2 || addr2 == 3));
  
  /* Print the first word in file.ob*/
  encode_opcode(&memory_word1, opcode);
  encode_addr(&memory_word1, addr1,addr2);
  encode_A_field(&memory_word1);
  memory_word_to_fp_ob(memory_word1, fp_ob, current_ob_line);
  
  /* Print the second word in file.ob, if the command has a first argument.*/
  if (addr1 != -1) {
    if (addr2 == -1)
      is_destination = 1;
    error = encode_arg(&memory_word2, arg1, addr1, symbol_head,
		       line_num, is_destination, fp_ext,*current_ob_line);
    if (!both_reg)
      memory_word_to_fp_ob(memory_word2, fp_ob, current_ob_line);
  }
  
  /* Print the third word in file.ob, if the command has a second argument.*/
  if (addr2 != -1) {
    is_destination = 1;
    if (!both_reg) {
    error = encode_arg(&memory_word3, arg2, addr2, symbol_head, line_num,
		       is_destination, fp_ext, *current_ob_line) || error;
    memory_word_to_fp_ob(memory_word3, fp_ob, current_ob_line);
    }else{
      error = encode_arg(&memory_word2, arg2, addr2, symbol_head, line_num,
			 is_destination, fp_ext, *current_ob_line) || error;
      memory_word_to_fp_ob(memory_word2, fp_ob, current_ob_line);      
    }
  }
  return error;
}

/* This function receives a pointer to a memory word and the opcode,
   and encodes the opcode in proper bits of the memory word. */
void encode_opcode(int *memory_word, int opcode) {
  /* Set bits 11-14 of memory_word to be 4 bits of opcode. Assume
     these bits are zero. */
  (*memory_word) |= (opcode <<11);
}
  
/* This function receives a pointer to a memory word and the addresing
   types of two arguments, and encodes the addressing type in proper
   bits of the memory word. */
void encode_addr(int *memory_word, int addr1, int addr2) {
  /* Assume that bits 4 to 12 of memory word are zero. */
  /* addr1 and addr2 have values in [0,3], and signify which bit
     should be turned on. */
  if (addr1 == -1) {
    /* command has no 1st argument */ 
    return;
  }

  if (addr2 == -1) {
    /* command has no 2nd argument. Addr1 is the destination. */
    (*memory_word) |= (1 << (addr1+3));
  } else {
    (*memory_word) |= (1 << (addr2+3));
    (*memory_word) |= (1 << (addr1+7));
  }
}

/* This function receives a pointer to a memory word, and it encodes
   the A fied flag in proper bits of the memory word. */
void encode_A_field(int *memory_word) {
  (*memory_word) |= 1 << 2;
}  

/* This function receives a pointer to a memory word, and it encodes
   the R fied flag in proper bits of the memory word. */
void encode_R_field(int *memory_word) {
  (*memory_word) |= 1 << 1;
}

/* This function receives a pointer to a memory word, and it encodes
   the E fied flag in proper bits of the memory word. */
void encode_E_field(int *memory_word) {
  (*memory_word) |= 1 << 0;
}  

/* This function receives a number and returns the two's compliment of
   it. */
int twos_complement(int num) {
  if (num > 0) {
    return num;
  } else {
    return (~(-num)) + 1;
  }
}

/* This function receives a pointer to a memory word, an argument, the
   head of the symbol table, the current line number, a flag that
   indicates whether the argument is a destination argument, a pointer
   to the .ext file and the current line number in the .ob file. The
   function encodes the arguments information into the memory word
   according to its type. If the argument is an external symbol the
   function updates the .ext file. The fumction returns the
   appropriate error code to the argument's information. */
int encode_arg(int *memory_word,char* arg,int addr, Symbol_node* symbol_head,
	       int line_num, int is_destination, FILE* fp_ext,
	       int current_ob_line) {
  int num;
  char* non_digit;
  Symbol_node* ptr;
  int error = 0;

  switch(addr) {
    
  case ADDR_IMMEDIATE:
    arg++;
    num = strtol(arg, &non_digit, 10);
    /* encode num as two's comlemnt then into bits 3-14 */
    (*memory_word) |= twos_complement(num) << 3;
    encode_A_field(memory_word);
    break;
    
  case ADDR_DIRECT:
    ptr = find_symbol(symbol_head, arg);
    if (ptr == NULL) {
      printf("encode_arg: Error: line#%d the symbol [%s] is %s",line_num,
	     arg, error_message(ERR_UNKNOWN_SYMBOL));
      error = ERR_UNKNOWN_ENTRY_SYMBOL;
      break;
    }
    num = get_symbols_address(ptr);
    /* put num into bits 3-14 */
    (*memory_word) |= num << 3;

    if (get_symbols_is_external(ptr)) {
      encode_E_field(memory_word);
      fprintf(fp_ext, "%-3s %04d\n",arg,current_ob_line);
    } else {
      encode_R_field(memory_word);
    }
    break;
    
  case ADDR_REG_INDIRECT:
    arg++;
  case ADDR_REG_DIRECT:
    encode_A_field(memory_word);
    arg++;
    num = strtol(arg, &non_digit, 10);
    if (is_destination) {
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
    printf("encode_arg: Error: line #%d %s\n", line_num,
	   error_message(ERR_UNKNOWN));
  }
  return error;
}

/* This function receives a pointer to the .ob file, the head of the
   info table and the current line in the .ob file. The function
   encodes each info node to a memory word and append it to the .ob
   file.*/
void append_info_table_to_fp_ob(FILE* fp_ob, Info_node* info_head,
				int* current_ob_line) {
  int memory_word = 0;
  int num;
  while (info_head != NULL) {
    num = get_info_value(info_head);
    num = twos_complement(num);
    memory_word = num;
    memory_word_to_fp_ob(memory_word, fp_ob, current_ob_line);
    info_head = get_info_next(info_head);
  }
}

/* This function receives a memory word and printd it for debugging
   porpouses.*/
void print_memory_word(int memory_word) {
  int i;
  printf("memory_word: %8d  ", memory_word);
  for (i = 0; i< MEMORY_NUM_BITS; i++) {
    printf("%d", ((memory_word & (1<<(MEMORY_NUM_BITS-1))) >>(MEMORY_NUM_BITS-1)));
    memory_word = memory_word << 1;
  }
  printf("\n");
}
