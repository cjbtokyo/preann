/*
 * inputLayer.h
 *
 *  Created on: Nov 28, 2010
 *      Author: timon
 */

#ifndef INPUTLAYER_H_
#define INPUTLAYER_H_

#include "layer.h"

class InputLayer: public Layer {
	Interface* tInput;
protected:
	ImplementationType getImplementationType();
public:
	InputLayer(Interface* interface, ImplementationType implementationType);
	InputLayer(unsigned size, BufferType bufferType, ImplementationType implementationType);
	virtual ~InputLayer();

	virtual void addInput(Buffer* input);
	virtual void calculateOutput();
	virtual void copyWeighs(Layer* sourceLayer);

	Interface* getInputInterface();

};

#endif /* INPUTLAYER_H_ */
