#include <iostream>
#include <fstream>

using namespace std;

#include "common/test.h"
#include "common/dummy.h"

#define START                                                                           \
    Buffer* buffer = Dummy::buffer(parametersMap);

#define END                                                                             \
    delete (buffer);

void chronoCopyToInterface(ParametersMap* parametersMap)
{
    START

    Interface interface(buffer->getSize(), buffer->getBufferType());
    START_CHRONO
        buffer->copyToInterface(&interface);
    STOP_CHRONO

    END
}

void chronoCopyFromInterface(ParametersMap* parametersMap)
{
    START

    Interface interface(buffer->getSize(), buffer->getBufferType());

    START_CHRONO
        buffer->copyFromInterface(&interface);
    STOP_CHRONO

    END
}

void chronoActivation(ParametersMap* parametersMap)
{
    START

    Buffer* results = Factory::newBuffer(buffer->getSize(), BT_FLOAT, buffer->getImplementationType());

    START_CHRONO
        buffer->activation(results, FT_IDENTITY);
    STOP_CHRONO

    delete (results);

    END
}

void chronoClone(ParametersMap* parametersMap)
{
    START

    START_CHRONO
        Buffer* copy = buffer->clone();
        delete (copy);
    STOP_CHRONO

    END
}

int main(int argc, char *argv[])
{
    Chronometer total;
    total.start();
    try {
        Test test;
        test.parameters.putString(Test::PLOT_PATH, PREANN_DIR + to_string("output/"));
        test.parameters.putString(Test::PLOT_X_AXIS, "Size");
        test.parameters.putString(Test::PLOT_Y_AXIS, "Time (seconds)");
        test.parameters.putNumber(Dummy::WEIGHS_RANGE, 20);
        test.parameters.putNumber(Test::REPETITIONS, 100);
        test.parameters.putNumber(Enumerations::enumTypeToString(ET_FUNCTION), FT_IDENTITY);

        EnumLoop* implTypeLoop = new EnumLoop(Enumerations::enumTypeToString(ET_IMPLEMENTATION),
                                              ET_IMPLEMENTATION);
        test.addLoop(implTypeLoop);

        EnumLoop* bufferTypeLoop = new EnumLoop(Enumerations::enumTypeToString(ET_BUFFER), ET_BUFFER);
        test.addLoop(bufferTypeLoop);

        test.parameters.putPtr(Test::LINE_COLOR, implTypeLoop);
        test.parameters.putPtr(Test::POINT_TYPE, bufferTypeLoop);

        test.getLoop()->print();

        test.plot(chronoCopyToInterface, "Buffer_copyToInterface", Dummy::SIZE, 2000, 20001, 2000);
        test.plot(chronoCopyFromInterface, "Buffer_copyFromInterface", Dummy::SIZE, 2000, 20001, 2000);
        test.plot(chronoClone, "Buffer_clone", Dummy::SIZE, 1000, 10001, 3000);

        // exclude BYTE
        bufferTypeLoop->exclude(ET_BUFFER, 1, BT_BYTE);
        test.getLoop()->print();

        test.plot(chronoActivation, "Buffer_activation", Dummy::SIZE, 2000, 20001, 2000);

        printf("Exit success.\n");
    } catch (std::string error) {
        cout << "Error: " << error << endl;
        //	} catch (...) {
        //		printf("An error was thrown.\n", 1);
    }

    MemoryManagement::printTotalAllocated();
    MemoryManagement::printTotalPointers();
    //MemoryManagement::mem_printListOfPointers();
    total.stop();
    printf("Total time spent: %f \n", total.getSeconds());
    return EXIT_SUCCESS;
}
