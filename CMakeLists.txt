cmake_minimum_required(VERSION 3.5.1)
project(tools)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Release)

add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -ansi -pedantic -fno-rtti)

option(BUILD_EXAMPLES "Build example code" ON)
option(BUILD_STATIC "By default, a shared library is built. Set to on to switch to a static library" OFF)

include_directories("${PROJECT_SOURCE_DIR}/include")
set(SOURCE "")

add_subdirectory("${PROJECT_SOURCE_DIR}/lib")
if(${BUILD_STATIC})
	add_library(tools STATIC ${SOURCE})
else()
	add_library(tools SHARED ${SOURCE})
endif()

install(TARGETS tools DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

if(${BUILD_EXAMPLES})
	add_executable(i8n examples/i8n/main.cpp)
	target_link_libraries(i8n tools)

	add_executable(system examples/system/main.cpp)
	target_link_libraries(system tools)

	add_executable(json examples/json/main.cpp)
	target_link_libraries(json tools)

	add_executable(file_utils examples/file_utils/main.cpp)
	target_link_libraries(file_utils tools)
	add_custom_command(TARGET file_utils POST_BUILD COMMAND cp -r ../examples/file_utils/*.txt ./)

#	add_executable(menu examples/json/main.cpp)
#	target_link_libraries(menu tools)
#	add_custom_command(TARGET menu POST_BUILD COMMAND cp -r ../examples/menu/menu.json ./)
endif()
