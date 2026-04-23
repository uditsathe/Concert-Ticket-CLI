cmake_minimum_required(VERSION 3.16)
project(ConcertBooking)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/*.cpp")

add_executable(concert_booking ${SOURCES})

set_target_properties(concert_booking PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

if(UNIX AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(concert_booking stdc++fs)
endif()
