#include "cube.hpp"
#include <string.h>
#include <glm/glm.hpp>

Cube::Cube() {
    memcpy(vertice, g_vertex_buffer_data, sizeof(vertice));
    build_normals();
}

void Cube::build_normals() {
	for (int triangle_index = 0; triangle_index < 12; triangle_index++) {
		int index = triangle_index * 3 * 3;
		glm::vec3 v1(vertice[index], vertice[index+1], vertice[index+2]);
		glm::vec3 v2(vertice[index+3], vertice[index+4], vertice[index+5]);
		glm::vec3 v3(vertice[index+6], vertice[index+7], vertice[index+8]);
		glm::vec3 vv1 = v2 - v1;
		glm::vec3 vv2 = v3 - v2;
		glm::vec3 normal = glm::normalize(glm::cross(vv1, vv2));
		for (int vertex_index = 0; vertex_index < 3; vertex_index++) {
			index = triangle_index * 9 +  vertex_index * 3;
            normals[index] = normal.x;
			normals[index+1] = normal.y;
			normals[index+2] = normal.z;
		}
	}   
}

void Cube::register_buffers() {
	glGenBuffers(1, &vertice_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertice_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);

	glGenBuffers(1, &normals_id);
	glBindBuffer(GL_ARRAY_BUFFER, normals_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
}

void Cube::unregister_buffers() {
    glDeleteBuffers(1, &vertice_id);
    glDeleteBuffers(1, &normals_id);
}