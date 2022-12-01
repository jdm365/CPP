cmake_minimum_required(VERSION "3.16.3")

project("GradientBoostedTrees")

add_executable("${PROJECT_NAME}" "src/main.cpp")

install(TARGETS "${PROJECT_NAME}" DESTINATION bin)
install(FILES "src/main.cpp" DESTINATION src)
