#include <math.h>
#include <stdio.h>
#include <time.h>

#include "./include/GL/freeglut.h"
#include "./Object.cpp"
#include "Data.cpp"

#include "./glm/glm.hpp"
#include "./glm/gtc/type_ptr.hpp"
#include "./glm/gtx/vec_swizzle.hpp"

#define ORTHOGRAPHIC_PROJECTION 0
#define MOUSE_SENSITIVITY 0.75f
#define FRAMERATE 120
float orthographic_clipping = 200.0f;

float perspective_fov = 70.0f;
float clip_near = 0.01f;
float clip_far = 500.0f;

int backface_culling = 0;
int spin_velocity = 0;

glm::vec3 camera_position;
glm::vec3 camera_rotation;

glm::vec3 grid_colour;

unsigned char mouse_left_down = 0;
unsigned char mouse_middle_down = 0;
unsigned char mouse_right_down = 0;

unsigned char shift_down = 0;
unsigned char ctrl_down = 0;
unsigned char keybind_active = 0;

glm::vec2 mouse_position;
glm::vec2 mouse_delta;
glm::vec2 view_velocity;

glm::mat4 modifier_transform_matrix;

float scroll_increment = 5.0f;

int place_object = 0;

Data* scene_data;


void Init(int* success_state) {
	// set background colour
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	// (maybe) activate backface culling settings
	if (backface_culling) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);

	// initialise camera position matrix
	camera_position = glm::vec3(0.0f, 0.0f, -40.0f); // initial camera position
	camera_rotation = glm::vec3(45.0f, 45.0f, 0.0f); // initial camera angle
	grid_colour = glm::vec3(0.75f, 0.75f, 0.75f); // initial grid colour
	mouse_position = glm::vec2(0.0f, 0.0f);
	mouse_delta = glm::vec2(0.0f, 0.0f);
	view_velocity = glm::vec2(0.0f, 0.0f);

	scene_data = new Data(success_state);

}

void DrawGrid(float range, float spacing, int orientation) {
	// initialise line behaviour
	glLineStipple(1, 0xAAAA);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_LINE_STIPPLE);

	// grid draw sequence
	glColor3f(grid_colour.x, grid_colour.y, grid_colour.z);
	glBegin(GL_LINES);
	for (float i = -range; i <= range; i += spacing) {
		// width lines
		glVertex3f(-range, 0, i);
		glVertex3f(range, 0, i);

		// depth lines
		glVertex3f(i, 0, -range);
		glVertex3f(i, 0, range);
	}
	glDisable(GL_LINE_STIPPLE);
	glEnd();

	glBegin(GL_LINES);
	if (orientation) {
		// X axis
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-range, 0.0f, 0.0f);
		glVertex3f(range, 0.0f, 0.0f);

		// Y axis
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, range, 0.0f);

		// Z axis
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -range);
		glVertex3f(0.0f, 0.0f, range);
	}

	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f); // reset draw colour

	glEnable(GL_DEPTH_TEST);
	return;
}

void StartFaceType(int vertex_count) { // TODO: implement support for composite-face mesh groups
	switch (vertex_count) {
	case(1): glBegin(GL_POINTS); break;
	case(2): glBegin(GL_LINES); break;
	case(3): glBegin(GL_TRIANGLES); break;
	case(4): glBegin(GL_QUADS); break;
	default: glBegin(GL_POLYGON); break;
	}
}

void DrawMeshActors() {
	Object** actor_list = scene_data->GetActors();
	for (int actor = 0; actor < scene_data->stored_actors; actor++) {
		// for every actor in top level scene
		glPushMatrix();
		float* temp_ptr = glm::value_ptr(*(actor_list[actor])->GetTransforms()); // dies here
		
		glMultMatrixf(temp_ptr);

		glm::vec3* object_colour = actor_list[actor]->Colour();
		if (object_colour != nullptr) glColor3f((*object_colour).x, (*object_colour).y, (*object_colour).z);
		else glColor3f(0.5f, 0.5f, 0.5f);

		int object_face_count;
		Face* object_faces = actor_list[actor]->GetFaces(&object_face_count);
		// add all faces into scene
		for (int curr_face_num = 0; curr_face_num < object_face_count;) {
			int vertex_count = object_faces[curr_face_num].vertex_count;
			Face curr_face = object_faces[curr_face_num++];
			
			StartFaceType(vertex_count);
			for (int i = 0; i < vertex_count; i++) {
				glm::vec3 vertex_data = curr_face.vertex_data[i];
				//printf("Draw vertex %.2f, %.2f, %.2f\n", vertex_data.x, vertex_data.y, vertex_data.z);

				glVertex3f(vertex_data.x, vertex_data.y, vertex_data.z);
			}
			glEnd();
		}

		glPopMatrix();
	}
}

void DrawFrame() {
	// frame preparation ~~~~~~~~~~~~~~~~~~~~~~~
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// place fixed background; TODO


	// first level transformations: place modelview, draw environment
	glPushMatrix();

	if (spin_velocity) {
		// retain dampened orbit velocity
		float dampen_factor = 0.97f;
		float dampen_threshold = 0.0000001f;
		if (view_velocity.x != 0.0f || view_velocity.y != 0.0f) {
			camera_rotation.x += view_velocity.x;
			camera_rotation.y += view_velocity.y;
			view_velocity.x = view_velocity.x <= dampen_threshold && view_velocity.x >= -dampen_threshold ? 0.0f : view_velocity.x * dampen_factor;
			view_velocity.y = view_velocity.y <= dampen_threshold && view_velocity.y >= -dampen_threshold ? 0.0f : view_velocity.y * dampen_factor;
		}
	}
	

	if (camera_rotation.y > 90.0f) camera_rotation.y = 90.0f;
	else if (camera_rotation.y < -90.0f) camera_rotation.y = -90.0f;

	glTranslatef(camera_position.x, camera_position.y, camera_position.z); // camera distance
	glRotatef(camera_rotation.y, 1.0f, 0.0f, 0.0f); // yaw
	glRotatef(camera_rotation.x, 0.0f, 1.0f, 0.0f); // pitch

	// environment draw sequence ~~~~~~~~~~~~~~~~~~~~
	DrawGrid(50.0f, 5.0f, 1);

	// first level transformations: place actors
	DrawMeshActors();

	// conclude draw frame
	glPopMatrix();
	glutSwapBuffers();
	//glutPostRedisplay();
}

void Resize(int w, int h) { //TODO: fix window scroll behaviour for orthographic scroll
	h = h == 0 ? 1 : h;
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (ORTHOGRAPHIC_PROJECTION) {
		if (w <= h) glOrtho(-orthographic_clipping, orthographic_clipping, -orthographic_clipping * h / w, orthographic_clipping * h / w, -orthographic_clipping, orthographic_clipping);
		else glOrtho(-orthographic_clipping * w / h, orthographic_clipping * w / h, -orthographic_clipping, orthographic_clipping, -orthographic_clipping, orthographic_clipping);
	}
	else gluPerspective(perspective_fov, (GLfloat)w / (GLfloat)h, clip_near, clip_far);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
}

void MouseEvent(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) { // mouse clicked
		mouse_position.x = (float)x; mouse_position.y = (float)y;
		switch (button) {
			case(GLUT_LEFT_BUTTON):
				if (keybind_active) {
					keybind_active = 0;
				} else mouse_left_down = 1;
				break;
			case(GLUT_MIDDLE_BUTTON):
				mouse_middle_down = 1;
				//printf("MMB down\n");
				break;
			case(GLUT_RIGHT_BUTTON):
				mouse_right_down = 1;
				break;
			case(3):
				if (!ORTHOGRAPHIC_PROJECTION) camera_position.z += scroll_increment;
				else orthographic_clipping += scroll_increment;
				//Resize(GLUT_WINDOW_WIDTH, GLUT_WINDOW_HEIGHT);
				break;
			case(4):
				if (!ORTHOGRAPHIC_PROJECTION) camera_position.z -= scroll_increment;
				else orthographic_clipping -= scroll_increment;
				//Resize(GLUT_WINDOW_WIDTH, GLUT_WINDOW_HEIGHT);
				break;
		}
	} else { // mouse released
		switch (button) {
		case(GLUT_LEFT_BUTTON):
			mouse_left_down = 0;
			break;
		case(GLUT_MIDDLE_BUTTON):
			if (mouse_middle_down && !shift_down) {
				view_velocity.x = mouse_delta.x;
				view_velocity.y = mouse_delta.y;
			}
			mouse_middle_down = 0;
			break;
		case(GLUT_RIGHT_BUTTON):
			mouse_right_down = 0;
			break;
		}
		mouse_position.x = (float)x; mouse_position.y = (float)y;
	}
}

void MouseMovement(int x, int y) {
	// positional difference between previous frame and current frame
	mouse_delta.x = (float)x - (mouse_position.x); mouse_delta.y = ((float)y - (mouse_position.y))/2.0f;
	//printf("Delta: %.3f:%.3f\n", mouse_delta.x, mouse_delta.y);
	// update current position
	mouse_position.x = (float)x; mouse_position.y = (float)y;
	
	if (mouse_middle_down && !shift_down && !ctrl_down && !keybind_active) {
		camera_rotation.x += mouse_delta.x * MOUSE_SENSITIVITY;
		camera_rotation.y += mouse_delta.y * MOUSE_SENSITIVITY;
	} else if (mouse_middle_down && shift_down && !ctrl_down && !keybind_active) {
		camera_position.x += mouse_delta.x / 5.0f;
		camera_position.y += mouse_delta.y/-5.0f;
	} else if (keybind_active) { // TODO: translate object planar to screen
		//modifier_transform_matrix *= 
	}
}

Object* CreateCube() {
	int success_state = 1;
	Object* production_cube = new Object(&success_state);
	
	production_cube->SetAsTrianglePrimitive(10.0f);
	scene_data->AddActor(production_cube, &success_state);
	printf("Cube created.\n");
	return(production_cube);
}

void KeyDown(unsigned char button, int _x, int _y) {
	switch (button) {
		case(' '): shift_down = 1; break;
		case(GLUT_ACTIVE_SHIFT): shift_down = 1; break;
		case('A'): CreateCube(); break;// create cube
		case('g'): keybind_active = 1; break; // move object
	}
}

void KeyUp(unsigned char button, int _x, int _y) {
	switch (button) {
		case(' '): shift_down = 0; break;
		case(GLUT_ACTIVE_SHIFT): shift_down = 0; break;
		case('g'): keybind_active = 0; break;
		}
}

void FramerateLimiter(int i) {
	glutPostRedisplay();
	glutTimerFunc(i, FramerateLimiter, i);
}

int main(int argc, char* argv[]) {
	int success_state = 1;

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glutInitWindowSize(1200, 580);

	glutCreateWindow("Test Scene");

	

	// system event handlers
	glutDisplayFunc(DrawFrame);
	glutReshapeFunc(Resize);

	// input handlers
	glutMouseFunc(MouseEvent);
	glutMotionFunc(MouseMovement);
	glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);

	// animal handlers
	Init(&success_state);

	for (int i = -50.0f; i < 50.0f; i += 10.0f) {
		glm::mat4* transform_matrix = (glm::mat4*)malloc(sizeof(glm::mat4));
		*transform_matrix = glm::mat4(1.0f);
		(*transform_matrix)[3][2] = i;
		Object* temp_cube = CreateCube();
		temp_cube->Transform(transform_matrix);
		temp_cube->SetColour((i + 50.0f) / 100.0f, (i + 50.0f) / 100.0f, (i + 50.0f) / 100.0f);
	}
	
	if (!success_state) { printf("Memory could not be allocated for matrices.\n"); return (69);
	} else printf("Scene setup successfully completed with %d actor(s).\n", scene_data->stored_actors);

	int frame_pause = 1000 / FRAMERATE;
	glutTimerFunc(frame_pause, FramerateLimiter, frame_pause);

	glutMainLoop();

	return 0;
}