#include "Filter.h"

MovingAverageFilter::MovingAverageFilter(int size) : size(size) {}

double MovingAverageFilter::filter(double value) {
    window.push_back(value);
    sum += value;

    if (window.size() > size) {
        sum -= window.front();
        window.pop_front();
    }

    return sum / window.size();
}