# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project (Flyaway)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)

set(RAYLIB_DIR D:/CodeFamily/CXXLIB/raylib-5.5_win64_msvc16)

include_directories(${RAYLIB_DIR}/include)

file(GLOB SRC_FILES 
	"${PROJECT_SOURCE_DIR}/src/*.cpp" 
	"${PROJECT_SOURCE_DIR}/src/*.c" 
	"${PROJECT_SOURCE_DIR}/src/*.hpp" 
	"${PROJECT_SOURCE_DIR}/src/*.h" 
    "${PROJECT_SOURCE_DIR}/src/**/*.cpp" # 递归查找子目录
    "${PROJECT_SOURCE_DIR}/src/**/*.c" 
    "${PROJECT_SOURCE_DIR}/src/**/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/**/*.h")

# 打印结果（可选）
message(STATUS "Filtered source files: ${SRC_FILES}")

add_executable(FlyawayGame ${SRC_FILES})

target_link_libraries(FlyawayGame PRIVATE ${RAYLIB_DIR}/lib/raylib.lib winmm)

add_custom_command(TARGET FlyawayGame POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy
	${RAYLIB_DIR}/lib/raylib.dll 
	$<TARGET_FILE_DIR:FlyawayGame>)

# add_custom_command(TARGET FlyawayGame POST_BUILD
# 	COMMAND ${CMAKE_COMMAND} -E copy_directory 
# 	${PROJECT_SOURCE_DIR}/asserts
# 	$<TARGET_FILE_DIR:FlyawayGame>/asserts)
