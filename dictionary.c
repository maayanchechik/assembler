#include "assembler.h"
#include <stdio.h>

enum errors{ERR_SYMBOL_EMPTY, 
	    ERR_SYMBOL_ILLEGAL_CHAR,
	    ERR_SYMBOL_SAVED_WORD,
	    ERR_SYMBOL_TOO_LONG,
	    ERR_SYMBOL_THEN_BLANK,
	    ERR_SYMBOL_THEN_ENTRY,
	    ERR_SYMBOL_THEN_EXTERN,
	    ERR_INSTR_ARG_EMPTY,
	    ERR_INSTR_DESTINATION_IMMEDIATE,
	    ERR_INSTR_EXCESSIVE_TEXT,
	    ERR_INSTR_ENDS_WITH_COMMA,
	    ERR_INSTR_ILLEGAL_OPCODE,
	    ERR_STRING_ILLEGAL_QUOTATION,
	    ERR_STRING_ILLEGAL_CHAR,
	    ERR_DATA_EMPTY,
	    ERR_DATA_ENDS_WITH_COMMA,
	    ERR_DATA_NO_COMMA,
	    ERR_DATA_ILLEGAL_CHAR,
	    ERR_DATA_DOUBLE_COMMA,
	    ERR_DATA_OUT_OF_RANGE,
	    ERR_COMMAND_UNKNOWN};
