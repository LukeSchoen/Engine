#include "Accelerator.h"

// System includes
#include <stdio.h>

// CUDA runtime
#include <cuda_runtime.h>
// CUDA Helper functions

#include "helper_functions.h"
#include "helper_cuda.h"
#include "device_launch_parameters.h"

__global__ void matrixMulCUDA(float *input, float *output, float *matrix)
{
  int ID = (blockIdx.x * blockDim.x + threadIdx.x);
  //output[ID] = input[ID] + 1;
}

Accelerator::Accelerator()
{
  int devID = 0;

  cudaDeviceProp deviceProp;
  assert(!cudaGetDevice(&devID));
  assert(!cudaGetDeviceProperties(&deviceProp, devID));
  printf("Using GPU Device %d: \"%s\" with compute capability %d.%d\n\n", devID, deviceProp.name, deviceProp.major, deviceProp.minor);

  printf("integrated %d\n", deviceProp.integrated);
  printf("totalGlobalMem %d\n", deviceProp.totalGlobalMem);
  printf("clockRate %d\n", deviceProp.clockRate);
  printf("multiProcessorCount %d\n", deviceProp.multiProcessorCount);
  printf("ThreadsPerMultiProcessor %d\n", deviceProp.maxThreadsPerMultiProcessor);
  printf("ThreadsPerBlock%d \n", deviceProp.maxThreadsPerBlock);
  printf("warpSize %d\n", deviceProp.warpSize);
  printf("regsPerBlock %d\n", deviceProp.regsPerBlock);

  // Use a larger block size for Fermi and above
  block_size = (deviceProp.major < 2) ? 16 : 32;
  multiProcessors = deviceProp.multiProcessorCount;
  maxThreadsPerMultiProcessor = deviceProp.maxThreadsPerMultiProcessor;
}
 
int Accelerator::OptimalThreadCount()
{
  return multiProcessors * maxThreadsPerMultiProcessor;
}

void Accelerator::MatVec(const float *input, float *output, const float *matrix, int64_t numVerts)
{
  // Allocate device memory
  float *inputGPU;
  float *outputGPU;
  float *matrixGPU;
  assert(!cudaMalloc(&inputGPU, numVerts * 3 * sizeof(float)));
  assert(!cudaMalloc(&outputGPU, numVerts * 3 * sizeof(float)));
  assert(!cudaMalloc(&matrixGPU, 16 * sizeof(float)));

  // copy host memory to device
  assert(!cudaMemcpy(matrixGPU, matrix, sizeof(float) * 16, cudaMemcpyHostToDevice));
  assert(!cudaMemcpy(inputGPU, input, sizeof(float) * 16, cudaMemcpyHostToDevice));

  // Setup execution parameters
  matrixMulCUDA<<<numVerts / 256, 256>>>(matrixGPU, inputGPU, outputGPU);

  //cudaDeviceSynchronize();

  assert(!cudaMemcpy(output, outputGPU, numVerts * 3 * sizeof(float), cudaMemcpyDeviceToHost));

  // Clean up memory
  cudaFree(matrixGPU);
  cudaFree(inputGPU);
  cudaFree(outputGPU);
}
