/*
 * test.cpp
 *
 *  Created on: Apr 22, 2011
 *      Author: timon
 */

#include "test.h"

Test::Test()
{
    tLoop = NULL;
}

Test::~Test()
{
    delete (tLoop);
}

Loop* Test::getLoop()
{
    return tLoop;
}

void Test::addLoop(Loop* loop)
{
    if (tLoop == NULL){
        tLoop = loop;
    } else {
        tLoop->addInnerLoop(loop);
    }
}

const string Test::DIFF_COUNT = "__differencesCounter";
const string Test::MEM_LOSSES = "__memoryLosses";
const string Test::REPETITIONS = "__repetitions";
const string Test::TIME_COUNT = "__timeCount";
const string Test::PLOT_LOOP = "__LOOP__PLOT_LOOP";
const string Test::PLOT_X_AXIS = "__LOOP__PLOT_X_AXIS";
const string Test::PLOT_Y_AXIS = "__LOOP__PLOT_Y_AXIS";
const string Test::PLOT_MIN = "__LOOP__PLOT_MIN";
const string Test::PLOT_MAX = "__LOOP__PLOT_MAX";
const string Test::PLOT_INC = "__LOOP__PLOT_INC";
const string Test::LINE_COLOR_LEVEL = "__LOOP__PLOT_LINE_COLOR";
const string Test::POINT_TYPE_LEVEL = "__LOOP__PLOT_POINT_TYPE";
const string Test::PLOT_PATH = "__plotPath";
const string Test::PLOT_FILE = "__plotFile";
const string Test::FIRST_STATE = "__firstState";
const string Test::SUB_PATH = "__subPath";
const string Test::INITIAL_POPULATION = "__initialPopulation";
const string Test::EXAMPLE_INDIVIDUAL = "__exampleIndividual";
const string Test::TASK = "__task_to_plot";
const string Test::MAX_GENERATIONS = "__generations_to_plot";

void Test::check(bool condition, string message)
{
    if (condition){
        cout << message << endl;
        throw message;
    }
}

unsigned char Test::areEqual(float expected, float actual, BufferType bufferType)
{
    if (bufferType == BT_FLOAT) {
        return (expected - 1 < actual && expected + 1 > actual);
    } else {
        return expected == actual;
    }
}

unsigned Test::assertEqualsInterfaces(Interface* expected, Interface* actual)
{
    if (expected->getBufferType() != actual->getBufferType()) {
        throw "The interfaces are not even of the same type!";
    }
    if (expected->getSize() != actual->getSize()) {
        throw "The interfaces are not even of the same size!";
    }
    unsigned differencesCounter = 0;

    for (unsigned i = 0; i < expected->getSize(); i++) {
        if (!areEqual(expected->getElement(i), actual->getElement(i), expected->getBufferType())) {
            printf("The interfaces are not equal at the position %d (expected = %f actual %f).\n", i,
                   expected->getElement(i), actual->getElement(i));
            ++differencesCounter;
        }
    }
    return differencesCounter;
}

unsigned Test::assertEquals(Buffer* expected, Buffer* actual)
{
    if (expected->getBufferType() != actual->getBufferType()) {
        throw "The buffers are not even of the same type!";
    }
    if (expected->getSize() != actual->getSize()) {
        throw "The buffers are not even of the same size!";
    }

    unsigned differencesCounter = 0;
    Interface* expectedInt = expected->toInterface();
    Interface* actualInt = actual->toInterface();

    for (unsigned i = 0; i < expectedInt->getSize(); i++) {
        if (!areEqual(expectedInt->getElement(i), actualInt->getElement(i), expectedInt->getBufferType())) {
            printf("The buffers are not equal at the position %d (expected = %f actual %f).\n", i,
                   expectedInt->getElement(i), actualInt->getElement(i));
            ++differencesCounter;
        }
    }
    delete (expectedInt);
    delete (actualInt);
    return differencesCounter;
}

void Test::checkEmptyMemory(ParametersMap* parametersMap)
{
    if (MemoryManagement::getPtrCounter() > 0 || MemoryManagement::getTotalAllocated() > 0) {

        cout
                << "Memory loss detected while testing " + parametersMap->getString(Loop::LABEL)
                        + " at state " + parametersMap->getString(Loop::STATE) << endl;

        MemoryManagement::printTotalAllocated();
        MemoryManagement::printTotalPointers();
        MemoryManagement::clear();
        unsigned memoryLosses = parametersMap->getNumber(Test::MEM_LOSSES);
        ++memoryLosses;
        parametersMap->putNumber(Test::MEM_LOSSES, memoryLosses);
    }
}

void testAction(void (*f)(ParametersMap*), ParametersMap* parametersMap)
{
    try {
        f(parametersMap);
        unsigned differencesCounter = parametersMap->getNumber(Test::DIFF_COUNT);
        if (differencesCounter > 0) {
            string state = parametersMap->getString(Loop::STATE);
            cout << state << " : " << differencesCounter << " differences detected." << endl;
        }
    } catch (string e) {
        cout
                << " while testing " + parametersMap->getString(Loop::LABEL) + " at state "
                        + parametersMap->getString(Loop::STATE) + " : " + e << endl;
    }
}
void Test::test(void (*func)(ParametersMap*), std::string functionLabel)
{
    cout << "Testing... " << functionLabel << endl;
    tLoop->repeatAction(testAction, func, &parameters, functionLabel);
}

int mapPointType(unsigned value)
{
    // pt : 1=+, 2=X, 3=*, 4=square, 5=filled square, 6=circle,
    //            7=filled circle, 8=triangle, 9=filled triangle, etc.
    switch (value) {
        default:
        case 0:
            return 2;
        case 1:
            return 6;
        case 2:
            return 4;
        case 3:
            return 8;
        case 4:
            return 1;
        case 5:
            return 3;
    }
}
int mapLineColor(unsigned value)
{
    // lt is for color of the points: -1=black 1=red 2=grn 3=blue 4=purple 5=aqua 6=brn 7=orange 8=light-brn
    switch (value) {
        default:
        case 0:
            return 1;
        case 1:
            return 2;
        case 2:
            return 3;
        case 3:
            return 5;
        case 4:
            return -1;
        case 5:
            return 7;
        case 6:
            return 4;
    }
}

int getPointType(ParametersMap* parametersMap)
{
    unsigned pointTypeLevel = 1;
    try {
        pointTypeLevel = parametersMap->getNumber(Test::POINT_TYPE_LEVEL);
    } catch (string e) {
    };
    unsigned pointTypeToMap = 1000;
    try {
        string levelName = Loop::getLevelName(pointTypeLevel);
        cout << parametersMap->printNumber(levelName) << endl;
        pointTypeToMap = parametersMap->getNumber(levelName);
    } catch (string e) {
    };
    int pointType = mapPointType(pointTypeToMap);
    return pointType;
}

int getLineColor(ParametersMap*& parametersMap)
{
    unsigned lineColorLevel = 0;
    try {
        lineColorLevel = parametersMap->getNumber(Test::LINE_COLOR_LEVEL);
    } catch (string e) {
    };
    unsigned lineColorToMap = 1000;
    try {
        string levelName = Loop::getLevelName(lineColorLevel);
        lineColorToMap = parametersMap->getNumber(levelName);
    } catch (string e) {
    };
    int lineColor = mapLineColor(lineColorToMap);
    return lineColor;
}

void preparePlotFunction(ParametersMap* parametersMap)
{
    FILE *plotFile = (FILE*) ((((parametersMap->getPtr(Test::PLOT_FILE)))));
    unsigned first = parametersMap->getNumber(Test::FIRST_STATE);
    if (first) {
        parametersMap->putNumber(Test::FIRST_STATE, 0);
    } else {
        fprintf(plotFile, " , \\\n\t");
    }
    string state = parametersMap->getString(Loop::STATE);
    string subPath = parametersMap->getString(Test::SUB_PATH);
    string dataPath = subPath + state + ".DAT";
    string line = " \"" + dataPath + "\" using 1:2 title \"" + state + "\"";

//    parametersMap->print();
    int lineColor = getLineColor(parametersMap);
    int pointType = getPointType(parametersMap);

    cout << "lineColor " << lineColor << " - pointType " << pointType << endl;

    line += " with linespoints lt " + to_string(lineColor);
    line += " pt " + to_string(pointType);

    fprintf(plotFile, "%s", line.data());
}
void Test::createGnuPlotScript(ParametersMap* parametersMap, string functionLabel)
{
    string path = parametersMap->getString(Test::PLOT_PATH);
    string xAxis = parametersMap->getString(PLOT_X_AXIS);
    string yAxis = parametersMap->getString(PLOT_Y_AXIS);

    string plotPath = path + "gnuplot/" + functionLabel + ".plt";
    string outputPath = path + "images/" + functionLabel + ".png";

    FILE* plotFile = openFile(plotPath);

    fprintf(plotFile, "set terminal png size 2048,1024 \n");
    fprintf(plotFile, "set key below\n");
    fprintf(plotFile, "set key box \n");

    fprintf(plotFile, "set title \"%s\" \n", functionLabel.data());
    fprintf(plotFile, "set xlabel \"%s\" \n", xAxis.data());
    fprintf(plotFile, "set ylabel \"%s\" \n", yAxis.data());
    fprintf(plotFile, "set output \"%s\" \n", outputPath.data());
    fprintf(plotFile, "plot ");

    unsigned count = 0;
    string subPath = path + "data/" + functionLabel + "_";

    parametersMap->putString(Test::SUB_PATH, subPath);
    parametersMap->putPtr(Test::PLOT_FILE, plotFile);
    parametersMap->putNumber(Test::FIRST_STATE, 1);

    tLoop->repeatFunction(preparePlotFunction, parametersMap, "preparePlotFunction");

    fprintf(plotFile, "\n");
    fclose(plotFile);
}

void plotAction(void (*f)(ParametersMap*), ParametersMap* parametersMap)
{
    try {
        string path = parametersMap->getString(Test::PLOT_PATH);
        string functionLabel = parametersMap->getString(Loop::LABEL);
        string state = parametersMap->getString(Loop::STATE);
        unsigned repetitions = parametersMap->getNumber(Test::REPETITIONS);

        string dataPath = path + "data/" + functionLabel + "_" + state + ".DAT";
        FILE* dataFile = openFile(dataPath);
        string plotVar = parametersMap->getString(Test::PLOT_LOOP);
        fprintf(dataFile, "# %s %s \n", plotVar.data(), state.data());

        float min = parametersMap->getNumber(Test::PLOT_MIN);
        float max = parametersMap->getNumber(Test::PLOT_MAX);
        float inc = parametersMap->getNumber(Test::PLOT_INC);

        for (float i = min; i < max; i += inc) {
            parametersMap->putNumber(plotVar, i);

            f(parametersMap);

            float totalTime = parametersMap->getNumber(Test::TIME_COUNT);
            fprintf(dataFile, " %f %f \n", i, totalTime / repetitions);
        }

        fclose(dataFile);
    } catch (string e) {
        cout
                << " while plotting " + parametersMap->getString(Loop::LABEL) + " at state "
                        + parametersMap->getString(Loop::STATE) + " : " + e << endl;
    }
}
void plotFile(string path, string functionLabel)
{
    string plotPath = path + "gnuplot/" + functionLabel + ".plt";
    string syscommand = "gnuplot " + plotPath;
    system(syscommand.data());
}
void Test::plot(void (*func)(ParametersMap*), std::string functionLabel, std::string plotVarKey, float min,
                float max, float inc)
{
    cout << "Plotting " << functionLabel << "...";
    Chronometer chrono;
    chrono.start();
    parameters.putString(PLOT_LOOP, plotVarKey);
    parameters.putNumber(PLOT_MIN, min);
    parameters.putNumber(PLOT_MAX, max);
    parameters.putNumber(PLOT_INC, inc);

    createGnuPlotScript(&parameters, functionLabel);

    tLoop->repeatAction(plotAction, func, &parameters, functionLabel);

    string path = parameters.getString(Test::PLOT_PATH);
    plotFile(path, functionLabel);
    chrono.stop();
    cout << chrono.getSeconds() << " segundos." << endl;
}

void plotTaskFunction(ParametersMap* parametersMap)
{
    string state = parametersMap->getString(Loop::STATE);
    try {
        string path = parametersMap->getString(Test::PLOT_PATH);

        Population* initialPopulation = (Population*) parametersMap->getPtr(Test::INITIAL_POPULATION);
        Population* population = new Population(initialPopulation);
        population->setParams(parametersMap);

        Task* task = population->getTask();
        string dataPath = path + "data/" + task->toString() + "_" + state + ".DAT";
        FILE* dataFile = openFile(dataPath);
        string plotVar = "generation";
        fprintf(dataFile, "# %s %s \n", plotVar.data(), state.data());

        float maxGenerations = parametersMap->getNumber(Test::MAX_GENERATIONS);
        for (unsigned i = 0; i < maxGenerations; ++i) {
            float fitness = population->getBestIndividualScore();
            fprintf(dataFile, " %d %f \n", i, fitness);
            population->nextGeneration();
        }
        fclose(dataFile);
        delete (population);
    } catch (string e) {
        cout << " while plotting " + parametersMap->getString(Loop::LABEL) + " at state " + state + " : " + e
                << endl;
    }
}
void Test::plotTask(unsigned maxGenerations)
{
    Task* task = (Task*) parameters.getPtr(Test::TASK);
    string testedTask = task->toString();
    cout << "Plotting " << testedTask << " with goal " << task->getGoal() << "..." << endl;
    Chronometer chrono;
    chrono.start();

    parameters.putNumber(Test::MAX_GENERATIONS, maxGenerations);
    Individual* example = (Individual*) parameters.getPtr(Test::EXAMPLE_INDIVIDUAL);
    unsigned populationSize = parameters.getNumber(Population::SIZE);
    float weighsRange = parameters.getNumber(Dummy::WEIGHS_RANGE);
    Population* initialPopulation = new Population(task, example, populationSize, weighsRange);
    parameters.putPtr(Test::INITIAL_POPULATION, initialPopulation);

    createGnuPlotScript(&parameters, testedTask);

    tLoop->repeatFunction(plotTaskFunction, &parameters, testedTask);

    delete (initialPopulation);

    string path = parameters.getString(Test::PLOT_PATH);
    plotFile(path, testedTask);
    chrono.stop();
    cout << chrono.getSeconds() << " segundos." << endl;
}


