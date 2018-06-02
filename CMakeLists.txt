cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# if (NOT CMAKE_BUILD_TYPE)
#     set(CMAKE_BUILD_TYPE Debug CACHE STRING
#         "None Debug Release RelWithDebInfo MinSizeRel"
#         FORCE)
# endif()

set(CMAKE_BUILD_TYPE Debug)

# if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -fsanitize=undefined -fsanitize=address -fsanitize=leak")
# endif()

project(freepoi)

find_package(Qt5 REQUIRED COMPONENTS Core Network)

add_subdirectory(src)
# add_subdirectory(test)
