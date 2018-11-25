cmake_minimum_required (VERSION 2.6)
project (voxpopuli)
include(FindPkgConfig)


find_package(OpenGL REQUIRED)

set(GLFW_INCLUDE_PATH "/usr/include/GLFW" CACHE PATH "/usr/include/GLFW" )
set(GLFW_LIBRARY "glfw" CACHE FILEPATH "/usr/lib/x86_64-linux-gnu" )
set(GLEW_INCLUDE_PATH "/usr/include/" CACHE PATH "The directory that contains GL/glew.h" )
set(GLEW_LIBRARY "GLEW" CACHE FILEPATH "/usr/lib/x86_64-linux-gnu" )
set(GLM_INCLUDE_PATH "usr/include/glm" CACHE PATH "usr/include/glm" )

set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

include_directories(
	${GLFW_INCLUDE_PATH}
	${GLEW_INCLUDE_PATH}
	${GLM_INCLUDE_PATH}
)

add_executable(tuto1
	tuto1.cpp
	shader.cpp
	simplex.cpp
	simple.vert
	simple.frag
)

add_compile_options(${CMAKE_REQUIRED_FLAGS})

target_link_libraries(tuto1
	${OPENGL_LIBRARY} # filled by "find_package(OpenGL REQUIRED)"
	${GLFW_LIBRARY}
	${GLEW_LIBRARY}
)
add_custom_command(
        TARGET tuto1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/simple.vert
                ${CMAKE_CURRENT_BINARY_DIR}/simple.vert
)
add_custom_command(
        TARGET tuto1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/simple.frag
                ${CMAKE_CURRENT_BINARY_DIR}/simple.frag
)