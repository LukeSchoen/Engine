#include "Accelerator.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
  const int64_t numVerts = 16 * 1024 * 1024; // Vector Count
  float * __restrict input = new float[numVerts * 3]; // Input
  float * __restrict output = new float[numVerts * 3]; // Output
  float matrix[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };  // Matrix

  printf("Making Random Data!\n");
  for (int64_t i = 0; i < numVerts * 3; i++) input[i] = (float)rand() / RAND_MAX; // Random Data

  printf("\nCPU Started!\n");
  auto startTime = clock();
  // CPU Version
  float * __restrict m = matrix;
  if (true)
    for (int64_t l = 0; l < 20; l++)
    {
      float * __restrict i = input;
      float * __restrict o = output;
      while (o < output + numVerts * 3)
      {
        *(o + 0) = *(i + 0) * *(m + 0) + *(i + 1) * *(m + 1) + *(i + 2) * *(m + 2) + 1 * *(m + 3);
        *(o + 1) = *(i + 0) * *(m + 4) + *(i + 1) * *(m + 5) + *(i + 2) * *(m + 6) + 1 * *(m + 7);
        *(o + 2) = *(i + 0) * *(m + 8) + *(i + 1) * *(m + 9) + *(i + 2) * *(m + 10) + 1 * *(m + 11);
        float w = *(i + 0) * *(m + 12) + *(i + 1) * *(m + 13) + *(i + 2) * *(m + 14) + 1 * *(m + 15);
        i += 3;
        o += 3;
      }
    }

  printf("CPU Finished!\n");
  printf( "In %d ms.\n", clock() - startTime);

  printf("\n\nGPU Initializing...\n");
  Accelerator accelerator;
  printf("\nGPU Starting!\n");
  printf("%f\n", *input);

  // GPU Version
  startTime = clock();

  if (true)
    for (int64_t l = 0; l < 1; l++)
      accelerator.MatVec(input, output, matrix, numVerts);
  printf("%f\n", *output);
  printf("GPU Finished!\n");
  printf("In %d ms.\n", clock() - startTime);
  getchar();
}