cmake_minimum_required(VERSION "3.16.3")
set(CMAKE_CXX_STANDARD 20)

project(GradientBoostedTrees)

file(GLOB all_SRCS
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.c"
	#"${PROJECT_SOURCE_DIR}/python/*.cpp"
	)
add_executable(${PROJECT_NAME} ${all_SRCS})


find_package(PythonLibs 3.9 REQUIRED)
find_package(Boost COMPONENTS python REQUIRED)

set(CMAKE_SHARED_MODULE_PREFIX "")

add_library(GBDT_ MODULE "${PROJECT_SOURCE_DIR}/python/api.cpp")
target_link_libraries(GBDT_ ${Boost_LIBRARIES} ${Python_LIBRARIES})
target_include_directories(GBDT_ PRIVATE ${PYTHON_INCLUDE_DIRS})

include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PYTHON_INCLUDE_DIRS}
	${BOOST_INCLUDE_DIRS}
	)

########################################################################
############################# OPEN MP ##################################
########################################################################
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
	target_link_libraries("${PROJECT_NAME}" PUBLIC OpenMP::OpenMP_CXX)
endif()
########################################################################

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wpedantic -Wextra -Ofast -fopenmp -fPIC -march=native")

## DO NOT ADD -shared TAG. WILL CAUSE SEG FAULTS!!! ##

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/release)
file(COPY data DESTINATION ${CMAKE_BINARY_DIR})

install(TARGETS "${PROJECT_NAME}" RUNTIME DESTINATION "${EXECUTABLE_OUTPUT_PATH}")
