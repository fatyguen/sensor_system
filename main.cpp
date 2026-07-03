#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

#include "ThreadSafeQueue.h"
#include "Sensor.h"
#include "ProcessedData.h"
#include "Filter.h"
#include "AnomalyDetector.h"
#include "Logger.h"

ThreadSafeQueue<SensorData> sensorQueue;
ThreadSafeQueue<ProcessedData> processedQueue;

std::atomic<bool> running(true);

//--------------------------------------------------
// SENSOR THREAD
//--------------------------------------------------

void sensorThread()
{
    Sensor sensor;

    while (running)
    {
        sensorQueue.push(sensor.generate());
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    sensorQueue.close();
}

//--------------------------------------------------
// PROCESSOR THREAD
//--------------------------------------------------

void processorThread()
{
    MovingAverageFilter tempFilter(5);
    MovingAverageFilter pressureFilter(5);
    MovingAverageFilter accelFilter(5);

    AnomalyDetector tempDetector(50, 5.0);
    AnomalyDetector pressureDetector(50, 5.0);
    AnomalyDetector accelDetector(50, 5.0);

    SensorData data;

    while (sensorQueue.pop(data))
    {
        // Save raw values
        double rawTemp = data.temperature;
        double rawPressure = data.pressure;
        double rawAccel = data.acceleration;

        // Detect anomalies on RAW data
        bool tempAnomaly = tempDetector.isAnomaly(rawTemp);
        bool pressureAnomaly = pressureDetector.isAnomaly(rawPressure);
        bool accelAnomaly = accelDetector.isAnomaly(rawAccel);

        // Filter values for logging/display
        data.temperature = tempFilter.filter(rawTemp);
        data.pressure = pressureFilter.filter(rawPressure);
        data.acceleration = accelFilter.filter(rawAccel);

        if (tempAnomaly || pressureAnomaly || accelAnomaly)
        {
            std::cout << "[WARNING] ";

            if (tempAnomaly)
                std::cout << "Temperature ";

            if (pressureAnomaly)
                std::cout << "Pressure ";

            if (accelAnomaly)
                std::cout << "Acceleration ";

            std::cout
                << "Anomaly | "
                << "T=" << data.temperature
                << " | P=" << data.pressure
                << " | A=" << data.acceleration
                << std::endl;
        }

        ProcessedData output;
        output.sensor = data;
        output.tempAnomaly = tempAnomaly;
        output.pressureAnomaly = pressureAnomaly;
        output.accelAnomaly = accelAnomaly;

        processedQueue.push(output);
    }

    processedQueue.close();
}

//--------------------------------------------------
// LOGGER THREAD
//--------------------------------------------------

void loggerThread()
{
    Logger logger;

    ProcessedData data;

    while (processedQueue.pop(data))
    {
        logger.logCSV(
            data.sensor.timestamp,
            data.sensor.temperature,
            data.sensor.pressure,
            data.sensor.acceleration,
            data.tempAnomaly,
            data.pressureAnomaly,
            data.accelAnomaly
        );
    }
}

//--------------------------------------------------
// MAIN
//--------------------------------------------------

int main()
{
    std::thread sensor(sensorThread);
    std::thread processor(processorThread);
    std::thread logger(loggerThread);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    running = false;

    sensor.join();
    processor.join();
    logger.join();

    std::cout << "\nSystem shutdown complete.\n";

    return 0;
}