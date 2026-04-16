# QTNMrecon

Proposal for a reconstruction pipeline for QTNM.

Build depends on ROOT: easiest build environment at Warwick, source from cvmfs.

source /cvmfs/sft.cern.ch/lcg/views/LCG_109a/x86_64-el9-gcc15-opt/setup.sh

which delivers ROOT version 6.38, built with C++23. The header-only dependencies also depend on C++20
with a modern compiler, here GCC15,, with GCC13 a minimum requirment. Some parts compile on Apple-clang version 15
and GCC12 but all together fail eventually. Only GCC13 covers all dependencies.

There are two small libraries to be built, the tools and the modules. Each is required to build an app,
i.e. a reconstruction pipeline code.

There are currently 5 apps in the apps folder and examples on how
to use the pipeline in the apps/examples folder. One app reconstructs QTNMSim output with antenna
waveforms from the simulation, one reconstructs the purely kinematic output data from the separate
QTNMSim branch. Each comes with either ROOT output or HDF5 file output. The final app shows the
operation of the track merger in a complete reconstruction app.

The tools are considered fairly complete, and they would be used in modules which
are meant to run in the pipeline. The modules folder holds examples for a few of the tools as well as
examples for reader modules (sources) and pipeline sinks like a writer or a print to screen.

Update: with the simulation now including secondary electron production, several waveforms could result
for a single event. The reconstruction should be able to merge them into one before adding noise and
subsequent operations, i.e. a track merging module has been included for that step.

The cmake file builds all the individual little test codes from the examples folder. These should demonstrate
the proper functioning of individual tools, at least for the case considered in the example code. They
would typically produce a little output file (csv type) which can be read and plotted with a Python script from
the examples folder for that particular case. Can all be done neater but is one-by-one for now.

One main external dependency to get used to (also for reading code) is the unit system from here

https://github.com/mpusz/mp-units

which is being included in this pipeline reconstruction since our tools mostly represent
physics objects and good unit handling is beneficial. It just needs getting used to when reading and using code.
Note also the gsl, version 1.1 here, external dependency of mp-units in version 2.5.

The pipeline 'yapp' also builds heavily on C++20, from here:

https://github.com/picanumber/yapp

Examples and features can be found on that page. Two more external header-only libraries are in the utils
folder for now and may change in the future. A convenient CSV file reader (https://github.com/d99kris/rapidcsv)
and a spline algorithm for interpolation (https://github.com/ttk592/spline/tree/master). The FFT library from
(https://github.com/jdupuy/dj_fft) has been used and heavily simplified (feature-reduced) since we only
require 1-D FFT with real values, nothing else. The Butterworth filter algorithm comes from here
(https://github.com/adis300/filter-c), and again was simplified to using the low-pass only. The apps
use the command line tool CLI11 (https://github.com/CLIUtils/CLI11). The HDF5 file writer uses the library
(https://github.com/BlueBrain/HighFive) in the external folder.
