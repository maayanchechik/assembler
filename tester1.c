#include <stdio.h>
#include <stdlib.h>
int main(){
  char* arg = "*r23";
  int num;
  char* non_digit;
  arg++;
  arg++;
  num = strtol(arg, &non_digit, 10);
  printf("%d\n", num);
  return 0;
}
