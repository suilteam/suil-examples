cmake_minimum_required(VERSION 3.8)

if (SUIL_BASE_PATH)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SUIL_BASE_PATH}/share/cmake/Modules")
endif (SUIL_BASE_PATH)

set(CMAKE_CXX_STANDARD 17)
include(Suil)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(email_VERSION        0.0.0 CACHE STRING     "Current application version")

set(email_DEFINES
        "-DAPI_VERSION=\"${email_VERSION}\"")

SuilProject(email)

set(email_SOURCES
        src/main.cc)

SuilApp(email
        SOURCES   ${email_SOURCES}
        VERSION   ${email_VERSION}
        DEFINES   ${email_DEFINES}
        INSTALL   ON
        INSTALL_DIRS res)
