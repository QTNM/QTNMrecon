# - Basic CMake setup
# Check version meets our requirements
# Declare project, which will configure compiler for us
cmake_minimum_required(VERSION 3.26)
project(QTNMTools)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++20 -O3")

# Build the externals
add_subdirectory(external/gsl)
add_subdirectory(external/fmt)
add_subdirectory(external/mp-units/src)

#Require ROOT, find installed root
find_package(ROOT REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")

# find the headers
include_directories(include 
 include/signalgen 
 include/utils
 include/yap
 include/receiver
 include/modules
 external/gsl/include
 external/mp-units/src/core/include
 external/mp-units/src/systems/include
 ${ROOT_INCLUDES})

# Build a dynamic tool library from our sources
add_library(QTNMTools SHARED src/digitizer.cpp 
src/dsp.cpp 
src/filter.cpp
src/LIA.cpp 
src/CustomNoiseGen.cpp
src/SineGenerator.cpp
src/halfwavedipole.cpp
src/patchantenna.cpp
src/VReceiver.cpp
src/DownConverter.cpp
src/ChirpGenerator.cpp
src/WhiteNoiseGen.cpp)

# module library
add_library(ReconModules SHARED modules/QTNMSimAntennaReader.cpp 
modules/QTNMSimHitReader.cpp 
modules/QTNMSimKinematicsReader.cpp 
modules/xCsvWriter.cpp 
modules/SineSigGenerator.cpp 
modules/AntennaResponse.cpp
modules/WaveformSampling.cpp 
modules/AddNoise.cpp
modules/Digitize.cpp
modules/printSimReader.cpp)
target_link_libraries(ReconModules QTNMTools ${ROOT_LIBRARIES})

# Build apps
add_executable(exrp.exe apps/example_read_print.cpp)
target_link_libraries(exrp.exe PUBLIC ReconModules QTNMTools)
add_executable(exsc.exe apps/example_sine_csv.cpp)
target_link_libraries(exsc.exe PUBLIC ReconModules QTNMTools)
add_executable(exrip.exe apps/example_interpolate_print.cpp)
target_link_libraries(exrip.exe PUBLIC ReconModules QTNMTools)

# Build testing executables separately for file output
add_executable(testbw examples/bwfiltertest.cpp)
target_link_libraries(testbw PUBLIC QTNMTools)
add_executable(testdsp examples/dsptest.cpp)
target_link_libraries(testdsp PUBLIC QTNMTools)
add_executable(testmf examples/mfiltertest.cpp)
target_link_libraries(testmf PUBLIC QTNMTools)
add_executable(testtime examples/djbenchmark.cpp)
target_link_libraries(testtime PUBLIC QTNMTools)
add_executable(testlia examples/liatest.cpp)
target_link_libraries(testlia PUBLIC QTNMTools)
add_executable(testdigi examples/digitest.cpp)
target_link_libraries(testdigi PUBLIC QTNMTools)
add_executable(testnadd examples/sigaddtest.cpp)
target_link_libraries(testnadd PUBLIC QTNMTools)
add_executable(testspline examples/splinetest.cpp)
target_link_libraries(testspline PUBLIC QTNMTools)
add_executable(testEvent examples/tryEvent.cpp)
target_link_libraries(testEvent PUBLIC QTNMTools)
add_executable(testChirp examples/chirptest.cpp)
target_link_libraries(testChirp PUBLIC QTNMTools)
