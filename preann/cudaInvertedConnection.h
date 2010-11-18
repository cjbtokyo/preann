
#ifndef CUDAINVERTEDCONNECTION_H_
#define CUDAINVERTEDCONNECTION_H_

#include "connection.h"
#include "cudaVector.h"

class CudaInvertedConnection: public virtual Connection, public CudaVector {
protected:
	//redefined from CudaVector
	virtual void copyFromImpl(Interface* interface);
	virtual void copyToImpl(Interface* interface);
	virtual void crossoverImpl(Connection* other, Interface* bitVector);
public:
	CudaInvertedConnection(Vector* input, unsigned outputSize, VectorType vectorType);
	virtual ~CudaInvertedConnection() {};

	virtual void mutateImpl(unsigned pos, float mutation);
	virtual void addToResults(Vector* results);

};

#endif /* CUDAINVERTEDCONNECTION_H_ */