#include "Logger.h"
#include <iostream>

Logger::Logger()
{
    csvFile.open("sensor_log.csv");

    csvFile
        << "Timestamp,"
        << "Temperature,"
        << "Pressure,"
        << "Acceleration,"
        << "TempAnomaly,"
        << "PressureAnomaly,"
        << "AccelAnomaly\n";
}

Logger::~Logger()
{
    if (csvFile.is_open())
        csvFile.close();
}

void Logger::log(const std::string& message)
{
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << message << std::endl;
}

void Logger::logCSV(
    long timestamp,
    double temperature,
    double pressure,
    double acceleration,
    bool tempAnomaly,
    bool pressureAnomaly,
    bool accelAnomaly)
{
    std::lock_guard<std::mutex> lock(mtx);

    csvFile
        << timestamp << ","
        << temperature << ","
        << pressure << ","
        << acceleration << ","
        << tempAnomaly << ","
        << pressureAnomaly << ","
        << accelAnomaly << "\n";
}