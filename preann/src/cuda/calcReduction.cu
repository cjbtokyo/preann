#undef _GLIBCXX_ATOMIC_BUILTINS
#undef _GLIBCXX_USE_INT128

#include "cuda.h"

__device__
unsigned device_min(unsigned a, unsigned b)
{
    if (a < b)
        return a;
    return b;
}

template <unsigned int blockSize, BufferType inputType>
__global__
void SumConnectionsKernel(void* inputPtr, unsigned input_size, unsigned output_size, void* weighs, float* results)
{
    extern __shared__ float sdata[];

    unsigned weighsOffset = (blockIdx.x * input_size);

    float result = 0;
    unsigned i = threadIdx.x;

    if (inputType == BT_FLOAT) {
        while (i < input_size) {
            result += ((float*)inputPtr)[i] * ((float*)weighs)[weighsOffset + i];
            i += blockDim.x;
        }
    } else {
        weighsOffset += threadIdx.x * BITS_PER_UNSIGNED;

        unsigned input_blocks_to_read = ((input_size - 1) / BITS_PER_UNSIGNED) + 1;
        while (i < input_blocks_to_read) {

            //TODO TCC check performance penalty (this is just for BT_SIGN)
            unsigned maxBits = device_min(BITS_PER_UNSIGNED, input_size - (i * BITS_PER_UNSIGNED));

            unsigned mask = 0x80000000;
            unsigned currentInput = ((unsigned*)inputPtr)[i];

            for (unsigned j=0; j < maxBits; j++) {

                if (currentInput & mask) {
                    result += ((unsigned char*)weighs)[weighsOffset + j] - 128;
                } else {
                    if (inputType == BT_SIGN) {
                        result -= ((unsigned char*)weighs)[weighsOffset + j] - 128;
                    }
                }
                mask >>= 1;
            }
            i += blockSize;
            weighsOffset += blockDim.x * BITS_PER_UNSIGNED;
        }
    }

    unsigned tid = threadIdx.x;
    sdata[tid] = result;
    __syncthreads();

    if (blockSize >= 512) {if (tid < 256) {sdata[tid] += sdata[tid + 256];}__syncthreads();}
    if (blockSize >= 256) {if (tid < 128) {sdata[tid] += sdata[tid + 128];}__syncthreads();}
    if (blockSize >= 128) {if (tid < 64) {sdata[tid] += sdata[tid + 64];}__syncthreads();}

#if __DEVICE_EMULATION__
    if (blockSize >= 64) {if (tid < 32) {sdata[tid] += sdata[tid + 32];}__syncthreads();}
    if (blockSize >= 32) {if (tid < 16) {sdata[tid] += sdata[tid + 16];}__syncthreads();}
    if (blockSize >= 16) {if (tid < 8) {sdata[tid] += sdata[tid + 8];}__syncthreads();}
    if (blockSize >= 8) {if (tid < 4) {sdata[tid] += sdata[tid + 4];}__syncthreads();}
    if (blockSize >= 4) {if (tid < 2) {sdata[tid] += sdata[tid + 2];}__syncthreads();}
    if (blockSize >= 2) {if (tid < 1) {sdata[tid] += sdata[tid + 1];}__syncthreads();}
#else
    if (tid < 32) {
        if (blockSize >= 64) sdata[tid] += sdata[tid + 32];
        if (blockSize >= 32) sdata[tid] += sdata[tid + 16];
        if (blockSize >= 16) sdata[tid] += sdata[tid + 8];
        if (blockSize >= 8) sdata[tid] += sdata[tid + 4];
        if (blockSize >= 4) sdata[tid] += sdata[tid + 2];
        if (blockSize >= 2) sdata[tid] += sdata[tid + 1];
    }
#endif
    if (tid == 0) {
        results[blockIdx.x] += sdata[0];
    }
}

extern "C" void cuda_inputCalculationReduction(void* inputPtr, unsigned input_size, BufferType inputType,
                                               unsigned output_size, void* weighs, float* results,
                                               unsigned block_size)
{
    unsigned grid_size = output_size;
    unsigned shared_mem_size = block_size * sizeof(float);

    if (inputType == BT_BYTE) {
        std::string error = "cuda_inputCalculation is not implemented for BufferType BYTE as input.";
        throw error;
    } else if (inputType == BT_FLOAT) {
        switch (block_size) {
            case 512:
                SumConnectionsKernel<512, BT_FLOAT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 256:
                SumConnectionsKernel<256, BT_FLOAT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 128:
                SumConnectionsKernel<128, BT_FLOAT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 64:
                SumConnectionsKernel< 64, BT_FLOAT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 32:
                SumConnectionsKernel< 32, BT_FLOAT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 16:
                SumConnectionsKernel< 16, BT_FLOAT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 8:
                SumConnectionsKernel< 8, BT_FLOAT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 4:
                SumConnectionsKernel< 4, BT_FLOAT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 2:
                SumConnectionsKernel< 2, BT_FLOAT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 1:
                SumConnectionsKernel< 1, BT_FLOAT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
        }
    } else if (inputType == BT_BIT) {
        switch (block_size) {
            case 512:
                SumConnectionsKernel<512, BT_BIT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 256:
                SumConnectionsKernel<256, BT_BIT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 128:
                SumConnectionsKernel<128, BT_BIT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 64:
                SumConnectionsKernel< 64, BT_BIT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 32:
                SumConnectionsKernel< 32, BT_BIT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 16:
                SumConnectionsKernel< 16, BT_BIT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 8:
                SumConnectionsKernel< 8, BT_BIT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 4:
                SumConnectionsKernel< 4, BT_BIT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 2:
                SumConnectionsKernel< 2, BT_BIT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 1:
                SumConnectionsKernel< 1, BT_BIT><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
        }
    } else {
        switch (block_size) {
            case 512:
                SumConnectionsKernel<512, BT_SIGN><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 256:
                SumConnectionsKernel<256, BT_SIGN><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 128:
                SumConnectionsKernel<128, BT_SIGN><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 64:
                SumConnectionsKernel< 64, BT_SIGN><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 32:
                SumConnectionsKernel< 32, BT_SIGN><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 16:
                SumConnectionsKernel< 16, BT_SIGN><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 8:
                SumConnectionsKernel< 8, BT_SIGN><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 4:
                SumConnectionsKernel< 4, BT_SIGN><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 2:
                SumConnectionsKernel< 2, BT_SIGN><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
            case 1:
                SumConnectionsKernel< 1, BT_SIGN><<< grid_size, block_size, shared_mem_size >>>(inputPtr, input_size, output_size, weighs, results); break;
        }
    }
    checkCUDAError("cuda_inputCalculation2");
}
