cmake_minimum_required(VERSION "3.16.3")
set(CMAKE_CXX_STANDARD 20)

project(GradientBoostedTrees)


########################################################################
########################### PYTHON LIBRARY #############################
########################################################################
file(GLOB lib_SRCS 
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.c"
	"${PROJECT_SOURCE_DIR}/python/*.cpp"
	)
file(GLOB cuda_SRCS
	"${PROJECT_SOURCE_DIR}/cuda/*.cu"
	)

find_package(PythonLibs 3.9 REQUIRED)
find_package(PythonInterp REQUIRED)
find_package(Boost REQUIRED COMPONENTS python REQUIRED numpy REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Thrust 1.9.10 REQUIRED)
find_package(CUDA 11.8 REQUIRED)

set(CMAKE_SHARED_MODULE_PREFIX "")

include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PYTHON_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${CUDA_INCLUDE_DIRS}
	)

set(CMAKE_CUDA_COMPILER "/usr/local/cuda-11/bin/nvcc")
cuda_add_library(culib SHARED ${cuda_SRCS})
set_target_properties(culib PROPERTIES CUDA_SEPERABLE_COMPILATION ON)

PYTHON_ADD_MODULE(GBDTEngine ${lib_SRCS})
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/python)
install(TARGETS GBDTEngine LIBRARY DESTINATION "${LIBRARY_OUTPUT_PATH}")
target_link_libraries(GBDTEngine PUBLIC 
	OpenMP::OpenMP_CXX 
	${Boost_LIBRARIES} 
	${Python_LIBRARIES}
	${CUDA_LIBRARIES}
	culib
	)
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra -Ofast -fopenmp -fPIC -march=native")
## DO NOT ADD -shared TAG. WILL CAUSE SEG FAULTS!!! ##


########################################################################




########################################################################
########################### MAIN PROJECT ###############################
########################################################################
#file(GLOB all_SRCS
#	"${PROJECT_SOURCE_DIR}/src/*.cpp"
#	"${PROJECT_SOURCE_DIR}/src/*.c"
#	)
#add_library(${PROJECT_NAME} ${all_SRCS})

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/release)
#file(COPY data DESTINATION ${CMAKE_BINARY_DIR})

#install(TARGETS "${PROJECT_NAME}" RUNTIME DESTINATION "${EXECUTABLE_OUTPUT_PATH}")
########################################################################
