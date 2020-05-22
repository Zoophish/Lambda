cmake_minimum_required(VERSION 3.15)

project(lambda VERSION 1.0
	DESCRIPTION "A high performance unbiased physically based renderer"
	LANGUAGES CXX)

#Set output folder
set(CMAKE_BINARY_DIR ${PROJECT_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

file(GLOB lambda_SRC "src\*.cpp")

add_library(lambda SHARED ${lambda_SRC})

target_include_directories(lambda PUBLIC "src")
target_include_directories(lambda PRIVATE "third_party")

#Embree
find_package(embree 3.0 REQUIRED)
target_link_libraries(lambda PUBLIC embree)

#Assimp
find_package(assimp REQUIRED)
target_link_libraries(lambda PUBLIC assimp)

set_target_properties(lambda
	PROPERTIES
	CMAKE_CONFIGURE_DEPENDS ON
	WINDOWS_EXPORT_ALL_SYMBOLS ON
)

add_executable(lambda_demo ${PROJECT_SOURCE_DIR}/lambda_demo/main.cpp)

target_link_libraries(lambda_demo PUBLIC lambda)