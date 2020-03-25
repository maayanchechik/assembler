/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
  char* token;
  char* line_buf = " 12 2";
  token = strsep(&line_buf, " ");
  printf("token = %s\n",token);
  token = strsep(&line_buf, " ");
  printf("token = %s\n",token);
  return 0;
  }*/
#include <stdio.h>
#include <string.h>
#include "tester.h"

struct info{
  int address;
  int value;
  struct info *next;
};


enum errors{ERR0,ERR1,ERR2,ERR3}; 

/* This function goes over the end of an input string and retruns
   whether there is a comma after the text.*/
int string_ends_with_comma_and_blanks(char* s){
  int i;
  /* If the string is empty or null it cannot end with a comma.*/
  if(s == NULL|| strlen(s) == 0){
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

int string_blank_or_empty(char* s){
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

int extract_two_arguments(char* line_buf, char** arg1, char** arg2) { 
 if(string_ends_with_comma_and_blanks(line_buf))
    return 4;  
  *arg1 = strsep(&line_buf, ",");
  if (arg1 == NULL || string_blank_or_empty(*arg1))
    return 1;
  *arg2 = strsep(&line_buf, ",");
  if (arg2 == NULL || string_blank_or_empty(*arg2))
    return 2;
  if(!string_blank_or_empty(line_buf))
    return 3;
  return 0;
}

int main()
{
  /*
  char *line_buf,*arg1,*arg2;
  int error = 0;
    line_buf = strdup("12,        2");
    error = extract_two_arguments(line_buf, &arg1, &arg2);
    printf("errorcode = %d\n",error);
    int error = ERR1;
    printf("error = %d\n",error);
    return 0;
  
    char* message;
    message = "hey\n";
    printf("%s",message);

  struct info **head, *ptr;
*/
  int i;
  int num = 4;
  int memory_word[15];
  for(i = 11; i > 0; i--, num/=2){
    memory_word[i] = num%2;
  }
  memory_word[12] = 1;
  memory_word[13] = 0;
  memory_word[14] = 0;
  for(i = 0; i<15; i++){
    printf("%d\n", memory_word[i]);
  }
  
}
