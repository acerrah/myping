cmake_minimum_required(VERSION 3.12)
project(myping)

# Set source and include directory
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Source files
set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/myping.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/arppacket.cpp)

# Include files
set(INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/src/myping.hpp ${CMAKE_CURRENT_SOURCE_DIR}/src/arppacket.hpp)

add_library(myping STATIC ${SOURCES})

set_target_properties(myping PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Specify the installation directory
install(TARGETS myping
    ARCHIVE DESTINATION /usr/local/lib
    LIBRARY DESTINATION /usr/local/lib
    RUNTIME DESTINATION /usr/local/bin)

install(FILES ${INCLUDES} DESTINATION /usr/local/include)