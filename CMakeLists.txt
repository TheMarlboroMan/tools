cmake_minimum_required(VERSION 3.5.1)
project(tools)

option(BUILD_EXAMPLES "Build example code" ON)
option(BUILD_DEBUG "Build a debug release" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(${BUILD_DEBUG})
        set(CMAKE_BUILD_TYPE Debug)

else()
        set(CMAKE_BUILD_TYPE Release)
endif()


add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -ansi -pedantic -fno-rtti)

include_directories("${PROJECT_SOURCE_DIR}/include")
set(SOURCE "")

add_subdirectory("${PROJECT_SOURCE_DIR}/lib")
add_library(tools_static STATIC ${SOURCE})
set_target_properties(tools_static PROPERTIES OUTPUT_NAME tools)

add_library(tools_shared SHARED ${SOURCE})
set_target_properties(tools_shared PROPERTIES OUTPUT_NAME tools)

install(TARGETS tools_static DESTINATION lib)
install(TARGETS tools_shared DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

IF(WIN32)
	target_compile_definitions(tools_static PUBLIC -DWINBUILD)
	target_compile_definitions(tools_shared PUBLIC -DWINBUILD)

	include_directories("${CMAKE_CURRENT_LIST_DIR}/../rapidjson/include/")
	target_compile_definitions(tools_static PUBLIC -DWINBUILD)
	target_compile_definitions(tools_shared PUBLIC -DWINBUILD)
ENDIF()


if(${BUILD_EXAMPLES})
	add_executable(i8n examples/i8n/main.cpp)
	target_link_libraries(i8n tools_shared)

	add_executable(system examples/system/main.cpp)
	target_link_libraries(system tools_shared)

	add_executable(json_config_file examples/json_config_file/main.cpp)
	target_link_libraries(json_config_file tools_shared)

	add_executable(file_utils examples/file_utils/main.cpp)
	target_link_libraries(file_utils tools_shared)
	add_custom_command(TARGET file_utils POST_BUILD COMMAND cp -r ../examples/file_utils/*.txt ./)

	add_executable(menu examples/menu/main.cpp)
	target_link_libraries(menu tools_shared)
	add_custom_command(TARGET menu POST_BUILD COMMAND cp -r ../examples/menu/menu.json ./)

	add_executable(chrono examples/chrono/main.cpp)
	target_link_libraries(chrono tools_shared)
endif()
