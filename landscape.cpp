// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "cube.hpp"
#include "simplex.hpp"

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 04 - Colored Cube", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "cube.vert", "cube.frag" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ColorID = glGetUniformLocation(programID, "COLOR");

	// Get a handle for our buffers
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
								glm::vec3(0,0,20), // Camera is at (4,3,-3), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	Simplex simplex;

	const int global_map_size = 1000;
	float global_map[global_map_size][global_map_size];
	float resolution = 10.0;
	for (int x = 0; x < global_map_size; x++) {
		for (int y = 0; y < global_map_size; y++) {
			float xx = (float)x/(float)global_map_size * resolution;
			float yy = (float)y/(float)global_map_size * resolution; 
			float height = (simplex.noise(xx, yy) + 1.0) / 2.0;
			global_map[x][y] = height;
		}
	}
	glm::vec2 position = vec2(global_map_size / 2.0 , global_map_size / 2.0);

	std::vector<glm::vec3> local_map;

	local_map.clear();
	float size = 20.0f;
	float step = 1.0f;
	float tile = step * 0.45;
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	auto update = [&]() {
		local_map.clear();
		for (float map_x = -size; map_x < size; map_x += step) {
			for(float map_y = -size; map_y < size; map_y += step) {
				int x = (int) (map_x + position.x);
				int y = (int) (map_y + position.y);
				if (x >= 0 && x < global_map_size && y >= 0 && y < global_map_size) {
					float height = global_map[x][y];
					local_map.push_back(vec3(map_x,map_y,height));
				}
			}
		}
	};

	update();

	vec2 up = vec2(0.0,1.0);
	vec2 right = vec2(1.0,0.0);
	float speed = 10.0;
	int frame = 0;
	do {
		// glfwGetTime is called only once, the first time this function is called
		static double lastTime = glfwGetTime();
		static double lastFrameTime = glfwGetTime();
		// Compute time difference between current and last frame
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		lastTime = currentTime;
		if (currentTime - lastFrameTime >= 1.0) {
			std::cout << frame << std::endl;
			lastFrameTime = currentTime;
			frame = 0;
		}
		frame++;
		// Move forward
		if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
			position += up * deltaTime * speed;
			update();		
		}
		// Move backward
		if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
			position -= up * deltaTime * speed;
			update();				
		}
		// Strafe right
		if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
			position += right * deltaTime * speed;
			update();		
		}
		// Strafe left
		if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
			position -= right * deltaTime * speed;
			update();		
		}
			// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			vertexPosition_modelspaceID, // The attribute we want to configure
			3,                           // size
			GL_FLOAT,                    // type
			GL_FALSE,                    // normalized?
			0,                           // stride
			(void*)0                     // array buffer offset
		);
		for (auto &item:local_map) {
			glm::mat4 translation = glm::translate(glm::mat4(1), vec3(item.x,item.y,0.0));
			glm::mat4 scale = glm::scale(glm::mat4(1),glm::vec3(tile,tile,tile/10.0f));
			//glm::mat4 scale = glm::mat4(tile);
			glm::mat4 Model      = translation*scale;
			//glm::mat4 Model      = 	glm::mat4(1.0);
			// Our ModelViewProjection : multiplication of our 3 matrices
			
			glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glUniform4f(ColorID, item.z,item.z,item.z,1.0);
			// Draw the triangleS !
			glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
		}


		glDisableVertexAttribArray(vertexPosition_modelspaceID);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

