cmake_minimum_required(VERSION "3.16.3")
set(CMAKE_CXX_STANDARD 20)

project(GradientBoostedTrees)

file(GLOB all_SRCS
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.c"
	## "${PROJECT_SOURCE_DIR}/python/*.cpp"
	)
add_executable(${PROJECT_NAME} ${all_SRCS})

include_directories(
	${PROJECT_SOURCE_DIR}/include
	## /usr/include/python3.9/
	)

########################################################################
############################ OPEN MP ###################################
########################################################################
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
	target_link_libraries("${PROJECT_NAME}" PUBLIC OpenMP::OpenMP_CXX)
endif()
########################################################################

#[===[
########################################################################
############################ PYBIND11 ##################################
########################################################################
find_package(Python 3.9 COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)
pybind11_add_module("GBDT" ${all_SRCS})
target_link_libraries("${PROJECT_NAME}" PUBLIC)
set(CMAKE_EXE_LINKER_FLAGS "-lstdc++ -lpython3.9")
install(
	TARGETS "${PROJECT_NAME}"
	COMPONENT python 
	LIBRARY DESTINATION "${PROJECT_SOURCE_DIR}"
	)
########################################################################
]===]

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wpedantic -Wextra -O3 -ffast-math -fopenmp -fPIC -march=native")

## DO NOT ADD -shared TAG. WILL CAUSE SEG FAULTS!!! ##

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/release)
file(COPY data DESTINATION ${CMAKE_BINARY_DIR})

install(TARGETS "${PROJECT_NAME}" RUNTIME DESTINATION "${EXECUTABLE_OUTPUT_PATH}")
