#pragma once

#include "Object.cpp"

#include "./glm/glm.hpp"
#include "./glm/gtx/vec_swizzle.hpp"



class Data {
private:
	int allocated_objects;
	Object** top_level_actors;

public:
	int stored_actors; // empty descending stack pointer
	Data(int* success_state) {
		Object** temp_objects = (Object**)malloc(4 * sizeof(Object*)); // initialise for 4 actors
		if (temp_objects == nullptr) {
			*success_state = 0;
			printf("Scene data initialisation unsuccessful.\n");
			return;
		}
		top_level_actors = temp_objects;

		allocated_objects = 4;
		stored_actors = 0;

		printf("Scene data initialised.\n");
	}

	void AddActor(Object* actor, int* success_state) { // add created actor to main scene, not parented
		if (allocated_objects == stored_actors) { // hit limit, must reallocate for more
			// exponential allocation of new spaces
			Object** temp = (Object**)realloc(top_level_actors, sizeof(Object*) * allocated_objects * 1.5);
			if (temp == nullptr) {
				*success_state = 0;
				return;
			}
			else top_level_actors = temp;
			printf("Top level actor memory reallocated\n");
		}
		// after possible allocation, store top level actors within procedure stack to prevent multiple pointer calls
		printf("Object added to top level scene %d\n", stored_actors);
		top_level_actors[stored_actors++] = actor;
	}

	Object** GetActors() {
		return(top_level_actors);
	}
};