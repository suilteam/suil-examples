cmake_minimum_required(VERSION 3.8)

set(SUIL_BASE_PATH "" CACHE STRING "Path to an install of suil package")

if (SUIL_BASE_PATH)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SUIL_BASE_PATH}/share/cmake/Modules")
endif (SUIL_BASE_PATH)

set(CMAKE_CXX_STANDARD 17)
include(Suil)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(APP_VERSION        0.0.0 CACHE STRING     "Current application version")
set(API_VERSION        v1    CACHE STRING     "Current application version")

set(zmq_DEFINES -DAPI_VERSION=\"${API_VERSION}\")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SUIL_BUILD_DEBUG ON)
endif()

if (SUIL_BUILD_DEV_PACKAGE)
    set(_SCC_BIN     ${CMAKE_BINARY_DIR}/tools/scc)
    set(_IOD_BIN     ${CMAKE_BINARY_DIR}/deps/iod/iodsyms)
    set(_GENTPS_BIN  ${CMAKE_BINARY_DIR}/tools/suiltps)
endif()

SuilProject(zmqb
        SCC_BIN     ${_SCC_BIN}
        IOD_BIN     ${_IOD_BIN}
        GENTPS_BIN  ${_GENTPS_BIN})

set(zmq_SCC_SOURCES)

set(zmq_SOURCES
        src/main.cc)

SuilApp(zmqb
        SOURCES      ${zmq_SOURCES}
        VERSION      ${APP_VERSION}
        DEFINES      ${zmq_DEFINES}
        INSTALL      ON
        SCC_SOURCES  ${zmq_SCC_SOURCES})

file(GLOB zmq_TEST_SOURCES tests/*.cpp)
SuilTest(zmqb
        SOURCES ${zmq_TEST_SOURCES} tests/main.cc
        VERSION ${zmq_VERSION}
        DEFINES ${zmq_DEFINES})
