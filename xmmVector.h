/*
 * xmmVector.h
 *
 *  Created on: Nov 17, 2009
 *      Author: timon
 */

#ifndef XMMVECTOR_H_
#define XMMVECTOR_H_

#include "vector.h"
#include "xmmDefinitions.h"

class XmmVector: public Vector {
	virtual unsigned getByteSize();
public:
	XmmVector(unsigned size, VectorType vectorType);
	virtual ~XmmVector();

	virtual void copyFrom(Interface* interface);
	virtual void copyTo(Interface* interface);
	virtual void activation(float* results, FunctionType functionType);

	unsigned getNumLoops();
};

#endif /* XMMVECTOR_H_ */
