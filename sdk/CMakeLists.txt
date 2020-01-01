cmake_minimum_required(VERSION 3.8)

set(SUIL_BASE_PATH "" CACHE STRING "Path to an install of suil package")

if (SUIL_BASE_PATH)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SUIL_BASE_PATH}/share/cmake/Modules")
endif (SUIL_BASE_PATH)

set(CMAKE_CXX_STANDARD 17)
include(Suil)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(sdk_VERSION        0.0.0 CACHE STRING     "Current application version")

set(sdk_DEFINES -DAPI_VERSION=\"${sdk_VERSION}\")

if (SUIL_BUILD_DEV_PACKAGE)
    set(_SCC_BIN     ${CMAKE_BINARY_DIR}/tools/scc)
    set(_IOD_BIN     ${CMAKE_BINARY_DIR}/deps/iod/iodsyms)
    set(_GENTPS_BIN  ${CMAKE_BINARY_DIR}/tools/suiltps)
endif()

SuilProject(sdk
        SCC_BIN     ${_SCC_BIN}
        IOD_BIN     ${_IOD_BIN}
        GENTPS_BIN  ${_GENTPS_BIN})

set(sdk_SOURCES
        src/main.cc)

SuilApp(sdk
        SOURCES   ${sdk_SOURCES}
        VERSION   ${sdk_VERSION}
        DEFINES   ${sdk_DEFINES}
        INSTALL   ON)

file(GLOB sdk_TEST_SOURCES tests/*.cpp)
SuilTest(sdk
        SOURCES ${sdk_TEST_SOURCES} tests/main.cc
        VERSION ${sdk_VERSION}
        DEFINES ${sdk_DEFINES})
