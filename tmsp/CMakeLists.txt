cmake_minimum_required(VERSION 3.8)

set(SUIL_BASE_PATH "" CACHE STRING "Path to an install of suil package")

if (SUIL_BASE_PATH)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SUIL_BASE_PATH}/share/cmake/Modules")
endif (SUIL_BASE_PATH)

set(CMAKE_CXX_STANDARD 17)
include(Suil)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(counter_VERSION        0.0.0 CACHE STRING     "Current application version")

set(counter_DEFINES
        "-DAPI_VERSION=\"${counter_VERSION}\"")

SuilProject(counter)

set(counter_SOURCES
        src/counter.cpp
        src/main.cc)

SuilApp(counter
        SOURCES   ${counter_SOURCES}
        VERSION   ${counter_VERSION}
        DEFINES   ${counter_DEFINES}
        INSTALL   ON
        INSTALL_DIRS res)