#include "Sensor.h"
#include <chrono>

Sensor::Sensor() : noise(0.0, 0.6)
{
    std::random_device rd;
    gen.seed(rd());
}

SensorData Sensor::generate()
{
    auto now = std::chrono::system_clock::now().time_since_epoch();
    long ts =
        std::chrono::duration_cast<std::chrono::milliseconds>(now).count();

    // Normal operating values
    double temperature = 25.0 + noise(gen);
    double pressure = 1.0 + noise(gen) * 0.03;
    double acceleration = 9.81 + noise(gen) * 0.15;

    std::uniform_real_distribution<double> probability(0.0, 1.0);
    double r = probability(gen);

    // About 1% chance each
    if (r < 0.01)
    {
        temperature += 8.0;          // overheating
    }
    else if (r < 0.02)
    {
        pressure += 0.35;            // pressure spike
    }
    else if (r < 0.03)
    {
        acceleration += 2.0;         // impact
    }
    else if (r < 0.04)
    {
        temperature -= 6.0;          // faulty reading
    }

    return {
        temperature,
        pressure,
        acceleration,
        ts
    };
}