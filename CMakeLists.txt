cmake_minimum_required(VERSION 3.16)

project(logtransfer LANGUAGES C)

message( STATUS "CMAKE_C_COMPILER: " ${CMAKE_C_COMPILER} )

# global needed variables
set(APPLICATION_NAME ${PROJECT_NAME})

# include cmake files
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 14)

set(INCLUDE_DIRECTORIES
        /home/sybase/OCS-16_0/include

        .
        )
include_directories(${INCLUDE_DIRECTORIES})

set(SOURCE_FILES
        ./exutils.h
        ./example.h

        ./exutils.c
        )

add_executable(rpc ${SOURCE_FILES} ./rpc.c)
add_executable(logtransfer ${SOURCE_FILES} ./logtransfer.c)

add_compile_options()
