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

if(WIN32)
    set(WINDOWS true)
    add_definitions(-DWINDOWS)
endif()

if(UNIX)
    if (CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(LINUX TRUE)
        add_definitions(-DLINUX)
        add_definitions(-DPOSIX)
    endif()

    if(APPLE)
        if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
            set(MACOSX TRUE)
            add_definitions(-DMACOSX)
            add_definitions(-DPOSIX)
        endif()
    endif()
endif()

if (WINDOWS)
    message(STATUS "Windows")
endif()

if (LINUX)
    message(STATUS "Linux")
endif()

if (MACOSX)
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



aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)

if (WINDOWS)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Win SRC_LIST)
endif()

if (Posix)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Posix SRC_LIST)
endif()


set(HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/ChunkAllocator.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ObjectStorage.h"
    )


add_custom_target(main DEPENDS ${APPLICATION_NAME})
add_executable(${APPLICATION_NAME} ${SRC_LIST} ${HEADERS})

