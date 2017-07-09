#include "Accelerator.h"

// System includes
#include <stdio.h>

// CUDA runtime
#include <cuda_runtime.h>

#include "helper_functions.h"
#include "helper_cuda.h"
#include "device_launch_parameters.h"

__global__ void matrixMulCUDA(float *matrix, float *input, float *output)
{
  int ID = (blockIdx.x * blockDim.x + threadIdx.x) * 3;
  output[ID + 0] = input[ID] * matrix[0] + input[ID + 1] * matrix[1] + input[ID + 2] * matrix[2] + 1 * matrix[3];
  output[ID + 1] = input[ID] * matrix[4] + input[ID + 1] * matrix[5] + input[ID + 2] * matrix[6] + 1 * matrix[7];
  output[ID + 2] = input[ID] * matrix[8] + input[ID + 1] * matrix[9] + input[ID + 2] * matrix[10] + 1 * matrix[11];
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
  assert(!cudaMemcpy(inputGPU, input, numVerts * 3 * sizeof(float), cudaMemcpyHostToDevice));

  // Setup execution parameters
  for (int64_t i = 0; i < 1; i++)
    cudaError_t code = matrixMulCUDA<<<numVerts / 256, 256>>>(matrixGPU, inputGPU, outputGPU);
    if(code != cudaSuccess)
    {

    }
  

if ()
{
  std::stringstream ss;
  ss << file << "(" << line << ")";
  std::string file_and_line;
  ss >> file_and_line;
  throw thrust::system_error(code, thrust::cuda_category(), file_and_line);
}

  //cudaDeviceSynchronize();

  assert(!cudaMemcpy(output, outputGPU, numVerts * 3 * sizeof(float), cudaMemcpyDeviceToHost));

  // Clean up memory
  cudaFree(matrixGPU);
  cudaFree(inputGPU);
  cudaFree(outputGPU);
}
