/*
 * rangeLoop.h
 *
 *  Created on: Feb 21, 2012
 *      Author: timon
 */

#ifndef RANGELOOP_H_
#define RANGELOOP_H_

#include "loop.h"

class RangeLoop : public Loop
{
protected:
    float tValue, tMin, tMax, tInc;
    virtual unsigned valueToUnsigned();

    virtual void repeatFunctionImpl(void(*func)(ParametersMap*), ParametersMap* parametersMap);
    virtual void
    repeatActionImpl(void(*action)(void(*)(ParametersMap*), ParametersMap* parametersMap),
                     void(*func)(ParametersMap*), ParametersMap* parametersMap);
    virtual std::string valueToString();
public:
    RangeLoop(std::string key, float min, float max, float inc, Loop* innerLoop);
    virtual ~RangeLoop();

    void resetRange(float min, float max, float inc);

    virtual void print();
};

#endif /* RANGELOOP_H_ */