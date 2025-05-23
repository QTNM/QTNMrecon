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
add_subdirectory(external/HighFive)

#Require ROOT, find installed root
find_package(ROOT REQUIRED Minuit)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ROOT_CXX_FLAGS}")

# find the headers
include_directories(include 
 include/signalgen 
 include/utils
 include/yap
 include/receiver
 include/modules
 external/gsl/include
 external/HighFive/include
 external/mp-units/src/core/include
 external/mp-units/src/systems/include
 ${ROOT_INCLUDES})

# Build a dynamic tool library from our sources
add_library(QTNMTools SHARED src/digitizer.cpp 
src/dsp.cpp 
src/filter.cpp
src/spline.cpp
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
modules/FullKinematicsSimReader.cpp
modules/FullAntennaSimReader.cpp
modules/QTNMSimKinematicsReader.cpp 
modules/xCsvWriter.cpp 
modules/x2CsvWriter.cpp 
modules/digiCsvWriter.cpp 
modules/SineSigGenerator.cpp 
modules/AddChirpToTruth.cpp
modules/OmegaBeatToTruth.cpp
modules/AntennaResponse.cpp
modules/WaveformSampling.cpp 
modules/AddNoise.cpp
modules/Amplifier.cpp
modules/Mixer.cpp
modules/Digitize.cpp
modules/TestG4AntGenerator.cpp
modules/FakeG4AntWriter.cpp
modules/writeWfmToRoot.cpp
modules/writeDigitizerToRoot.cpp
modules/writeHitDigiToRoot.cpp
modules/writeHitDigiToHDF5.cpp
modules/WfmReader.cpp
src/trackMerger.cpp
modules/printSimReader.cpp)
target_link_libraries(ReconModules QTNMTools HighFive ${ROOT_LIBRARIES})

# Build apps and examples
add_executable(reconmerger.exe apps/reconAntWithMerger.cpp)
target_link_libraries(reconmerger.exe PUBLIC ReconModules QTNMTools)
add_executable(antrecon.exe apps/antennaSimRecon.cpp)
target_link_libraries(antrecon.exe PUBLIC ReconModules QTNMTools)
add_executable(noantrecon.exe apps/noAntennaSimRecon.cpp)
target_link_libraries(noantrecon.exe PUBLIC ReconModules QTNMTools)
add_executable(antreconHDF5.exe apps/antennaSimReconToHDF5.cpp)
target_link_libraries(antreconHDF5.exe PUBLIC ReconModules QTNMTools)
add_executable(noantreconHDF5.exe apps/noAntennaSimReconToHDF5.cpp)
target_link_libraries(noantreconHDF5.exe PUBLIC ReconModules QTNMTools)

add_executable(minantrecon.exe apps/examples/minimalFirstStepRecon.cpp)
target_link_libraries(minantrecon.exe PUBLIC ReconModules QTNMTools)
add_executable(minnoantrecon.exe apps/examples/minimalNoAntennaRecon.cpp)
target_link_libraries(minnoantrecon.exe PUBLIC ReconModules QTNMTools)
add_executable(testReconMerged.exe apps/examples/test_writeFakeG4.cpp)
target_link_libraries(testReconMerged.exe PUBLIC ReconModules QTNMTools)
add_executable(testTrackMerge.exe apps/examples/test_trackMerger.cpp)
target_link_libraries(testTrackMerge.exe PUBLIC ReconModules QTNMTools)
add_executable(testG4out.exe apps/examples/test_fakeG4_ToRoot.cpp)
target_link_libraries(testG4out.exe PUBLIC ReconModules QTNMTools)
add_executable(testG4toWfm.exe apps/examples/test_readFakeG4ToWfm.cpp)
target_link_libraries(testG4toWfm.exe PUBLIC ReconModules QTNMTools)
add_executable(exwrhdf5.exe apps/examples/example_write_hdf5.cpp)
target_link_libraries(exwrhdf5.exe PUBLIC ReconModules QTNMTools)
add_executable(exhdwrite.exe apps/examples/example_hitdigi_write.cpp)
target_link_libraries(exhdwrite.exe PUBLIC ReconModules QTNMTools)
add_executable(exhdprint.exe apps/examples/example_hitdigi_print.cpp)
target_link_libraries(exhdprint.exe PUBLIC ReconModules QTNMTools)
add_executable(exdw.exe apps/examples/example_digi_write.cpp)
target_link_libraries(exdw.exe PUBLIC ReconModules QTNMTools)
add_executable(exdp.exe apps/examples/example_digi_print.cpp)
target_link_libraries(exdp.exe PUBLIC ReconModules QTNMTools)
add_executable(exrp.exe apps/examples/example_read_print.cpp)
target_link_libraries(exrp.exe PUBLIC ReconModules QTNMTools)
add_executable(exsc.exe apps/examples/example_sine_csv.cpp)
target_link_libraries(exsc.exe PUBLIC ReconModules QTNMTools)
add_executable(exrip.exe apps/examples/example_interpolate_print.cpp)
target_link_libraries(exrip.exe PUBLIC ReconModules QTNMTools)
add_executable(exnoant.exe apps/examples/example_noAnt_read.cpp)
target_link_libraries(exnoant.exe PUBLIC ReconModules QTNMTools)
add_executable(noantwrite.exe apps/examples/example_noAnt_write.cpp)
target_link_libraries(noantwrite.exe PUBLIC ReconModules QTNMTools)

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
add_executable(testbp examples/bptest.cpp)
target_link_libraries(testbp PUBLIC QTNMTools)
add_executable(testmovav examples/movavtest.cpp)
target_link_libraries(testmovav PUBLIC QTNMTools)
add_executable(testmix examples/mixertest.cpp)
target_link_libraries(testmix PUBLIC QTNMTools)
add_executable(testmixsamp examples/mixsampletest.cpp)
target_link_libraries(testmixsamp PUBLIC QTNMTools)

# utils
add_executable(checkreader utils/readHitDigifile.cpp)
target_link_libraries(checkreader PUBLIC QTNMTools ${ROOT_LIBRARIES})
