#version 330 core

// Ouput data
out vec3 color;
uniform mat4 MVP;
uniform vec3 GREY;

void main()
{

	// Output color = red 
	//color = vec3(1,0,0);
	color = GREY;
}