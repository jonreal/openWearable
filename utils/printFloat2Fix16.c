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

  float in = strtod(argv[1],NULL);
  fix16_t convert = fix16_from_float(in);
  float reconvert = fix16_to_float(convert);

  printf("Floating point: %4.8f\n"
         "Fix16: 0x%X\n"
         "Floating point: %4.8f\n",
         in, convert, reconvert);
  return 0;
}
