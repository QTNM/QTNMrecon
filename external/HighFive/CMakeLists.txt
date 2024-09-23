cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

project(HighFive VERSION 3.0.0)
set(HIGHFIVE_VERSION_PRERELEASE 1)

# Configure HighFive
# ------------------
option(HIGHFIVE_VERBOSE "Set logging level to verbose." OFF)

# Controls if HighFive classes are friends of each other.
#
# There are two compiler bugs that require incompatible choices. The
# GCC compiler bug [1] prevents us from writing:
#
#     template<class D>
#     friend class NodeTraits<D>;
#
# While a MSVC compiler bug [2] complains that it can't access a
# protected constructor, e.g., `HighFive::Object::Object`.
#
# Starting with `2.7.0` these friend declarations don't matter
# anymore. It's mearly a means of appeasing a compiler.
#
# The values of `HIGHFIVE_HAS_FRIEND_DECLARATIONS` are:
#   - that the macro is undefined.
#   - `0` which implies not adding the friend declarations.
#   - any non-zero integer, i.e. `1`, to add the friend declarations.
#
# Not defining the macro implies that it'll be set to `1` if MSVC is
# detected (or other compilers requiring the friend declarations).
#
# [1]: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52625
# [2]: https://developercommunity.visualstudio.com/t/MSVC-compiler-improperly-implements-N489/1516410
option(HIGHFIVE_HAS_FRIEND_DECLARATIONS "Enable additional friend declarations. Certain compiler require this On, others Off." OFF)
mark_as_advanced(HIGHFIVE_HAS_FRIEND_DECLARATIONS)

option(HIGHFIVE_FIND_HDF5 "Find and link with HDF5." ON)

# HighFive
# --------
add_library(HighFiveInclude INTERFACE)
add_library(HighFive::Include ALIAS HighFiveInclude)
set_target_properties(HighFiveInclude PROPERTIES EXPORT_NAME Include)

target_include_directories(HighFiveInclude INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

add_library(HighFive INTERFACE)
add_library(HighFive::HighFive ALIAS HighFive)

target_link_libraries(HighFive INTERFACE HighFive::Include)

if(HIGHFIVE_FIND_HDF5)
  find_package(HDF5 REQUIRED)
  target_link_libraries(HighFive INTERFACE HDF5::HDF5)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/highfive/H5Version.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/include/highfive/H5Version.hpp)
