#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "../app/include/fix16.h"

int main(int argc, char **argv)
{

  // Command Line Inputs
  if(argc != 2){
    printf("error: wrong number of arguments\n");
    return -1;
  }

  printf("Floating point: %4.3f\n"
         "Fix16: 0x%X\n", strtod(argv[1],NULL),
          fix16_from_float(strtod(argv[1],NULL)));
  return 0;
}
