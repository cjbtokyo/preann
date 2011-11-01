/*
 * individual.cpp
 *
 *  Created on: Feb 22, 2010
 *      Author: timon
 */

#include "individual.h"

Individual::Individual(ImplementationType implementationType) :
	NeuralNet(implementationType)
{
}

Individual::~Individual()
{
}

Individual* Individual::newCopy(ImplementationType implementationType, bool copyWeighs)
{
	Individual* copy = new Individual(implementationType);

	// layers and inputs
	for (unsigned i = 0; i < inputs.size(); i++){
		copy->addInputLayer(inputs[i]->getInputInterface());
	}
	for (unsigned i = 0; i < layers.size(); i++)
	{
		Buffer* layerBuffer = layers[i]->getOutput();
		copy->addLayer(layerBuffer->getSize(), layerBuffer->getBufferType(), layers[i]->getFunctionType());
	}

	// connections
	std::vector< std::pair<unsigned, unsigned> >::iterator it;
	for(it = inputConnectionsGraph.getIterator(); it != inputConnectionsGraph.getEnd(); ++it){
		copy->addInputConnection(it->first, it->second);
	}
	for(it = connectionsGraph.getIterator(); it != connectionsGraph.getEnd(); ++it){
		copy->addLayersConnection(it->first, it->second);
	}

	// weighs
	if(copyWeighs){
		for (unsigned i = 0; i < layers.size(); i++) {
			copy->getLayer(i)->copyWeighs(layers[i]);
		}
	}
	return copy;
}

Individual* Individual::newCopy(bool copyWeighs)
{
	return newCopy(this->getImplementationType(), copyWeighs);
}

void Individual::mutate(unsigned numMutations, float mutationRange)
{
	for (unsigned i = 0; i < numMutations; i++)
	{
		unsigned chosenLayer = Random::positiveInteger(layers.size());
		unsigned chosenConnection = Random::positiveInteger(
				layers[chosenLayer]->getNumberInputs() + 1);
		Connection* connection;
		if (chosenConnection == layers[chosenLayer]->getNumberInputs())
		{
			connection = layers[chosenLayer]->getThresholds();
		}
		else
		{
			connection = layers[chosenLayer]->getConnection(chosenConnection);
		}
		connection->mutate(Random::positiveInteger(connection->getSize()), Random::floatNum(
				mutationRange));
	}
}

void Individual::mutate(float probability, float mutationRange)
{
	for (unsigned i = 0; i < layers.size(); i++)
	{
		for (unsigned j = 0; j < layers[i]->getNumberInputs(); j++)
		{

			Connection* connection = layers[i]->getConnection(j);
			for (unsigned k = 0; k < connection->getSize(); k++)
			{
				if (Random::positiveFloat(1) < probability)
				{
					connection->mutate(k, Random::floatNum(mutationRange));
				}
			}
		}
		Connection* connection = layers[i]->getThresholds();
		for (unsigned k = 0; k < connection->getSize(); k++)
		{
			if (Random::positiveFloat(1) < probability)
			{
				connection->mutate(k, Random::floatNum(mutationRange));
			}
		}
	}
}

void Individual::proportionalCrossover(CrossoverLevel crossoverLevel, Individual* other)
{
	float otherFitness = other->getFitness();
	if (fitness * otherFitness < 0)
	{
		std::string error =
			"The fitness of the individuals have different sign or are equal to zero: cannot crossover them proportionally.";
		throw error;
	}
	float thisFitness = fitness;
	if (fitness < 0)
	{
		otherFitness = -thisFitness;
		thisFitness = -otherFitness;
	}
	float probability = thisFitness / (thisFitness + otherFitness);
	uniformCrossover(crossoverLevel, other, probability);
}

void Individual::uniformCrossover(CrossoverLevel crossoverLevel, Individual* other, float probability)
{
	checkCrossoverCompatibility(other);

	vector<Interface*> bitmaps = prepareCrossover(crossoverLevel);
	applyUniform(bitmaps, probability);
	crossover(crossoverLevel, other, bitmaps);
	freeBitmaps(bitmaps);
}

void Individual::multipointCrossover(CrossoverLevel crossoverLevel, Individual* other, unsigned numPoints)
{
	checkCrossoverCompatibility(other);

	vector<Interface*> bitmaps = prepareCrossover(crossoverLevel);
	applyMultipoint(bitmaps, numPoints);
	crossover(crossoverLevel, other, bitmaps);
	freeBitmaps(bitmaps);
}

void Individual::freeBitmaps(vector<Interface*> bitmaps)
{
	for (unsigned i = 0; i < bitmaps.size(); ++i) {
		delete(bitmaps[i]);
	}
	bitmaps.clear();
}

void Individual::applyUniform(vector<Interface*> bitmaps, float probability)
{
	for (unsigned i = 0; i < bitmaps.size(); i++) {
		for (unsigned j = 0; j < bitmaps[i]->getSize(); j++){

			if (Random::positiveFloat(1) < probability)
			{
				bitmaps[i]->setElement(j, 1);
			}
		}
	}
}

void Individual::applyMultipoint(vector<Interface*> bitmaps, unsigned numPoints)
{
	while (numPoints > 0)
	{
		unsigned chosenConnection = Random::positiveInteger(bitmaps.size());
		unsigned chosenPoint = Random::positiveInteger(bitmaps[chosenConnection]->getSize());

		if (!bitmaps[chosenConnection]->getElement(chosenPoint))
		{
			bitmaps[chosenConnection]->setElement(chosenPoint, 1);
			--numPoints;
		}
	}
	unsigned progenitor = 0;
	for (unsigned i = 0; i < bitmaps.size(); i++){
		for (unsigned j = 0; j < bitmaps[i]->getSize(); j++){

			if (bitmaps[i]->getElement(j))
			{
				if (progenitor == 1)
					progenitor = 0;
				else
					progenitor = 1;
			}
			bitmaps[i]->setElement(j, progenitor);
		}
	}
}

vector<Interface*> Individual::prepareCrossover(CrossoverLevel crossoverLevel)
{
	switch (crossoverLevel)
	{
	case WEIGH:
		return prepareCrossoverWeighs();
	case NEURON:
	case NEURON_INVERTED:
		return prepareCrossoverNeurons();
	case LAYER:
		return prepareCrossoverLayers();
	}
}

void Individual::crossover(CrossoverLevel crossoverLevel, Individual* other, vector<Interface*> bitmaps)
{
	switch (crossoverLevel)
	{
	case WEIGH:
		crossoverWeighs(other, bitmaps);
		break;
	case NEURON:
		crossoverNeurons(other, bitmaps);
		break;
	case NEURON_INVERTED:
		crossoverNeuronsInverted(other, bitmaps);
		break;
	case LAYER:
		crossoverLayers(other, bitmaps);
		break;
	}
}

vector<Interface*> Individual::prepareCrossoverWeighs()
{
	vector<Interface*> bitmaps;
	for (unsigned i = 0; i < layers.size(); i++){
		for (unsigned j = 0; j < layers[i]->getNumberInputs(); j++){

			Connection* connection = layers[i]->getConnection(j);
			Interface* bitmap = new Interface(connection->getSize(), BIT);
			bitmaps.push_back(bitmap);
		}
		Connection* thresholds = layers[i]->getThresholds();
		Interface* bitmap = new Interface(thresholds->getSize(), BIT);
		bitmaps.push_back(bitmap);
	}
	return bitmaps;
}

vector<Interface*> Individual::prepareCrossoverNeurons()
{
	vector<Interface*> bitmaps;
	for (unsigned i = 0; i < layers.size(); i++){

		Interface* bitmap = new Interface(layers[i]->getOutput()->getSize(), BIT);
		bitmaps.push_back(bitmap);
	}
	return bitmaps;
}

vector<Interface*> Individual::prepareCrossoverLayers()
{
	vector<Interface*> bitmaps;
	Interface* bitmap = new Interface(layers.size(), BIT);
	bitmaps.push_back(bitmap);
	return bitmaps;
}

void Individual::crossoverWeighs(Individual* other, vector<Interface*> bitmaps)
{
	unsigned index = 0;
	for (unsigned i = 0; i < layers.size(); i++)
	{
		for (unsigned j = 0; j < layers[i]->getNumberInputs(); j++)
		{
			Connection* connection = layers[i]->getConnection(j);
			connection->crossover(other->getLayer(i)->getConnection(j), bitmaps[index]);
			index++;
		}
		Connection* thresholds = layers[i]->getThresholds();
		thresholds->crossover(other->getLayer(i)->getThresholds(), bitmaps[index]);
		index++;
	}
}

void Individual::crossoverNeurons(Individual *other, vector<Interface*> bitmaps)
{
	vector<Interface*> bitmapsWeighs = prepareCrossoverWeighs();

	unsigned index = 0;

	for (unsigned i = 0; i < layers.size(); i++)
	{
		unsigned outputSize = layers[i]->getOutput()->getSize();

		for (unsigned j = 0; j < layers[i]->getNumberInputs(); j++)
		{
			unsigned inputSize = layers[i]->getConnection(j)->getInput()->getSize();

			unsigned offset = 0;
			for (unsigned k = 0; k < outputSize; k++)
			{
				if (bitmaps[i]->getElement(k))
				{
					for (unsigned l = 0; l < inputSize; l++)
					{
						bitmapsWeighs[index]->setElement(offset + l, 1);
					}
				}
				offset += inputSize;
			}
			++index;
		}
		bitmapsWeighs[index]->copyFromFast(bitmaps[i]);
		++index;
	}

	crossoverWeighs(other, bitmapsWeighs);
}

void Individual::crossoverNeuronsInverted(Individual* other, vector<Interface*> bitmaps)
{
	vector<Interface*> bitmapsWeighs = prepareCrossoverWeighs();

	unsigned index = 0;

	for (unsigned outputLay = 0; outputLay < layers.size(); outputLay++)
	{
		for (unsigned k = 0; k < layers[outputLay]->getNumberInputs(); k++)
		{
			for (unsigned inputLay = 0; inputLay < layers.size(); inputLay++)
			{
				Buffer* input = layers[inputLay]->getOutput();
				if (input == layers[outputLay]->getConnection(k)->getInput())
				{
					for (unsigned i = 0; i < input->getSize(); i++)
					{
						if (bitmaps[inputLay]->getElement(i))
						{
							unsigned offset = 0;
							while (offset < layers[outputLay]->getConnection(k)->getSize())
							{
								bitmapsWeighs[index]->setElement( offset + i, 1);
								offset += input->getSize();
							}
						}
					}
				}
			}
			++index;
		}
		bitmapsWeighs[index]->copyFromFast(bitmaps[outputLay]);
		++index;
	}

	crossoverWeighs(other, bitmapsWeighs);
}

void Individual::crossoverLayers(Individual* other, vector<Interface*> bitmaps)
{
	if (bitmaps[0]->getSize() != layers.size())
	{
		std::string error =
				"The number of layers must be equal to the size of the bitBuffer.";
		throw error;
	}
	for (unsigned i = 0; i < layers.size(); i++)
	{
		if (!bitmaps[0]->getElement(i))
		{
			//TODO no debería ser swapWeighs ?
			layers[i]->copyWeighs(other->layers[i]);
		}
	}
}

void Individual::setFitness(float fitness)
{
	this->fitness = fitness;
}

float Individual::getFitness()
{
	return fitness;
}

void Individual::checkCrossoverCompatibility(Individual* other)
{
	if (layers.size() != other->getNumLayers()
		|| inputs.size() != other->getNumInputs()
		|| this->getImplementationType() != other->getImplementationType())
	{
		std::string error =
				"The individuals are incompatible: cannot crossover them.";
		throw error;
	}
	for (unsigned i = 0; i < layers.size(); ++i)
	{
		Layer* tLayer = layers[i];
		Layer* otherLayer = other->getLayer(i);

		if (tLayer->getOutput()->getSize() != otherLayer->getOutput()->getSize()
			|| tLayer->getOutput()->getBufferType() != otherLayer->getOutput()->getBufferType()
			|| tLayer->getNumberInputs() != otherLayer->getNumberInputs())
		{
			std::string error =
					"The individuals are incompatible: cannot crossover them.";
			throw error;
		}
		for (unsigned j = 0; j < tLayer->getNumberInputs(); ++j)
		{
			Connection* tConnection = tLayer->getConnection(j);
			Connection* otherConnection = otherLayer->getConnection(j);

			if (tConnection->getSize() != otherConnection->getSize()
				|| tConnection->getBufferType() != otherConnection->getBufferType())
			{
				std::string error =
						"The individuals are incompatible: cannot crossover them.";
				throw error;
			}
		}
	}
}

