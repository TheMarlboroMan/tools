cmake_minimum_required(VERSION 3.5.1)

project(tools)

set(CMAKE_BUILD_TYPE Release)

add_compile_options(-Wall -Wextra -Wundef -Wcast-align -Wwrite-strings -Wlogical-op -Wmissing-declarations -Wredundant-decls -Wshadow -Woverloaded-virtual -Wno-deprecated -ansi -pedantic -fno-rtti)

#TODO: Should also allow for c++14
add_compile_options(-std=c++11)

#TODO: Add these:
#optimizations_y="OPTIMIZATION=-O2" optimizations_n="#OPTIMIZATION=-O2"
#debug_y="DEBUG=-g" debug_n="#DEBUG=-g"
#debug_code_y="WITH_DEBUG_CODE=-DWITH_DEBUG_CODE" debug_code_n="#WITH_DEBUG_CODE=-DWITH_DEBUG_CODE"
#disable_assert_y="#DISABLE_ASSERT=-DNDEBUG=1" disable_assert_n="DISABLE_ASSERT=-DNDEBUG=1";

set(SOURCE "")

add_subdirectory("${PROJECT_SOURCE_DIR}/base")
add_subdirectory("${PROJECT_SOURCE_DIR}/class")
add_subdirectory("${PROJECT_SOURCE_DIR}/source")

add_library(tools SHARED ${SOURCE})
install(TARGETS tools DESTINATION /usr/lib)
install(DIRECTORY class/ DESTINATION include/tools/class FILES_MATCHING PATTERN "*.h")
install(DIRECTORY base/ DESTINATION include/tools/base FILES_MATCHING PATTERN "*.h")
install(DIRECTORY templates/ DESTINATION include/tools/templates FILES_MATCHING PATTERN "*.h")
install(DIRECTORY source/ DESTINATION include/tools/source FILES_MATCHING PATTERN "*.h")
