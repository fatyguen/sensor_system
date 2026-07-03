#pragma once
#include <deque>

class MovingAverageFilter {
private:
    std::deque<double> window;
    int size;
    double sum = 0;

public:
    MovingAverageFilter(int size);

    double filter(double value);
};