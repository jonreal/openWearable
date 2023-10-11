#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "../library/include/fix16.h"

int main(int argc, char **argv)
{

  // Command Line Inputs
  if(argc != 2){
    printf("error: wrong number of arguments\n");
    return -1;
  }

  fix16_t in = strtod(argv[1],NULL);
  float convert = fix16_to_float(in);
  fix16_t reconvert = fix16_from_float(convert);

  printf("Fix16: 0x%X\n"
         "Floating point: %f\n"
         "Fix16: 0x%X\n",
         in, convert, reconvert);
  return 0;
}
