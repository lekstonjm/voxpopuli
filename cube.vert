#version 120

// Input vertex data, different for all executions of this shader.
attribute vec3 vertexPosition_modelspace;
attribute vec3 vertexNormal_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 mvp;
uniform mat4 model;
uniform vec3 light_direction;

varying float lambert;

void main(){	

	// Output position of the vertex, in clip space : MVP * position
	vec4 vertexNormal_worldspace = model * vec4(vertexNormal_modelspace,1.0);
	lambert =  dot(-light_direction, vertexNormal_worldspace.xyz);
	lambert = clamp(lambert,0.3, 0.8);
	gl_Position =  mvp * vec4(vertexPosition_modelspace,1);
}