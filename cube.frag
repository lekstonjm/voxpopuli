#version 120

uniform vec4 light_color;
varying float intensity;

void main(){

	// Output color = color specified in the vertex shader, 
	// interpolated between all 3 surrounding vertices
	gl_FragColor = light_color * intensity;

}