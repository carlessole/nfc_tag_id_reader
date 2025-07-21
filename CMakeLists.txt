cmake_minimum_required(VERSION 3.23)
project(nfc_tag_id_reader_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Look libnfc with pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNFC REQUIRED IMPORTED_TARGET libnfc)

# Output bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add source files
add_executable(nfc_tag_id_reader_cpp src/main.cpp src/NFCHandler.cpp)

target_include_directories(nfc_tag_id_reader_cpp PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# Link with target pkg-config library
target_link_libraries(nfc_tag_id_reader_cpp PRIVATE
    PkgConfig::LIBNFC
)

# Install output
install(TARGETS nfc_tag_id_reader_cpp
        RUNTIME DESTINATION bin)