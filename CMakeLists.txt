cmake_minimum_required(VERSION 3.5.1)
project(tools)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Release)

add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -ansi -pedantic -fno-rtti)

option(BUILD_EXAMPLES "Build example code" ON)

include_directories("${PROJECT_SOURCE_DIR}/include")
set(SOURCE "")

add_subdirectory("${PROJECT_SOURCE_DIR}/lib")
add_library(tools SHARED ${SOURCE})
install(TARGETS tools DESTINATION /usr/local/lib)
install(DIRECTORY include/ DESTINATION /usr/local/include/ FILES_MATCHING PATTERN "*.h")
install(DIRECTORY include/ DESTINATION /usr/local/include/ FILES_MATCHING PATTERN "*.h")
install(DIRECTORY include/ DESTINATION /usr/local/include/ FILES_MATCHING PATTERN "*.h")
install(DIRECTORY include/ DESTINATION /usr/local/include/ FILES_MATCHING PATTERN "*.h")

if(${BUILD_EXAMPLES})
	add_executable(i8n examples/i8n/main.cpp)
	target_link_libraries(i8n tools)
	add_executable(system examples/system/main.cpp)
	target_link_libraries(system tools)
endif()
