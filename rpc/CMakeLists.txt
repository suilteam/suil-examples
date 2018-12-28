cmake_minimum_required(VERSION 3.8)

if (SUIL_BASE_PATH)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SUIL_BASE_PATH}/share/cmake/Modules")
endif (SUIL_BASE_PATH)

set(CMAKE_CXX_STANDARD 17)
include(Suil)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(jsonrpc_VERSION        0.0.0 CACHE STRING     "Current application version")

set(jsonrpc_DEFINES
        "-DAPI_VERSION=\"${jsonrpc_VERSION}\"")

SuilProject(jsonrpc
        SCC_BIN  ${CMAKE_BINARY_DIR}/tools/scc)

set(jsonrpc_SOURCES
        src/main.cc
        src/calculator.cpp)

SuilApp(jsonrpc
        SOURCES         ${jsonrpc_SOURCES}
        VERSION         ${jsonrpc_VERSION}
        DEFINES         ${jsonrpc_DEFINES}
        SCC_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/rpc.scc
        SCC_OUTDIR      ${CMAKE_CURRENT_SOURCE_DIR}/src
        INSTALL         ON
        INSTALL_DIRS    res)