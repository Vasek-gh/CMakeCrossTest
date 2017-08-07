cmake_minimum_required(VERSION 2.8)

project(CMakeCrossTest)

set(CMAKE_CXX_STANDARD 14)
set(APPLICATION_NAME "CMakeCrossTest")

################################################################################
#
# Config detect
#
################################################################################

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DEBUG true)
    set(RELEASE false)
    add_definitions(-DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(DEBUG false)
    set(RELEASE true)
    add_definitions(-DRELEASE)
else()
    message(FATAL_ERROR "Unknown build type")
endif()

if(DEBUG)
    message(STATUS "Mode - debug")
endif()

if(RELEASE)
    message(STATUS "Mode - release")
endif()

################################################################################
#
# Bit depth detect
#
################################################################################

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    set(AM32 false)
    set(AM64 true)
    add_definitions(-DAM64)
elseif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
    set(AM32 true)
    set(AM64 false)
    add_definitions(-DAM32)
else()
    message(FATAL_ERROR "Unknown adress model")
endif()

if(AM32)
    message(STATUS "Adress model - 32bit")
endif()

if(AM64)
    message(STATUS "Adress model - 64bit")
endif()

################################################################################
#
# OS detect
#
################################################################################

if (WIN32)
    set(OS_WINDOWS true)
    add_definitions(-DOS_WINDOWS)
endif()

if (UNIX)
    if (CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(POSIX TRUE)
        set(OS_LINUX TRUE)
        add_definitions(-DOS_LINUX)
        add_definitions(-DOS_POSIX)
    endif()

    if(APPLE)
        if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
            set(POSIX TRUE)
            set(OS_MACOSX TRUE)
            add_definitions(-DOS_MACOSX)
            add_definitions(-DOS_POSIX)
        endif()
    endif()
endif()

if (OS_WINDOWS)
    message(STATUS "Windows")
endif()

if (OS_LINUX)
    message(STATUS "Linux")
endif()

if (OS_MACOSX)
    message(STATUS "Mac OS X")
endif()

if (POSIX)
    message(STATUS "Posix")
endif()


################################################################################
#
# Compiler detect
#
################################################################################

if(MINGW)
    add_definitions(-DMINGW)
    message(STATUS "MinGw")
    set(CMAKE_CXX_FLAGS "-fnon-call-exceptions -fexceptions")
    set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static")
endif()

if(MSVC)
    add_definitions(-DMSVC)
    message(STATUS "MSVC")
    set(CMAKE_CXX_FLAGS "/EHsc")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-DGCC)
    message(STATUS "GCC")
    set(CMAKE_CXX_FLAGS "-fnon-call-exceptions -fexceptions -pthread")
endif()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Test SRC_LIST)

if (OS_WINDOWS)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Win SRC_LIST)
endif()

if (POSIX)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Posix SRC_LIST)
endif()


set(HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/Memory.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Collections/Base.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Collections/Consts.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Collections/DlList.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Collections/Queue.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Collections/SlList.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Collections/Stack.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ObjectStorage.h"
    )


add_custom_target(main DEPENDS ${APPLICATION_NAME})
add_executable(${APPLICATION_NAME} ${SRC_LIST} ${HEADERS})

