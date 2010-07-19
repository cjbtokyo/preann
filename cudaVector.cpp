/*
 * cudaVector.cpp
 *
 *  Created on: Mar 25, 2010
 *      Author: timon
 */

#include "cudaVector.h"

//special constructor for bit coalescing vectors
CudaVector::CudaVector(unsigned size, VectorType vectorType, unsigned block_size)
{
	(((size-1)/BITS_PER_UNSIGNED)+1) * sizeof(unsigned);
	this->size = size;
	this->vectorType = vectorType;

	unsigned byte_sz = ((size-1)/(BITS_PER_UNSIGNED * block_size)+1) * (sizeof(unsigned) * block_size);
	data = cuda_malloc(byte_sz);

	cuda_setZero(data, byte_sz, vectorType, CUDA_THREADS_PER_BLOCK);
}

CudaVector::CudaVector(unsigned size, VectorType vectorType, FunctionType functionType)
{
	this->size = size;
	this->vectorType = vectorType;
	this->functionType = functionType;

	unsigned byte_sz = getByteSize();
	data = cuda_malloc(byte_sz);

	cuda_setZero(data, byte_sz, vectorType, CUDA_THREADS_PER_BLOCK);
}

CudaVector::~CudaVector()
{
	if (data) {
		cuda_free(data);
		data = NULL;
	}
}

void CudaVector::copyFrom2(Interface* interface, unsigned block_size)
{
	if (vectorType != interface->getVectorType()){
		string error = "The Type of the Interface is different than the Vector Type.";
		throw error;
	}
	if (size < interface->getSize()){
		string error = "The Interface is greater than the Vector.";
		throw error;
	}
	if (interface->getVectorType() == FLOAT){
		cuda_copyToDevice(data, interface->getDataPointer(), interface->getByteSize());
	} else {


		unsigned interfaceSize = interface->getSize();

		unsigned otherSize = ((interfaceSize-1)/(BITS_PER_UNSIGNED * block_size)+1) * (BITS_PER_UNSIGNED * block_size);
		Interface interfaceOrderedByBlockSize = Interface(otherSize, interface->getVectorType());

		unsigned bit = 0;
		unsigned thread = 0;
		unsigned block_offset = 0;

		for (unsigned i=0; i < interfaceSize; i++){

			unsigned weighPos = (thread * BITS_PER_UNSIGNED) + bit + block_offset;
			interfaceOrderedByBlockSize.setElement(weighPos, interface->getElement(i++));
			thread++;
			if (thread == block_size){
				thread = 0;
				bit++;
				if (bit == BITS_PER_UNSIGNED){
					bit = 0;
					block_offset += (block_size * BITS_PER_UNSIGNED);
				}
			}
		}
		cuda_copyToDevice(data, interfaceOrderedByBlockSize.getDataPointer(), interfaceOrderedByBlockSize.getByteSize());
	}
}

void CudaVector::copyFrom(Interface *interface)
{
	if (size < interface->getSize()){
		string error = "The Interface is greater than the Vector.";
		throw error;
	}
	if (vectorType != interface->getVectorType()){
		string error = "The Type of the Interface is different than the Vector Type.";
		throw error;
	}
	cuda_copyToDevice(data, interface->getDataPointer(), interface->getByteSize());
}

void CudaVector::copyTo(Interface *interface)
{
	if (interface->getSize() < size){
		string error = "The Vector is greater than the Interface.";
		throw error;
	}
	if (vectorType != interface->getVectorType()){
		string error = "The Type of the Interface is different than the Vector Type.";
		throw error;
	}
	cuda_copyToHost(interface->getDataPointer(), data, this->getByteSize());
}

void CudaVector::activation(float* results)
{
	cuda_activation(data, size, vectorType, results, functionType, CUDA_THREADS_PER_BLOCK);
}

