#version 120

// Input vertex data, different for all executions of this shader.
attribute vec3 vertexPosition_modelspace;
attribute vec3 vertexNormal_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 M;
uniform vec3 light_direction;

varying float intensity;

void main(){	

	// Output position of the vertex, in clip space : MVP * position
	vec4 normal_worldspace = M * vec4(normal_worldspace);
	float light_len = len(light_direction);
	float normal_len = len(normal_worldspace);
	float norm = normal_len * light_len;
	if (norm >= 1.0e-6) {		
		intensity = max(0.0, dot(light_direction.xyz,normal_worldspace.xyz) ) / norm ;
	} else {
		intensity = 0.0;
	}
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
}