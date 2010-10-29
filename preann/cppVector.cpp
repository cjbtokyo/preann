
#include "cppVector.h"

CppVector::CppVector(unsigned size, VectorType vectorType)
{
	this->size = size;
	this->vectorType = vectorType;

	size_t byteSize = getByteSize();
	data = mi_malloc(byteSize);

	switch (vectorType){
		case BYTE:  SetValueToAnArray<unsigned char>(data, byteSize, 128); 		break;
		case FLOAT: SetValueToAnArray<float>(data, byteSize/sizeof(float), 0);  break;
		case BIT:
		case SIGN: 	SetValueToAnArray<unsigned char>(data, byteSize, 0);		break;
	}
}

CppVector::~CppVector()
{
	if (data) {
		mi_free(data);
		data = NULL;
	}
}

Vector* CppVector::clone()
{
	//TODO implementar CppVector::clone()
	Vector* clone = new CppVector(size, vectorType);
	copyToVector(clone);
	return clone;
}

void CppVector::copyFrom(Interface* interface)
{
	if (size < interface->getSize()){
		std::string error = "The Interface is greater than the Vector.";
		throw error;
	}
	if (vectorType != interface->getVectorType()){
		std::string error = "The Type of the Interface is different than the Vector Type.";
		throw error;
	}
	memcpy(data, interface->getDataPointer(), interface->getByteSize());
}

void CppVector::copyTo(Interface* interface)
{
	if (interface->getSize() < size){
		std::string error = "The Vector is greater than the Interface.";
		throw error;
	}
	if (vectorType != interface->getVectorType()){
		std::string error = "The Type of the Interface is different than the Vector Type.";
		throw error;
	}
	memcpy(interface->getDataPointer(), data, this->getByteSize());
}

void CppVector::inputCalculation(Vector* input, Vector* inputWeighsVect)
{
	void* inputWeighs = inputWeighsVect->getDataPointer();
	float* results = (float*)this->getDataPointer();

	void* inputPtr = input->getDataPointer();
	unsigned inputSize = input->getSize();

	for (unsigned j=0; j < size; j++){

		for (unsigned k=0; k < inputSize; k++){

			unsigned weighPos = (j * inputSize) + k;
			if (input->getVectorType() == FLOAT) {
				results[j] += ((float*)inputPtr)[k] * ((float*)inputWeighs)[weighPos];
			} else {
				if ( ((unsigned*)inputPtr)[k/BITS_PER_UNSIGNED] & (0x80000000>>(k % BITS_PER_UNSIGNED)) ) {
					results[j] += (((unsigned char*)inputWeighs)[weighPos] - 128);
				} else if (input->getVectorType() == SIGN) {
					results[j] -= (((unsigned char*)inputWeighs)[weighPos] - 128);
				}
			}
		}
	}
}

void CppVector::activation(Vector* resultsVect, FunctionType functionType)
{
	float* results = (float*)resultsVect->getDataPointer();

	if (vectorType == FLOAT){
		for (unsigned i=0; i < size; i++){
			((float*)data)[i] = Function(results[i], functionType);
		}
	} else {
		unsigned* vectorData = (unsigned*)data;
		unsigned mask;
		for (unsigned i=0; i < size; i++){

			if (i % BITS_PER_UNSIGNED == 0){
				mask = 0x80000000;
			} else {
				mask >>= 1;
			}

			if (results[i] > 0){
				vectorData[i/BITS_PER_UNSIGNED] |= mask;
			} else {
				vectorData[i/BITS_PER_UNSIGNED] &= ~mask;
			}
		}
	}
	mi_free(results);
}

void CppVector::mutate(unsigned pos, float mutation, unsigned inputSize)
{
	if (pos > size){
		std::string error = "The position being mutated is greater than the size of the vector.";
	}
	switch (vectorType){
	case BYTE:{
		unsigned char* weigh = &(((unsigned char*)data)[pos]);
		int result = (int)mutation + *weigh;
		if (result <= 0){
			*weigh = 0;
		}
		else if (result >= 255) {
			*weigh = 255;
		}
		else {
			*weigh = result;
		}
		}break;
	case FLOAT:
		((float*)data)[pos] += mutation;
		break;
	case BIT:
	case SIGN:
		{
		std::string error = "CppVector::mutate is not implemented for VectorType BIT nor SIGN.";
		throw error;
		}
	}
}
void CppVector::weighCrossover(Vector* other, Vector* bitVector, unsigned inputSize)
{
	//TODO impl CppVector::weighCrossover
}


unsigned CppVector::getByteSize()
{
	switch (vectorType){
	case BYTE:
		return size;
	case FLOAT:
		return size * sizeof(float);
	case BIT:
	case SIGN:
		return (((size-1)/BITS_PER_UNSIGNED)+1) * sizeof(unsigned);
	}
}
