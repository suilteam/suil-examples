cmake_minimum_required(VERSION 3.8)

set(SUIL_BASE_PATH "" CACHE STRING "Path to an install of suil package")

if (SUIL_BASE_PATH)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SUIL_BASE_PATH}/share/cmake/Modules")
endif (SUIL_BASE_PATH)

set(CMAKE_CXX_STANDARD 17)
include(Suil)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(intkey_VERSION        0.0.0 CACHE STRING     "Current application version")

set(intkey_DEFINES -DAPI_VERSION=\"${intkey_VERSION}\")

if (SUIL_BUILD_DEV_PACKAGE)
    set(_SCC_BIN     ${CMAKE_BINARY_DIR}/tools/scc)
    set(_IOD_BIN     ${CMAKE_BINARY_DIR}/deps/iod/iodsyms)
    set(_GENTPS_BIN  ${CMAKE_BINARY_DIR}/tools/suiltps)
endif()

SuilProject(intkey
        SCC_BIN     ${_SCC_BIN}
        IOD_BIN     ${_IOD_BIN}
        GENTPS_BIN  ${_GENTPS_BIN})

set(intkey_SOURCES
        src/main.cc
        src/addresser.cpp)

SuilApp(intkey
        SOURCES   ${intkey_SOURCES}
        VERSION   ${intkey_VERSION}
        DEFINES   ${intkey_DEFINES}
        INSTALL   ON
        LIBRARIES sawtooth
        INSTALL_DIRS res)

file(GLOB intkey_TEST_SOURCES tests/*.cpp)
SuilTest(intkey
        SOURCES ${intkey_TEST_SOURCES} tests/main.cc
        VERSION ${intkey_VERSION}
        DEFINES ${intkey_DEFINES})
