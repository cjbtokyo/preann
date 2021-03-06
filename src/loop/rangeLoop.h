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

    virtual void __repeatImpl(LoopFunction* func);
    virtual std::string valueToString();
public:
    RangeLoop(std::string key, float min, float max, float inc);
    virtual ~RangeLoop();

    void resetRange(float min, float max, float inc);
    float getCurrentValue();

    virtual unsigned getNumBranches();
    float* toArray();

    virtual void print();
};

#endif /* RANGELOOP_H_ */
