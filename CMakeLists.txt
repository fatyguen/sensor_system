cmake_minimum_required(VERSION 3.10)
project(SensorSystem)

set(CMAKE_CXX_STANDARD 17)

add_executable(sensor_system
    main.cpp
    Sensor.cpp
    Filter.cpp
    AnomalyDetector.cpp
    Logger.cpp
    Processor.cpp
)