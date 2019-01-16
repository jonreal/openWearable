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
#include <sys/time.h>

#include "fix16.h"
#include "filter.h"
#include "filtcoeff.h"

volatile uint32_t* debug_buff;

fix16_t absV(fix16_t v);

const int n = 1000;
const float alpha = 0.1;
const float beta = 0.1;
const float maxEmg = 1;
const float scale = 4;

int main(int argc, char **argv)
{
  char* file;
  int dataLength;
  uint32_t buffer[10];
  debug_buff = &buffer;

  if(argc == 1){
    printf("Error: no input file");
    return -1;
  }
  else {
    file = argv[1];
    printf("Input file: %s\n",file);
  }

  printf("\n Testing Filters\n");
 // iir_filt_t* lp_1_5Hz_1 = FiltIirInit(1, k_lp_1_5Hz_b, k_lp_1_5Hz_a);
  nlb_filt_t* nlb_filt = FiltNlbInit(n, fix16_from_float(1000.0),
                                        fix16_from_float(maxEmg),
                                        fix16_from_float(alpha),
                                        fix16_from_float(beta));
  // Read in input file data
  FILE* fp = fopen(file, "r");
  FILE* fout = fopen("./out.txt", "w");

  // First element lenght
  fscanf(fp, "%u\n", &dataLength);

  // make fix16_t array of data
  fix16_t data[dataLength];
  fix16_t out[dataLength];
  double mean_ex_t = 0.0;
  float v;

//  dataLength = 10;

  struct timeval  tv1, tv2;
  for(int i=0; i<dataLength; i++){
    fscanf(fp, "%f\n", &v);

   // printf("\n-----\n");
   // printf("%d\n",i+1);

    // FIR
    //out[i] = FiltIir(data[i],lp_1_5Hz_1);

    // Bayesian
    data[i] = fix16_from_float(v);
    data[i] = absV(fix16_ssub(data[i],fix16_from_int(890)));
    data[i] = fix16_sdiv(data[i],fix16_from_int(507));
    data[i] = fix16_smul(fix16_smul(fix16_from_int(scale),data[i]),
                             fix16_from_float(maxEmg));
    if(data[i] > fix16_from_float(maxEmg))
      data[i] = fix16_from_float(maxEmg);

   // printf("\nemg = %f\n",fix16_to_float(data[i]));

    gettimeofday(&tv1, NULL);
    out[i] = FiltNlb(data[i],nlb_filt);
    gettimeofday(&tv2, NULL);
    mean_ex_t = mean_ex_t + (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
         (double) (tv2.tv_sec - tv1.tv_sec);

    fprintf(fout,"%f\t%f\t%f\n",
            v, fix16_to_float(data[i]),fix16_to_float(out[i]));
  }
  fclose(fp);
  mean_ex_t = mean_ex_t / ((double)dataLength);
  printf("\nmean execution time = %f10\n", mean_ex_t);



  return 0;
}

fix16_t absV(fix16_t v){
  if (v < 0)
    return -v;
  return v;
}


