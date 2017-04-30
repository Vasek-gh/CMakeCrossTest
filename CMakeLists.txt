cmake_minimum_required(VERSION 2.8)

project(CMakeCrossTest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-fnon-call-exceptions -fexceptions")
set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static")
set(APPLICATION_NAME "CMakeCrossTest")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DEBUG true)
    set(RELEASE false)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(DEBUG false)
    set(RELEASE true)
else(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(FATAL_ERROR "Unknown build type")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    set (AM32 false)
    set (AM64 true)
elseif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
    set (AM32 true)
    set (AM64 false)
else("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
    message(FATAL_ERROR "Unknown adress model")
endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")

if(DEBUG)
    add_definitions(-DDEBUG)
    message(STATUS "Mode - debug")
endif()

if(RELEASE)
    add_definitions(-DRELEASE)
    message(STATUS "Mode - release")
endif()

if(AM32)
    add_definitions(-DAM32)
    message(STATUS "Adress model - 32bit")
endif()

if(AM64)
    add_definitions(-DAM64)
    message(STATUS "Adress model - 64bit")
endif()


aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
add_custom_target(main DEPENDS ${APPLICATION_NAME})
add_executable(${APPLICATION_NAME} ${SRC_LIST} ${HEADERS})

