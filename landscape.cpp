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
#include "heightmap_generator.hpp"

#include "noise/noise.h"
#include "noise/noiseutils.h"
using namespace noise;

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
	GLuint MVPID = glGetUniformLocation(programID, "mvp");
	GLuint LightColorID = glGetUniformLocation(programID, "light_color");
	GLuint LightDirectionID = glGetUniformLocation(programID, "light_direction");
	GLuint ModelID = glGetUniformLocation(programID, "model");

	// Get a handle for our buffers
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");
	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
								glm::vec3(0,2,0), // Camera is at (4,3,-3), in World Space
								glm::vec3(0,0,-20), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	Cube cube;
	/*
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube.vertice), cube.vertice, GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube.normals), cube.normals, GL_STATIC_DRAW);
	*/
	cube.register_buffers();
/*
	noise::module::Simplex simplex;
	module::RidgedMulti mountainTerrain;

	module::Billow baseFlatTerrain;
	baseFlatTerrain.SetFrequency (2.0);

	module::ScaleBias flatTerrain;
	flatTerrain.SetSourceModule (0, baseFlatTerrain);
	flatTerrain.SetScale (0.125);
	flatTerrain.SetBias (-0.75);

	module::Simplex terrainType;
	terrainType.SetFrequency (0.5);
	terrainType.SetPersistence (0.25);
	terrainType.SetOctaves(5);
	terrainType.SetFractal(true);

	module::Select finalTerrain;
	finalTerrain.SetSourceModule (0, flatTerrain);
	finalTerrain.SetSourceModule (1, mountainTerrain);
	finalTerrain.SetControlModule (terrainType);
	finalTerrain.SetBounds (0.0, 1000.0);
	finalTerrain.SetEdgeFalloff (0.125);

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule (finalTerrain);
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (512, 512);
	heightMapBuilder.SetBounds (0.0, 5.0, 0.0, 5.0);
	heightMapBuilder.Build ();
*/

	HeightmapGenerator heightmap_generator;
	heightmap_generator.set_bounds(512, 0.0, 5.0);

	const int global_map_size = 512;
	float global_map[global_map_size][global_map_size];
	float resolution = 30.0;
	float hight = 20.0;
	for (int x = 0; x < global_map_size; x++) {
		for (int z = 0; z < global_map_size; z++) {
			float xx = (float)x/(float)global_map_size * resolution;
			float zz = (float)z/(float)global_map_size * resolution; 
			//float height = (simplex.noise(xx, zz) + 1.0) / 2.0 * hight;
			//float height = heightMap.GetValue(x,z);
			float height = heightmap_generator.compute_height(x,z);
			height = ( height + 1.0 ) / 2.0 * hight;
			global_map[x][z] = height;
		}
	}
	glm::vec2 position = vec2(global_map_size / 2.0 , global_map_size / 2.0);


	int size = 20;
	float tile = 1.0f;
	std::vector<glm::vec3> local_map(size*size);
	// Send our transformation to the currentlz bound shader, 
	// in the "MVP" uniform
	auto update = [&]() {
		local_map.clear();
		for(int map_z = 0; map_z < 2.0 * size; map_z++) {
			for (int map_x = -map_z - 1 ; map_x < map_z + 1; map_x++) {
				int x = map_x + (int)position.x;
				int z = -map_z + (int)position.y;
				if (x >= 0 && x < global_map_size && z >= 0 && z < global_map_size) {
					float height = global_map[x][z];
					local_map.push_back(vec3((float)map_x*tile,height,-(float)map_z*tile));
				}
			}
		}
		int x = (int) (position.x);
		int z = (int) (position.y);
		if (x >= 0 && x < global_map_size && z >= 0 && z < global_map_size) {
			float height = global_map[x][z];
			View = glm::lookAt(
				glm::vec3(0,height+1.0,0), // Camera is at (4,3,-3), in World Space
				glm::vec3(0,height+.8,-1), // and looks at the origin
				glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
		}

	};

	update();

	glm::vec2 up = glm::vec2(0.0,1.0);
	glm::vec2 right = glm::vec2(1.0,0.0);
	float speed = 10.0;
	int frame = 0;
	glm::vec3 light_direction = glm::vec3(0.0, -1.0, 0.0);
	glm::vec4 ligh_color = glm::vec4(1.0, 1.0, .9, 1.0);
	glm::vec4 sky_color = glm::vec4(0.4,0.4,1.0,1.0);

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
			position -= up * deltaTime * speed;
			update();		
		}
		// Move backward
		if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
			position += up * deltaTime * speed;
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
		//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, cube.vertice_id);

		glVertexAttribPointer(
			vertexPosition_modelspaceID, // The attribute we want to configure
			3,                           // size
			GL_FLOAT,                    // type
			GL_FALSE,                    // normalized?
			0,                           // stride
			(void*)0                     // array buffer offset
		);
		glEnableVertexAttribArray(vertexNormal_modelspaceID);
		//glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, cube.normals_id);
		glVertexAttribPointer(
			vertexNormal_modelspaceID, // The attribute we want to configure
			3,                           // size
			GL_FLOAT,                    // type
			GL_FALSE,                    // normalized?
			0,                           // stride
			(void*)0                     // array buffer offset
		);
		for (auto &item:local_map) {
			//glm::mat4 scale = glm::scale(glm::mat4(1),glm::vec3(tile*0.20,item.y,tile*0.2));
			glm::mat4 scale = glm::scale(glm::mat4(1),glm::vec3(tile*0.5,item.y,tile*0.5));
			//glm::mat4 translation = glm::translate(glm::mat4(1), vec3(item.x,0.0,item.z));
			glm::mat4 translation = glm::translate(glm::mat4(1), vec3(item.x,0.0,item.z));
			//glm::mat4 scale = glm::mat4(tile);
			glm::mat4 Model      = translation*scale;
			//glm::mat4 Model      = 	glm::mat4(1.0);
			// Our ModelViewProjection : multiplication of our 3 matrices
			
			glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

			glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);
			glUniform4f(LightColorID, ligh_color.r, ligh_color.g, ligh_color.b, ligh_color.a);
			glUniform3f(LightDirectionID, light_direction.x, light_direction.y, light_direction.z);
			// Draw the triangleS !
			glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
		}


		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexNormal_modelspaceID);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	//glDeleteBuffers(1, &vertexbuffer);
	cube.unregister_buffers();
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

