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

target_link_libraries(logtransfer
        sybct64 sybtcl64 sybcs64 sybcomn64 sybintl64 sybunic64
        )

set_target_properties(logtransfer
        PROPERTIES LINK_FLAGS
        -L/home/sybase/OCS-16_0/lib
        )

target_compile_options(logtransfer PRIVATE -m64 PRIVATE -g PRIVATE -DSYB_LP64 PRIVATE -D_REENTRANT PRIVATE -Werror PRIVATE -Wall PRIVATE -Wformat=2)

set(CMAKE_VERBOSE_MAKEFILE ON)
