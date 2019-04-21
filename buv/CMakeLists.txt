cmake_minimum_required(VERSION 3.2.0)
project(buv)

set(CMAKE_C_FLAGS_DEBUG "-g -std=c11 -Wall -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-std=c11 -Wall -O3")
message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
message("CMAKE_C_FLAGS is ${CMAKE_C_FLAGS}")
message("CMAKE_C_FLAGS_DEBUG is ${CMAKE_C_FLAGS_DEBUG}")
message("CMAKE_C_FLAGS_RELEASE is ${CMAKE_C_FLAGS_RELEASE}")

include_directories(include)
include_directories(/usr/include)
include_directories(/usr/local/include)
link_directories(/usr/lib)
link_directories(/usr/local/lib)

file(GLOB SOURCES "src/*.c")

add_executable(buv ${SOURCES})

target_link_libraries(buv microhttpd sqlite3 z)