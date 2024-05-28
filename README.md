# QTNMrecon

Proposal for a reconstruction pipeline for QTNM. This is currently a building site.

Build depends on ROOT: easiest build environment at Warwick, source from cvmfs.

source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc13-opt/setup.sh

which delivers ROOT version 6.30, built with C++20. The header-only dependencies also depend on C++20
with a modern compiler, i.e. GCC13 is a minimum. Some parts compile on Apple-clang version 15 and GCC12 but
all together fail eventually. Only GCC13 covers all dependencies.

There are two small libraries to be built, the tools and the modules. Each is required to build an app,
i.e. a reconstruction pipeline code. There are currently 2 apps in the apps folder as examples on how
to use the pipeline. The tools are considered fairly complete, and they would be used in modules which
are meant to run in the pipeline. The modules folder holds examples for a few of the tools as well as
examples for reader modules (sources) and pipeline sinks like a writer or a print to screen.

