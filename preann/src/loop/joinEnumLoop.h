/*
 * joinEnumLoop.h
 *
 *  Created on: Feb 21, 2012
 *      Author: timon
 */

#ifndef JOINENUMLOOP_H_
#define JOINENUMLOOP_H_

#include "loop.h"

class JoinEnumLoop : public Loop
{
protected:
    EnumType tEnumType;
    vector<unsigned> tValueVector;
    vector<Loop*> tInnerLoops;
    unsigned tIndex;
    virtual unsigned valueToUnsigned();

    virtual void repeatFunctionImpl(LoopFunction* func, ParametersMap* parametersMap);
public:
    JoinEnumLoop(std::string key, EnumType enumType);
    virtual ~JoinEnumLoop();

    virtual void addInnerLoop(Loop* innerLoop);
    void addEnumLoop(unsigned enumValue, Loop* loop);

    virtual Loop* findLoop(std::string key);
    virtual void print();
    virtual std::string valueToString();
};

#endif /* JOINENUMLOOP_H_ */