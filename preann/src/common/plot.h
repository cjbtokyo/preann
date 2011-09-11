
#ifndef PLOT_H_
#define PLOT_H_

#include "chronometer.h"
#include "test.h"
#include "factory.h"
#include "population.h"
#include "task.h"

#define START_PLOT Chronometer chrono;
#define END_PLOT return chrono.getSeconds();

#define START_BUFFER_PLOT START_PLOT START_BUFFER
#define END_BUFFER_PLOT END_BUFFER END_PLOT

#define START_CONNECTION_PLOT START_PLOT START_CONNECTION
#define END_CONNECTION_PLOT END_CONNECTION END_PLOT

#define FOR_PLOT_ITERATOR for(*plotIterator.variable = plotIterator.min; *plotIterator.variable <= plotIterator.max; *plotIterator.variable += plotIterator.increment)

class Plot : public Test {
protected:
	IteratorConfig plotIterator;

	EnumType colorEnum;
	EnumType pointEnum;
	int getPointType();
	int getLineColor();
public:
	Plot();
	virtual ~Plot();

	void setColorEnum(EnumType colorEnum);
	void setPointEnum(EnumType pointEnum);
	void addPlotIterator(int* variable, unsigned min, unsigned max, unsigned increment);
	float plot(string path, float (*f)(Test*, unsigned), unsigned repetitions, string testedMethod);
	float plotTask(string path, Population* population);

};

#endif /* PLOT_H_ */
