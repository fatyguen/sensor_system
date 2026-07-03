#include "Processor.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include <vector>

Processor::Processor(int window, double threshold, Logger& logger)
    : tempFilter(window),
      pressureFilter(window),
      accelFilter(window),
      tempDetector(50, 2.5),
      pressureDetector(50, 3.5),
      accelDetector(50, 3.0),
      logger(logger)
{}

void Processor::processLoop(int runtimeSeconds, int delayMs) {
    auto start = std::chrono::steady_clock::now();

    while (true) {

        auto loopStart = std::chrono::steady_clock::now();

        // check runtime
        auto now = std::chrono::steady_clock::now();
        int elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
        if (elapsed >= runtimeSeconds) break;

        // sensor generation
        SensorData data = sensor.generate();

        // filtering
        double t = tempFilter.filter(data.temperature);
        double p = pressureFilter.filter(data.pressure);
        double a = accelFilter.filter(data.acceleration);

        // anomaly detection
        bool tempAnomaly = tempDetector.isAnomaly(t);
        bool pressureAnomaly = pressureDetector.isAnomaly(p);
        bool accelAnomaly = accelDetector.isAnomaly(a);

        bool anomaly = tempAnomaly ||
                       pressureAnomaly ||
                       accelAnomaly;

        // console message
        std::stringstream ss;
        ss << "[T=" << t
           << " | P=" << p
           << " | A=" << a
           << "]";

        if (anomaly) {
            ss << "  [WARNING] ";

            if (tempAnomaly) ss << "Temperature ";
            if (pressureAnomaly) ss << "Pressure ";
            if (accelAnomaly) ss << "Acceleration ";

            ss << "Anomaly";
        }

        logger.log(ss.str());

        // CSV logging
        logger.logCSV(
            data.timestamp,
            t,
            p,
            a,
            tempAnomaly,
            pressureAnomaly,
            accelAnomaly
        );

        // latency timing (exclude sleep)
        auto loopEnd = std::chrono::steady_clock::now();

        double latency =
            std::chrono::duration<double, std::milli>(loopEnd - loopStart).count();

        latencies.push_back(latency);

        std::cout << "Latency: " << latency << " ms\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    }

    // safety check
    if (latencies.empty()) {
        std::cout << "No samples collected.\n";
        return;
    }

    // performance stats
    double sum = 0.0;
    double minL = latencies[0];
    double maxL = latencies[0];

    for (double l : latencies) {
        sum += l;
        if (l < minL) minL = l;
        if (l > maxL) maxL = l;
    }

    double avg = sum / latencies.size();

    std::cout << "\n========================\n";
    std::cout << "Performance Report\n";
    std::cout << "Samples: " << latencies.size() << "\n";
    std::cout << "Avg latency: " << avg << " ms\n";
    std::cout << "Min latency: " << minL << " ms\n";
    std::cout << "Max latency: " << maxL << " ms\n";
    std::cout << "========================\n";
}