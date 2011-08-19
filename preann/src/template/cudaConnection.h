/*
 * cudaConnection.h
 *
 *  Created on: Nov 15, 2010
 *      Author: timon
 */

#ifndef CUDACONNECTION_H_
#define CUDACONNECTION_H_

#include "connection.h"
#include "cudaBuffer.h"

template <BufferType bufferTypeTempl, class c_typeTempl>
class CudaConnection: public virtual FullConnection, public CudaBuffer<bufferTypeTempl, c_typeTempl> {
protected:
	virtual void mutateImpl(unsigned pos, float mutation)
	{
		cuda_mutate(data, pos, mutation, bufferTypeTempl);
	}

	virtual void crossoverImpl(Buffer* other, Interface* bitBuffer)
	{
		CudaBuffer<bufferTypeTempl, c_typeTempl> cudaBitBuffer(bitBuffer, Cuda_Threads_Per_Block);

	    cuda_crossover(this->getDataPointer(), other->getDataPointer(), (unsigned*)cudaBitBuffer.getDataPointer(),
							tSize, bufferTypeTempl, Cuda_Threads_Per_Block);
	}
public:
	CudaConnection(Buffer* input, unsigned outputSize)
		: CudaBuffer<bufferTypeTempl, c_typeTempl>(input->getSize() * outputSize)
	{
		tInput = input;
	}

	virtual ~CudaConnection() {};

	virtual void calculateAndAddTo(Buffer* results)
	{
		void* inputWeighs = this->getDataPointer();
		float* resultsPtr = (float*)results->getDataPointer();
		// TODO TCC este método no funciona correctamente para SIGN
		cuda_inputCalculation(tInput->getDataPointer(), tInput->getSize(), tInput->getBufferType(), results->getSize(), inputWeighs, resultsPtr, Cuda_Threads_Per_Block);
	}

};



#endif /* CUDACONNECTION_H_ */