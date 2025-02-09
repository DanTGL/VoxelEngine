cmake_minimum_required (VERSION 3.12.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

IF (WIN32)
	include(automate-vcpkg.cmake)

	vcpkg_bootstrap()
	vcpkg_install_packages(glfw3 libnoise glm glew)

ENDIF()

project (VoxelEngine)

file (GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" )

add_executable(VoxelEngine ${SOURCES})

add_custom_command(TARGET VoxelEngine PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/assets/ $<TARGET_FILE_DIR:VoxelEngine>/assets/)


target_include_directories(VoxelEngine
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include"                          
)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
#find_package(unofficial-noise CONFIG REQUIRED)
#find_package(unofficial-noiseutils CONFIG REQUIRED)

target_link_libraries(VoxelEngine PRIVATE ${OPENGL_LIBRARIES} GLEW::glew glfw glm::glm noise)

