  sensor_system
Multithreaded C++ sensor pipeline using a producer/consumer architecture with thread-safe queues, moving-average filtering, and z-score anomaly detection.

  Multithreaded Sensor Data Pipeline (C++)

A producer/consumer sensor monitoring pipeline built in modern C++17. Simulated sensor data flows through three concurrent stages — generation, filtering & anomaly detection, and CSV logging — connected by a custom thread-safe queue with clean, deadlock-free shutdown.

   Overview

This project models a real-time sensor monitoring system, the kind of pipeline you'd find in embedded/IoT or industrial telemetry applications. It generates synthetic temperature, pressure, and acceleration readings, smooths them with a moving-average filter, flags statistical anomalies using z-score detection, and logs everything to CSV — all while three threads run concurrently and communicate safely.

   Architecture

```
┌───────────────┐     ┌──────────────────┐     ┌───────────────┐     ┌────────────────┐     ┌───────────────┐
│ Sensor Thread │ --> │ ThreadSafeQueue   │ --> │ Processor      │ --> │ ThreadSafeQueue │ --> │ Logger Thread  │
│ (generate)    │     │ <SensorData>      │     │ Thread         │     │ <ProcessedData> │     │ (write CSV)    │
└───────────────┘     └──────────────────┘     │ filter + detect │     └────────────────┘     └───────────────┘
                                                 └────────────────┘
```

Stage 1 — Sensor Thread
Generates simulated temperature, pressure, and acceleration readings at a fixed interval, with occasional randomized spikes to emulate real-world anomalies (overheating, pressure surges, impacts, faulty readings).

Stage 2 — Processor Thread
Runs anomaly detection on the raw signal (z-score against a rolling baseline that only updates on normal readings, so a spike doesn't poison the baseline it's being compared against), then applies a moving-average filter to smooth the values for logging/display.

Stage 3 — Logger Thread
Writes timestamped rows — filtered sensor values plus per-channel anomaly flags — to `sensor_log.csv`.

Each stage runs on its own `std::thread` and stages are decoupled by `ThreadSafeQueue<T>`, a mutex + condition-variable queue that supports a `close()` call so downstream threads exit cleanly once upstream production stops, instead of blocking forever on an empty queue.

   Features

- Producer/consumer pipeline with three independent, concurrently-running stages
- Thread-safe queue (`ThreadSafeQueue<T>`) with graceful `close()`-based shutdown — no deadlocks, no busy-waiting
- Z-score anomaly detection per channel (temperature, pressure, acceleration) against a rolling statistical baseline
- Moving-average filtering to smooth noisy sensor signals independently of anomaly detection
- CSV logging of raw timestamps, filtered values, and anomaly flags for downstream analysis
- Clean shutdown sequence: an `std::atomic<bool>` stop signal propagates through the queues so all threads exit and `join()` returns without hanging

   Project Structure

```
.
├── main.cpp                Thread setup, wiring, and lifecycle management
├── Sensor.h / .cpp          Simulated sensor data generation
├── Filter.h / .cpp          Moving-average filter
├── AnomalyDetector.h / .cpp     Z-score based anomaly detection
├── ProcessedData.h          Struct combining a sensor reading with its anomaly flags
├── ThreadSafeQueue.h        Generic thread-safe queue used between pipeline stages
├── Logger.h / .cpp          CSV + console logging
├── CMakeLists.txt           Build configuration
└── sensor_log.csv           Example output
```

   Build Instructions

Requires a C++17 compiler and CMake 3.10+.

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

   Usage

Run the compiled binary:

```bash
./sensor_system
```

The pipeline runs for a fixed duration, printing anomaly warnings to the console as they're detected, then shuts down cleanly and writes results to `sensor_log.csv` in the working directory.

Example output row:

| Timestamp | Temperature | Pressure | Acceleration | TempAnomaly | PressureAnomaly | AccelAnomaly |
|---|---|---|---|---|---|---|
| 1723489201123 | 25.4 | 1.01 | 9.83 | 0 | 0 | 0 |

   Design Notes

- Why a custom thread-safe queue? It's the backbone of the producer/consumer pattern here: `push`/`pop` are mutex-protected, and `pop` blocks on a condition variable rather than spinning. `close()` wakes any threads blocked on an empty queue and lets them exit their loop instead of hanging when upstream production stops — this was the fix for an earlier deadlock-on-shutdown bug.
- Why detect anomalies on raw data, then filter for logging? Filtering (smoothing) and anomaly detection are separate concerns applied to separate copies of the value — smoothing raw spikes before detection would blunt the very signal the detector is designed to catch.
- Why exclude anomalous points from the detector's baseline? Feeding a spike into the rolling mean/stddev would drag the baseline toward the anomaly itself, making the detector progressively less sensitive over time.

   Possible Extensions

- Unit tests (GoogleTest/Catch2) for `AnomalyDetector`, `MovingAverageFilter`, and `ThreadSafeQueue`
- GitHub Actions CI workflow to build and test on push
- `SIGINT` handling for graceful interactive shutdown instead of a fixed runtime
- Config-file-driven runtime parameters (sampling rate, window size, z-score threshold)

   License

MIT (or your preferred license — update as needed).
