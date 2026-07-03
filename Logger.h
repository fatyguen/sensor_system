#pragma once

#include <fstream>
#include <mutex>
#include <string>

class Logger
{
private:
    std::mutex mtx;
    std::ofstream csvFile;

public:
    Logger();
    ~Logger();

    void log(const std::string& message);

    void logCSV(
        long timestamp,
        double temperature,
        double pressure,
        double acceleration,
        bool tempAnomaly,
        bool pressureAnomaly,
        bool accelAnomaly
    );
};