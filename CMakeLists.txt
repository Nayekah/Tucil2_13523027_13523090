cmake_minimum_required(VERSION 3.10)
project(QuadTreeImageCompression)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Pastikan executable akan ditempatkan di folder "bin" pada project root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

find_package(OpenCV REQUIRED)

# Cari file sumber dari folder src/comps
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/comps/*.cpp")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/header
    ${OpenCV_INCLUDE_DIRS}
)

add_executable(quadtree_compression ${SOURCES})
target_link_libraries(quadtree_compression ${OpenCV_LIBS})