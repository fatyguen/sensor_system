#include "AnomalyDetector.h"
#include <cmath>
#include <numeric>
#include <iostream>

AnomalyDetector::AnomalyDetector(int maxSize, double threshold)
    : maxSize(maxSize), threshold(threshold)
{
}

bool AnomalyDetector::isAnomaly(double value)
{
    // Build an initial baseline
    if (history.size() < 10)
    {
        history.push_back(value);
        return false;
    }

    // Compute mean
    double mean =
        std::accumulate(history.begin(), history.end(), 0.0)
        / history.size();

    // Compute variance
    double variance = 0.0;

    for (double v : history)
        variance += (v - mean) * (v - mean);

    variance /= history.size();

    // Standard deviation
    double stddev = std::sqrt(variance);


    // Prevent unrealistically small stddev
    if (stddev < 1.0)
        stddev = 1.0;

    double z = std::abs(value - mean) / stddev;

    bool anomaly = (z > threshold);

    // Only update baseline with normal values
    if (!anomaly)
    {
        history.push_back(value);

        if (history.size() > maxSize)
            history.erase(history.begin());
    }

    return anomaly;
}