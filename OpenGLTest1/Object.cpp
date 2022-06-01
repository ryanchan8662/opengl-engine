#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "Object.h"
#include "Data.cpp"

#include "./glm/glm.hpp"
#include "./glm/gtx/vec_swizzle.hpp"

class Object {
private:
	int face_count;
	int faces_allocated;
	Face* faces;

	float* object_colour;
	glm::mat4 transforms;

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
		
		

		float* temp = (float*)malloc(3 * sizeof(float)); // allocate and assign space for 0.75 grey colour
		if (temp == nullptr) *success_state = 0;
		else {
			object_colour = temp;
			object_colour[0] = 0.75f; object_colour[1] = 0.75f; object_colour[2] = 0.0f;
		} temp = nullptr;

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
		transforms = glm::mat4(1.0f);
		//transforms = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		
		
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
		Face* temp = (Face*)malloc(sizeof(Face));
		temp->vertex_count = vertices;
		temp->vertex_data = coordinates;
		faces[face_count++] = *temp;
		return;
	}

	Face* GetFaces(int* num_faces) {
		*num_faces = face_count;
		return(faces);
	}

	float* Colour() {
		return (object_colour);
	}

	glm::mat4 Transforms() {
		return(transforms);
	}

	// TODO: logical support for children
	// DONE: structural support for children
};
