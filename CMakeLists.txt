cmake_minimum_required(VERSION 3.23)
project(nfc_tag_id_reader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add source files
add_executable(nfc_tag_id_reader src/main.cpp)

# Install output
install(TARGETS nfc_tag_id_reader
        RUNTIME DESTINATION bin)