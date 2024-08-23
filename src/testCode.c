#include <omp.h>
#include <sampling.h>

int main() {
roofline_sampling_init("my_CARM_result.roofs", 1, ROOFLINE_CORE);
long hypothetic_flop_count = 2655619;
long hypothetic_byte_count = 2099429;

#pragma omp parallel 
{
#define NI 100
#define NJ 110
#define NK 120

float A[NI][NK];
float B[NK][NJ];
float C[NI][NJ];
float alpha = 1;
float beta = 1;

void * sample = roofline_sampling_start(0, hypothetic_flop_count, hypothetic_byte_count);

  for (int i = 0; i < NI; i++) {
    for (int j = 0; j < NJ; j++)
      C[i][j] *= beta;
    for (int k = 0; k < NK; k++)
      for (int j = 0; j < NJ; j++)
        C[i][j] += alpha * A[i][k] * B[k][j];
  }



roofline_sampling_stop (sample, "./testCode");

}

roofline_sampling_fini();

return 0;
}

