#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "./glm/glm.hpp"
#include "./glm/gtc/type_ptr.hpp"
#include "./glm/gtx/vec_swizzle.hpp"

struct Face {
	int vertex_count;
	glm::vec3* vertex_data;
};


class Object {
	
private:
	int face_count;
	int faces_allocated;
	Face* faces;

	glm::vec3* object_colour;
	glm::mat4* transforms;

	int num_children;
	int allocated_children;
	Object** children;
	

public:
	Object(int* success_state) {

		

		// initial allocation of 16 faces
		Face* temp_facedata = (Face*)malloc(16 * sizeof(Face));
		if (temp_facedata == nullptr) *success_state = 0;
		else faces = temp_facedata;
		face_count = 0; faces_allocated = 16;
		
		glm::vec3* temp_colour = (glm::vec3*)malloc(sizeof(glm::vec3));
		if (temp_colour == nullptr) *success_state = 0;
		else {
			object_colour = temp_colour;
			*object_colour = glm::vec3(0.75f, 0.75f, 0.75f);
		}

		Object** temp_children = (Object**)malloc(0); // allocate for 0 initial children
		if (temp_children == nullptr) *success_state = 0;
		else {
			children = temp_children;
		}
		num_children = 0; allocated_children = 0;


		/*
		|x1 x2 x3 0 |
		|y1 y2 y3 0 |
		|z1 z2 z3 0 |
		|tx ty tz 1 |
		*/
		glm::mat4* temp_transforms = (glm::mat4*)malloc(sizeof(glm::mat4));
		if (temp_transforms == nullptr) *success_state = 0;
		else {
			transforms = temp_transforms;
			*transforms = glm::mat4(1.0f);
		}
	}

	void AddPolygon(glm::vec3* coordinates, int vertices) {
		if (faces_allocated == face_count) { // if array is 100% filled, must allocate more memory
			Face* temp_ptr = (Face*)realloc(faces, (faces_allocated + 16) * sizeof(Face));
			if (temp_ptr != nullptr) faces = temp_ptr;
			else {
				printf("Extra memory could not be allocated because your computer sucks.\n");
				return;
			}
		}

		// add new face to (expanded) array
		Face* temp = (Face*)malloc(sizeof(Face));
		if (temp == nullptr) printf("System was not able to allocate new memory for face.");
		else {
			temp->vertex_count = vertices;
			temp->vertex_data = coordinates;
			faces[face_count++] = *temp;
		}
	}

	Face* GetFaces(int* num_faces) {
		*num_faces = face_count;
		return(faces);
	}

	void SetColour(float r, float g, float b) {
		//printf("Colour set to %.2f:%.2f:%.2f\n", colour->x, colour->y, colour->z);

		*object_colour = glm::vec3(r, g, b);
	}

	glm::vec3* Colour() {
		//printf("Colour set to %.2f:%.2f:%.2f\n", object_colour->x, object_colour->y, object_colour->z);
		return (object_colour);
	}

	void Transform(glm::mat4* input_matrix) {
		*transforms *= *input_matrix;
	}

	glm::mat4* GetTransforms() {
		glm::mat4* temp = transforms;
		return(transforms);
	}

	// TODO: logical support for children
	// DONE: structural support for children

	void SetAsTrianglePrimitive(float cube_scale) {
		glm::vec3 p1 = glm::vec3(-20.0f, 0.0f, 0.0f);
		glm::vec3 p2 = glm::vec3(20.0f, 0.0f, 0.0f);
		glm::vec3 p3 = glm::vec3(0.0f, 40.0f, 0.0f);

		glm::vec3* points = (glm::vec3*)malloc(3*sizeof(glm::vec3));
		if (points != nullptr) {
			points[0] = p1;
			points[1] = p2;
			points[2] = p3;

			this->AddPolygon(points, 3);
		} else printf("The triangle primitive could not be created.\n");
	}

	void SetAsCubePrimitive(float cube_scale) {

		glm::vec3* points = (glm::vec3*)malloc(4 * sizeof(glm::vec3));
		if (points != nullptr) {
			points[0] = glm::vec3(-cube_scale, -cube_scale, cube_scale);
			points[1] = glm::vec3(cube_scale, -cube_scale, cube_scale);
			points[2] = glm::vec3(cube_scale, cube_scale, cube_scale);
			points[3] = glm::vec3(-cube_scale, cube_scale, cube_scale);
			this->AddPolygon(points, 4);
		}

		points = (glm::vec3*)malloc(4 * sizeof(glm::vec3));
		if (points != nullptr) {
			points[0] = glm::vec3(cube_scale, -cube_scale, cube_scale);
			points[1] = glm::vec3(cube_scale, -cube_scale, -cube_scale);
			points[2] = glm::vec3(cube_scale, cube_scale, -cube_scale);
			points[3] = glm::vec3(cube_scale, cube_scale, cube_scale);
			this->AddPolygon(points, 4);
		}

		points = (glm::vec3*)malloc(4 * sizeof(glm::vec3));
		if (points != nullptr) {
			points[0] = glm::vec3(cube_scale, -cube_scale, -cube_scale);
			points[1] = glm::vec3(-cube_scale, -cube_scale, -cube_scale);
			points[2] = glm::vec3(-cube_scale, cube_scale, -cube_scale);
			points[3] = glm::vec3(cube_scale, cube_scale, -cube_scale);
			this->AddPolygon(points, 4);
		}

		points = (glm::vec3*)malloc(4 * sizeof(glm::vec3));
		if (points != nullptr) {
			points[0] = glm::vec3(-cube_scale, -cube_scale, -cube_scale);
			points[1] = glm::vec3(-cube_scale, -cube_scale, cube_scale);
			points[2] = glm::vec3(-cube_scale, cube_scale, cube_scale);
			points[3] = glm::vec3(-cube_scale, cube_scale, -cube_scale);
			this->AddPolygon(points, 4);
		}

		points = (glm::vec3*)malloc(4 * sizeof(glm::vec3));
		if (points != nullptr) {
			points[0] = glm::vec3(-cube_scale, cube_scale, cube_scale);
			points[1] = glm::vec3(cube_scale, cube_scale, cube_scale);
			points[2] = glm::vec3(cube_scale, cube_scale, -cube_scale);
			points[3] = glm::vec3(-cube_scale, cube_scale, -cube_scale);
			this->AddPolygon(points, 4);
		}

		points = (glm::vec3*)malloc(4 * sizeof(glm::vec3));
		if (points != nullptr) {
			points[0] = glm::vec3(-cube_scale, -cube_scale, -cube_scale);
			points[1] = glm::vec3(cube_scale, -cube_scale, -cube_scale);
			points[2] = glm::vec3(cube_scale, -cube_scale, cube_scale);
			points[3] = glm::vec3(-cube_scale, -cube_scale, cube_scale);
			this->AddPolygon(points, 4);
		}
	}
};
