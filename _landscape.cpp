#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "cube.hpp"
#include "shader.hpp"
#include "simplex.hpp"
GLFWwindow* window;
int main( void )
{
#ifdef COMPILE_WITH_SOME_PREPROCESSOR_DIRECTIVE
	printf("You checked the SOME_STUFF button in CMake !"); 
#endif

	Simplex simplex;
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
	window = glfwCreateWindow( 1024, 768, "Tutorial 01", NULL, NULL);	
	if( window == NULL )
	{
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Initialize GLEW
	glewExperimental = GL_TRUE; 
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Create a VBO
	/*
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	*/
	// Load shader program

	// Get a handle for our "MVP" uniform
	GLuint programID = LoadShaders( "cube.vert", "cube.frag" );
	GLuint MVPID = glGetUniformLocation(programID, "MVP");
	GLuint GREYID = glGetUniformLocation(programID, "GREY");
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");

	// Create a vertex buffer containing cube
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);


	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
								glm::vec3(0,0,5.0), // Camera is at (4,3,-3), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	std::vector<glm::vec3> positions;
	float size = 1.0f;
	for (float x = -size; x< size; x += 0.1f) {
		for (float y = -size; y< size; y += .1f) {
			positions.push_back(glm::vec3(x,y,0.0));
		}
	}
	do{
		// Draw nothing, see you in tutorial 2 !
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);


		// first attribute (vertices)
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			vertexPosition_modelspaceID,// attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		for (int index = 0; index < positions.size(); index++) {
			// Model matrix : an identity matrix (model will be at the origin)
			glm::vec3 position =  positions[index];
			glm::mat4 TranslationMatrix = glm::translate(glm::mat4(), position);
			glm::mat4 ScaleMatrix = glm::scale(glm::mat4(),glm::vec3(size,size,0.01f));
			glm::mat4 Model      = TranslationMatrix*ScaleMatrix;
			// Our ModelViewProjection : multiplication of our 3 matrices
			glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around
			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);

			float color = 0.5 + (simplex.noise(position.x+size,position.y+size))/2.0;
			color = 0.0;
			glm::vec3 GREY=glm::vec3(color);
			glUniform3f(GREYID, GREY.r, GREY.g, GREY.b);
			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 12*3); // 3 indices starting at 0 -> 1 triangle
		}

		glDisableVertexAttribArray(vertexPosition_modelspaceID);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Close OpenGL window and terminate GLFW
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	//glDeleteVertexArrays(1, &VertexArrayID);
	
	glfwTerminate();

	return 0;
}

