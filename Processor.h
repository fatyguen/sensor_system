#pragma once
#include "Sensor.h"
#include "Filter.h"
#include "AnomalyDetector.h"
#include "Logger.h"
#include <vector>



class Processor {
private:
    std::vector<double> latencies;
    Sensor sensor;
    MovingAverageFilter tempFilter;
    MovingAverageFilter pressureFilter;
    MovingAverageFilter accelFilter;
    AnomalyDetector tempDetector;
    AnomalyDetector pressureDetector;
    AnomalyDetector accelDetector;
    Logger& logger;

public:
    Processor(int window, double threshold, Logger& logger);

    void processLoop(int runtimeSeconds, int delayMs);
};