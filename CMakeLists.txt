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
add_compile_options(${CMAKE_REQUIRED_FLAGS})

add_executable(landscape
	landscape.cpp
	shader.cpp
	simplex.cpp
	cube.vert
	cube.frag
)

target_link_libraries(landscape
	${OPENGL_LIBRARY} # filled by "find_package(OpenGL REQUIRED)"
	${GLFW_LIBRARY}
	${GLEW_LIBRARY}
)

add_custom_command(
        TARGET landscape POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/cube.vert
                ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/cube.frag
                ${CMAKE_CURRENT_BINARY_DIR}				
)

add_executable(tuto02
	tuto02.cpp
	shader.cpp
	tuto02.vert
	tuto02.frag
)
target_link_libraries(tuto02
	${OPENGL_LIBRARY} # filled by "find_package(OpenGL REQUIRED)"
	${GLFW_LIBRARY}
	${GLEW_LIBRARY}
)

add_custom_command(
        TARGET tuto02 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/tuto02.vert
                ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/tuto02.frag
                ${CMAKE_CURRENT_BINARY_DIR})

add_executable(tuto04
	tuto04.cpp
	shader.cpp
	tuto04.vert
	tuto04.frag
)
target_link_libraries(tuto04
	${OPENGL_LIBRARY} # filled by "find_package(OpenGL REQUIRED)"
	${GLFW_LIBRARY}
	${GLEW_LIBRARY}
)

add_custom_command(
        TARGET tuto04 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/tuto04.vert
                ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/tuto04.frag
                ${CMAKE_CURRENT_BINARY_DIR}
)