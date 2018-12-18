/* Copyright 2017 Jonathan Realmuto

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
=============================================================================*/

// Test IIR and nonlinear Bayes Filter (fix16 versions)

#include <stdio.h>

#include "fix16.h"
#include "filter.h"
#include "filtcoeff.h"

volatile uint32_t* debug_buff;


int main(int argc, char **argv)
{
  char* file;
  int dataLength;

  if(argc == 1){
    printf("Error: no input file");
    return -1;
  }
  else {
    file = argv[1];
    printf("Input file: %s\n",file);
  }


  printf("\n Testing Filters\n");

  iir_filt_t* lp_1_5Hz_1 = FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a);

  // Read in input file data
  FILE* fp = fopen(file, "r");
  FILE* fout = fopen("./out.txt", "w");

  // First element lenght
  fscanf(fp, "%u\n", &dataLength);

  // make fix16_t array of data
  fix16_t data[dataLength];
  fix16_t out[dataLength];
  float v;

  for(int i=0; i<dataLength; i++){
    fscanf(fp, "%f\n", &v);
    data[i] = fix16_from_float(v);
    out[i] = FiltIir(data[i],lp_1_5Hz_1);
    fprintf(fout,"%f\t%f\t%f\n",
            v, fix16_to_float(data[i]),fix16_to_float(out[i]));
  }
  fclose(fp);
  return 0;
}


