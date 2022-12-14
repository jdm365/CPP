cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(histogram_test LANGUAGES CXX CUDA)

enable_language(CUDA)

file(GLOB all_SRCS
		"${PROJECT_SOURCE_DIR}/../src/gbm.cpp"
		"${PROJECT_SOURCE_DIR}/../src/tree.cpp"
		"${PROJECT_SOURCE_DIR}/../src/node.cpp"
		"${PROJECT_SOURCE_DIR}/../src/utils.cpp"
		"${PROJECT_SOURCE_DIR}/../src/feature_histograms.cpp"
		"${PROJECT_SOURCE_DIR}/../src/read_data.cpp"
		"${PROJECT_SOURCE_DIR}/../src/histogram_mapping.cpp"
		"${PROJECT_SOURCE_DIR}/../src/feature_histograms.cpp"
		"${PROJECT_SOURCE_DIR}/main_build.cpp"
		"${PROJECT_SOURCE_DIR}/histogram.cu"
        )

set(CMAKE_CXX_FLAGS "-Wall -O3 -ffast-math -fopenmp -fPIC -march=native")
add_executable(histogram ${all_SRCS})
target_include_directories(histogram PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "../${CMAKE_CURRENT_SOURCE_DIR}/include")
file(COPY ../data DESTINATION ${CMAKE_BINARY_DIR})
