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

  iir_coeff_t coeff;
  iir_buff_t filter;
  coeff.N = 1;
  coeff.b[0] = fix16_from_float(0.0185029748827219);
  coeff.b[1] = fix16_from_float(0.0185029748827219);
  coeff.a[0] = fix16_from_float(1.0);
  coeff.a[1] = fix16_from_float(-0.9629940390586853);

  iirInit(&filter, coeff);

  // Read in input file data
  FILE* fp = fopen(file, "r");
  FILE* fout = fopen("./out", "w");

  // First element lenght
  fscanf(fp, "%u\n", dataLength);

  // make fix16_t array of data
  fix16_t data[length];
  fix16_t out[length];
  float v;

  for(int i=0; i<length; i++){
    fscanf(fp, "%f\n", &v);
    data[i] = fix16_from_float(v);
    out[i] = iirFilt(&filter, coeff, data[i]);
    fprintf(fout,"%f\t%f\t%f\n",
            v, fix16_to_float(data[i]),fix16_to_float(out[i]));
  }
  fclose(fp);
  return 0;
}


