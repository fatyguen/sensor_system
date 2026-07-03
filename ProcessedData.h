#pragma once

#include "Sensor.h"

struct ProcessedData
{
    SensorData sensor;

    bool tempAnomaly = false;
    bool pressureAnomaly = false;
    bool accelAnomaly = false;
};