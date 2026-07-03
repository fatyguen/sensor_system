#pragma once
#include <random>

struct SensorData {
    double temperature;
    double pressure;
    double acceleration;
    long timestamp;
};

class Sensor {
private:
    std::default_random_engine gen;
    std::normal_distribution<double> noise;

public:
    Sensor();

    SensorData generate();
};
