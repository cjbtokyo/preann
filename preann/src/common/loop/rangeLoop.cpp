/*
 * rangeLoop.cpp
 *
 *  Created on: Feb 21, 2012
 *      Author: timon
 */

#include "rangeLoop.h"

RangeLoop::RangeLoop(std::string key, float min, float max, float inc) :
        Loop(key)
{
    tMin = min;
    tMax = max;
    tInc = inc;
    tUnsignedValue = 0;
}

RangeLoop::~RangeLoop()
{
}

void RangeLoop::resetRange(float min, float max, float inc)
{
    tMin = min;
    tMax = max;
    tInc = inc;
    tUnsignedValue = 0;
}

unsigned RangeLoop::valueToUnsigned()
{
    return tUnsignedValue;
}

void RangeLoop::print()
{
    if (tMin + tInc < tMax) {
        cout << tKey << ": from " << tMin << " to " << tMax << " by " << tInc << endl;
    }
    if (tInnerLoop != NULL) {
        tInnerLoop->print();
    }
}

std::string RangeLoop::valueToString()
{
    return to_string(tValue);
}

void RangeLoop::repeatFunctionImpl(void (*func)(ParametersMap*), ParametersMap* parametersMap)
{
    tUnsignedValue = 0;
    for (tValue = tMin; tValue < tMax; tValue += tInc) {
        ++tUnsignedValue;
        parametersMap->putNumber(tKey, tValue);
        this->repeatFunctionBase(func, parametersMap);
    }
}

void RangeLoop::repeatActionImpl(void (*action)(void (*)(ParametersMap*), ParametersMap* parametersMap),
                                 void (*func)(ParametersMap*), ParametersMap* parametersMap)
{
    tUnsignedValue = 0;
    for (tValue = tMin; tValue < tMax; tValue += tInc) {
        ++tUnsignedValue;
        parametersMap->putNumber(tKey, tValue);
        this->repeatActionBase(action, func, parametersMap);
    }
}

