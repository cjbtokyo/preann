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

// GENETIC OPERATORS

template <class type>
__global__
void crossoverKernel(type* buffer1, type* buffer2, unsigned* bitBuffer, unsigned size)
{
    unsigned weighPos = (blockIdx.x * blockDim.x * BITS_PER_UNSIGNED) + threadIdx.x;
    unsigned maxPosForThisBlock = device_min ( (blockIdx.x + 1) * blockDim.x * BITS_PER_UNSIGNED,
                                        size);
    unsigned bitsForTheThread, mask;
    if (weighPos < maxPosForThisBlock) {
        bitsForTheThread = bitBuffer[(blockIdx.x * blockDim.x) + threadIdx.x];
        mask = 0x80000000;
    }
    __syncthreads();
    while (weighPos < maxPosForThisBlock) {
        if (mask & bitsForTheThread) {
            type aux = buffer1[weighPos];
            buffer1[weighPos] = buffer2[weighPos];
            buffer2[weighPos] = aux;
        }
        weighPos += blockDim.x;
        mask >>= 1;
    }
}

extern "C" void cuda_crossover(void* buffer1, void* buffer2, unsigned* bitBuffer, unsigned size,
                               BufferType bufferType, unsigned block_size)
{
    unsigned grid_size = ((size - 1) / (block_size * BITS_PER_UNSIGNED)) + 1;

    switch (bufferType) {
        case BT_BYTE:
            crossoverKernel<unsigned char><<< grid_size, block_size >>>
            ((unsigned char*)buffer1, (unsigned char*)buffer2, (unsigned*)bitBuffer, size);

            break;
        case BT_FLOAT:
            crossoverKernel<float><<< grid_size, block_size >>>
            ((float*)buffer1, (float*)buffer2, (unsigned*)bitBuffer, size);
            break;
        case BT_BIT:
        case BT_SIGN:
            {
                std::string error = "cuda_crossover is not implemented for BufferType BIT nor SIGN.";
                throw error;
            }
    }
}

//TODO CU es necesario usar un kernel para esto ??
__global__
void resetFloatKernel(float* buffer, unsigned pos)
{
    if (threadIdx.x == 0) {
        buffer[pos] = 0;
    }
}

__global__
void resetByteKernel(unsigned char* buffer, unsigned pos)
{
    if (threadIdx.x == 0) {
        buffer[pos] = 128;
    }
}

__global__
void mutateFloatKernel(float* buffer, unsigned pos, float mutation)
{
    if (threadIdx.x == 0) {
        buffer[pos] += mutation;
    }
}

__global__
void mutateByteKernel(unsigned char* buffer, unsigned pos, int mutation)
{
    if (threadIdx.x == 0) {
        int result = mutation + buffer[pos];
        if (result <= 0) {
            buffer[pos] = 0;
        } else if (result >= 255) {
            buffer[pos] = 255;
        } else {
            buffer[pos] = (unsigned char) result;
        }
    }
}

extern "C" void cuda_mutateWeigh(void* buffer, unsigned pos, float mutation, BufferType bufferType)
{
    switch (bufferType) {
        case BT_BYTE:
            mutateByteKernel<<< 1, 8 >>>((unsigned char*)buffer, pos, (int)mutation);
            break;
        case BT_FLOAT:
            mutateFloatKernel<<< 1, 8 >>>((float*)buffer, pos, mutation);
            break;
        case BT_BIT:
        case BT_SIGN:
            {
                std::string error = "cuda_mutateWeigh is not implemented for BufferType BIT nor SIGN.";
                throw error;
            }
    }
}

extern "C" void cuda_resetWeigh(void* buffer, unsigned pos, BufferType bufferType)
{
    switch (bufferType) {
        case BT_BYTE:
            resetByteKernel<<< 1, 8 >>>((unsigned char*)buffer, pos);
            break;
        case BT_FLOAT:
            resetFloatKernel<<< 1, 8 >>>((float*)buffer, pos);
            break;
        case BT_BIT:
        case BT_SIGN:
            {
                std::string error = "cuda_resetWeigh is not implemented for BufferType BIT nor SIGN.";
                throw error;
            }
    }
}