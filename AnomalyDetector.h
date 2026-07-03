#pragma once

#include <vector>

class AnomalyDetector
{
private:
    std::vector<double> history;
    int maxSize;
    double threshold;

public:
    AnomalyDetector(int maxSize, double threshold);

    bool isAnomaly(double value);
};