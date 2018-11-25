#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "cube.hpp"
#include "shader.hpp"
#include "simplex.hpp"

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

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE,GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	if( !glfwOpenWindow( 1024, 768, 0,0,0,0, 32,0, GLFW_WINDOW ) )
	{
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}

	// Initialize GLEW
	glewExperimental = GL_TRUE; 
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	
	glfwSetWindowTitle( "tuto1" );

	// Ensure we can capture the escape key being pressed below
	glfwEnable( GLFW_STICKY_KEYS );

	// Dark blue background
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// Create a VBO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Load shader program
	GLuint programID = LoadShaders( "simple.vert", "simple.frag" );

	// Create a vertex buffer containing cube
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// Get a handle for our "MVP" uniform
	GLuint MVPID = glGetUniformLocation(programID, "MVP");
	GLuint GREYID = glGetUniformLocation(programID, "GREY");

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
								glm::vec3(0,0,25.0), // Camera is at (4,3,-3), in World Space
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
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(programID);


		// first attribute (vertices)
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
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
			glm::vec3 GREY=glm::vec3(color);
			glUniform3f(GREYID, GREY.r, GREY.g, GREY.b);
			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 12*3); // 3 indices starting at 0 -> 1 triangle
		}

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers();
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
		   glfwGetWindowParam( GLFW_OPENED ) );

	// Close OpenGL window and terminate GLFW
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
	
	glfwTerminate();

	return 0;
}

