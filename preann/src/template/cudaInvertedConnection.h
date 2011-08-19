#ifndef CUDAINVERTEDCONNECTION_H_
#define CUDAINVERTEDCONNECTION_H_

#include "connection.h"
#include "cudaBuffer.h"

template <BufferType bufferTypeTempl, class c_typeTempl>
class CudaInvertedConnection: public virtual FullConnection, public CudaBuffer<bufferTypeTempl, c_typeTempl> {
protected:
	//redefined from CudaBuffer
	virtual void copyFromImpl(Interface* interface)
	{
		interface->transposeMatrix(tInput->getSize());
		CudaBuffer<bufferTypeTempl, c_typeTempl>::copyFromImpl(interface);
	}

	virtual void copyToImpl(Interface* interface)
	{
		CudaBuffer<bufferTypeTempl, c_typeTempl>::copyToImpl(interface);
		interface->transposeMatrix(tSize / tInput->getSize());
	}

	virtual void mutateImpl(unsigned pos, float mutation)
	{
		//TODO z simplificar cuentas
		unsigned outputPos = pos / tInput->getSize();
		unsigned inputPos = (pos % tInput->getSize());
		unsigned outputSize = tSize / tInput->getSize();
		pos = outputPos + (inputPos * outputSize);

		cuda_mutate(data, pos, mutation, bufferTypeTempl);
	}

	virtual void crossoverImpl(Buffer* other, Interface* bitBuffer)
	{
		Interface invertedBitBuffer = Interface(bitBuffer);
		invertedBitBuffer.transposeMatrix(tInput->getSize());

		CudaBuffer<bufferTypeTempl, c_typeTempl> cudaBitBuffer(&invertedBitBuffer, Cuda_Threads_Per_Block);

	    cuda_crossover(this->getDataPointer(), other->getDataPointer(), (unsigned*)cudaBitBuffer.getDataPointer(),
							tSize, bufferTypeTempl, Cuda_Threads_Per_Block);
	}
public:
	CudaInvertedConnection(Buffer* input, unsigned outputSize)
		: CudaBuffer<bufferTypeTempl, c_typeTempl>(input->getSize() * outputSize)
	{
		tInput = input;
	}

	virtual ~CudaInvertedConnection() {};

	virtual ImplementationType getImplementationType() {
		return CUDA_INV;
	};

	virtual void calculateAndAddTo(Buffer* results)
	{
		void* inputWeighs = this->getDataPointer();
		float* resultsPtr = (float*)results->getDataPointer();

		cuda_inputCalculationInvertedMatrix(tInput->getDataPointer(), tInput->getSize(), tInput->getBufferType(), results->getSize(), inputWeighs, resultsPtr, Cuda_Threads_Per_Block);
	}

};

#endif /* CUDAINVERTEDCONNECTION_H_ */