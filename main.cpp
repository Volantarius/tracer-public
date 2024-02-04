#include "general.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>

#include <iostream>
#include <cstdio>
//#include <time.h>
#include <assert.h>

//#include "memory.h"

#include "vector.h"
#include "matrix.h"
#include "level_manager.h"
#include "timesplitters.h"
#include "debug_overlay.h"
//#include "collision.h"
#include "v_input.h"

#include "shaders_temp.h"// TODO: Actually setup a shader thing

unsigned int SCREEN_WIDTH = 1280;//1600;
unsigned int SCREEN_HEIGHT = 720;//900;

struct entity_point {
	v3 position;
	v3 rotation;
};

struct entity_butt {
	v3 euler;
	v3 position;
	v3 velocity;
	v3 impulse;
};

GLint posSimpAttrib;
GLint colSimpAttrib;
GLuint Matrix_Simp_Proj;
GLuint Matrix_Simp_World;
GLuint Matrix_Simp_View;
GLuint simp_vbo;

GLint posAttrib;
GLint colAttrib;
GLint norAttrib;
GLint texAttrib;
GLuint vbo;
GLuint ebo;
GLuint tex;

GLuint color, depth, fbo;

GLuint Matrix_Proj;
GLuint Matrix_World;
GLuint Matrix_View;

uint32_t WindowFlags = SDL_WINDOW_OPENGL;
SDL_Window * Window = SDL_CreateWindow("Perfect Shot", 64, 64, SCREEN_WIDTH, SCREEN_HEIGHT, WindowFlags);

SDL_GLContext Context;
GLenum glewError;

bool running = true;
bool lock_mouse = false;

mat4 world = mat4();
mat4 view = mat4();
mat4 projection = mat4();

Uint32 time_now = 0;
Uint32 time_last = 0;
Uint32 time_diff = 0;
float time_delta = 0;

float count_time = 0.0f;
unsigned int count_frames = 0;

// AHAHSDHASD
unsigned int room_count = 13;
unsigned int material_count = 0;
unsigned int temp_in_room_index = 0;
// ASIODHAOUIHSD

entity_point main_camera;
entity_butt player;
mat4 player_matrix_test;// TODO: REMOVE
float butt_zoom = 1.0f;

int main(int argc, char* argv[]) {
	GLint status;

	assert(Window);

	if (Window == NULL) {
		printf("SDL Error %s\n", SDL_GetError());
		return 1;
	}
	
	Context = SDL_GL_CreateContext(Window);

#ifdef V_GLVSYNC
	SDL_GL_SetSwapInterval(1);
#else
	SDL_GL_SetSwapInterval(0);
#endif

	// Need this for GLEW
	glewExperimental = GL_TRUE;
	glewError = glewInit();

	// Reversed Z Depth
	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_2D, color);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_SRGB8_ALPHA8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &depth);
	glBindTexture(GL_TEXTURE_2D, depth);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Reversed Z Depth
	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &shader_vert_src, NULL);
	glCompileShader(vertShader);

	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);

	// Need to print the errors
	assert(status == GL_TRUE);

	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &shader_frag_src, NULL);
	glCompileShader(fragShader);

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vertShader);
	glAttachShader(shader_program, fragShader);

	glLinkProgram(shader_program);

	posAttrib = glGetAttribLocation(shader_program, "position");
	colAttrib = glGetAttribLocation(shader_program, "color");
	norAttrib = glGetAttribLocation(shader_program, "normal");
	texAttrib = glGetAttribLocation(shader_program, "texcoord");

	glUseProgram(shader_program);// Probably what switchs shaders during rendering

	// ================== SIMPLE SHADER
	GLuint vert_simple_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_simple_shader, 1, &shader_simple_vert_src, NULL);
	glCompileShader(vert_simple_shader);
	glGetShaderiv(vert_simple_shader, GL_COMPILE_STATUS, &status);
	assert(status == GL_TRUE);
	GLuint frag_simple_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_simple_shader, 1, &shader_simple_frag_src, NULL);
	glCompileShader(frag_simple_shader);
	glGetShaderiv(frag_simple_shader, GL_COMPILE_STATUS, &status);
	assert(status == GL_TRUE);
	GLuint shader_simple_program = glCreateProgram();
	glAttachShader(shader_simple_program, vert_simple_shader);
	glAttachShader(shader_simple_program, frag_simple_shader);
	glLinkProgram(shader_simple_program);
	glUseProgram(shader_simple_program);
	
	posSimpAttrib = glGetAttribLocation(shader_simple_program, "position");
	colSimpAttrib = glGetAttribLocation(shader_simple_program, "color");
	
	Matrix_Simp_Proj = glGetUniformLocation(shader_simple_program, "u_proj");
	Matrix_Simp_View = glGetUniformLocation(shader_simple_program, "u_view");
	Matrix_Simp_World = glGetUniformLocation(shader_simple_program, "u_model");
	
	glGenBuffers(1, &simp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, simp_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 1024, NULL, GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(posSimpAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glVertexAttribPointer(colSimpAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(posSimpAttrib);
	glEnableVertexAttribArray(colSimpAttrib);
	// =================== END SIMPLE SHADER

	// Need to print the errors
	assert(status == GL_TRUE);
	
	glUseProgram(shader_program);
	
	Matrix_Proj = glGetUniformLocation(shader_program, "u_proj");
	Matrix_View = glGetUniformLocation(shader_program, "u_view");
	Matrix_World = glGetUniformLocation(shader_program, "u_model");
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW); // Without indexing
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * V_MAX_VERTICES, NULL, GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), 0);
	glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(7 * sizeof(float)));
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(10 * sizeof(float)));

	glEnableVertexAttribArray(posAttrib);
	glEnableVertexAttribArray(colAttrib);
	glEnableVertexAttribArray(norAttrib);
	glEnableVertexAttribArray(texAttrib);
	
	/*
	# 2 village
	# 5 chemical plant
	# 8 haunted mansion
	# 9 planet x
	# 10 compound
	# 11 streets
	# 12 warzone
	# 13 construction site
	# 14 cyberden
	# 15 bank
	# 16 graveyard
	# 17 spaceship
	# 18 mall
	# 21 chinese
	# 22 castle
	# 23 egypt
	# 24 docks
	# 25 unused or test map (VR themed)
	# 26 egypt MP
	# 27 spaceways

	***** TODO: Add the indice of the areaportals to the rooms
	***** FIX: VSYNC lol
	***** TODO: Seperate Slice Triangle function to somewhere else
	*****       Need to use it for other shiz! Like cutting props in half and stuff lol
	*/
	
	// Try and load up a level!
	ts_load_level("tslevels\\level2.raw");
	
	v3 w_up = v3(0.0f, 1.0f, 0.0f);
	v3 world_origin = v3(0.0f, 0.0f, 0.0f);

	projection.make_infinte_perspective(90.0f, 1.778f, 0.1f);

	main_camera.position = v3(0.0f, 1.0f, 0.0f);
	main_camera.rotation = v3(1.0f, 0.0f, 0.0f);
	
	player_matrix_test = mat4();
	player.euler = v3(0.0f, 0.0f, 0.0f);
	
	//player_matrix_test.from_pitchyaw(player.euler.x, player.euler.y);
	player_matrix_test.look_at(main_camera.rotation, w_up);

	//v_input_init();// Setup input
	//vcol::init();
	debug::init();
	debug::debug_line* debug_lines = debug::get_lines();
	unsigned int debug_line_count = 0;
	auto debug_size_test = sizeof(float) * 6;
	
	// UPDATE WHEN UPDATES LOL
	get_rooms_count(room_count);
	get_materials_count(material_count);
	auto rooms = get_rooms();
	auto buffer_size_test = sizeof(float) * 12;
	
	auto materials = get_level_materials();
	
	for (unsigned int i = 0; i < material_count; i++) {
		//printf("Test: %s\n", materials[i].name);
		
		glGenTextures(1, &materials[i].gl_texture);
		
		glBindTexture(GL_TEXTURE_2D, materials[i].gl_texture);
		
		int width, height;
		unsigned char* data = ts_load_texture(&width, &height, (const char *)materials[i].name);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const unsigned char *)data);
		
		glGenerateMipmap(GL_TEXTURE_2D);
		
		/*
		GL_CLAMP_TO_EDGE
		GL_MIRRORED_REPEAT
		GL_REPEAT
		*/
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		if ((materials[i].flags_a & 1) == 1) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		};
		
		if ((materials[i].flags_b & 1) == 1) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		};
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		//glBindTexture(GL_TEXTURE_2D, 0);//?? Maybe
		
		free(data);
	};
	
	float move_speed = 5.0f;
	v3 view_forward = v3(1.0f, 0.0f, 0.0f);
	v3 view_right = v3(0.0f, 1.0f, 0.0f);
	v3 move_forward = v3(1.0f, 0.0f, 0.0f);
	v3 move_right = v3(0.0f, 1.0f, 0.0f);
	
	while (running) {
		SDL_Event Event;

		while (SDL_PollEvent(&Event)) {
			if (Event.type == SDL_QUIT) {
				running = false;
				break;
			}

			v_input_event(Event);

			if (Event.type == SDL_MOUSEBUTTONDOWN) {
				if (Event.button.button == SDL_BUTTON_RIGHT) {
					lock_mouse = !lock_mouse;

					SDL_SetRelativeMouseMode((SDL_bool)lock_mouse);
				};
			};

			if (lock_mouse && Event.type == SDL_MOUSEMOTION) {
				player.euler.y += (float)Event.motion.xrel * 0.002f;
				player.euler.x += (float)Event.motion.yrel * 0.002f;
			};
		}
		
		time_now = SDL_GetTicks();
		time_diff = time_now - time_last;
		time_delta = time_diff / 1000.0f;
		
		count_time += time_delta;
		
		// TODO: If vsync is off we still need to limit SDL with SDL_Delay(1) or whatever
		// Also we aren't going to tick method, we can buffer the thread or something
		// We don't need to call SDL event 600 times a second

		if (g_keys_down & IN_LOOK_LEFT)  { player.euler.y -= time_delta * 2.0f; };
		if (g_keys_down & IN_LOOK_RIGHT) { player.euler.y += time_delta * 2.0f; };
		if (g_keys_down & IN_LOOK_UP)    { player.euler.x -= time_delta * 2.0f; };
		if (g_keys_down & IN_LOOK_DOWN)  { player.euler.x += time_delta * 2.0f; };
		
		// TODO: Find a better way of doing rotations... we don't really need a 90 whatever, it could be 1, and 2. Then multiply by 90 for degrees
		if (player.euler.x > 1.56f) { player.euler.x = 1.56f; };
		if (player.euler.x < -1.56f) { player.euler.x = -1.56f; };
		
		// Alright for rotations we are going to use a target vector position
		// This way we can do third person and stuff by normalizing difference vectors
		// Which is super sexy and makes stuff really nice!
		
		//v3 player_euler_radians = v3();
		//v3_to_radians(player.euler, player_euler_radians);
		//player_euler_radians.x = PI * 0.5f;// No the view matrix is fudged, gimbal lock I think.. EITHER deulerize is wrong or the matrix setup is fudged too
		
		v3_deulerize(player.euler, main_camera.rotation);
		main_camera.rotation.normalize();

		view_forward = main_camera.rotation;
		view_right = v3();
		
		v3_cross(view_forward, w_up, view_right);
		view_right.normalize();
		
		move_forward = view_forward;
		move_right = view_right;
		
		// TODO: This is a test!!!
		// Otherwise use the translate below!
		player_matrix_test.look_at(view_forward, w_up);
		view = player_matrix_test;

		move_forward.y = 0.0f;
		move_forward.normalize();
		move_right.y = 0.0f;
		move_right.normalize();
		
		move_speed = 10.0f;
		
		player.impulse *= 0.0f;

		if (g_keys_down & IN_SPRINT) { move_speed = 20.0f; };
		if (g_keys_down & IN_SLOW)   { move_speed = 1.0f; };

		if (g_keys_down & IN_FORWARD)      { player.impulse -= move_forward; };
		if (g_keys_down & IN_BACKWARD)     { player.impulse += move_forward; };
		if (g_keys_down & IN_STRAFE_RIGHT) { player.impulse -= move_right; };
		if (g_keys_down & IN_STRAFE_LEFT)  { player.impulse += move_right; };
		
		player.impulse.normalize();
		
		if (g_keys_down & IN_JUMP)         { player.position += w_up * (time_delta * 2.0f); };
		if (g_keys_down & IN_CROUCH)       { player.position -= w_up * (time_delta * 2.0f); };
		
		if (g_keys_pressed & IN_WHEEL_UP) {
			butt_zoom -= 0.25f;
		};

		if (g_keys_pressed & IN_WHEEL_DOWN) {
			butt_zoom += 0.25f;
		};
		
		player.impulse *= (time_delta * move_speed);
		
		//vcol::check_collision(&player.position, &player.impulse, time_delta);
		
		player.position += player.impulse;// REMOVE LATER
		
		//v3 debug_impulse = player.position + player.velocity;
		//v3 debug_color = v3(0.0f, 0.0f, 1.0f);
		
		//debug::add_line(player.position, debug_impulse, v3(0.0f, 1.0f, 1.0f));
		
		v3 test_thirdperson = player.position + (view_forward * butt_zoom);// Add scroll wheel zooming lol
		//v3 test_thirdperson = player.position + (w_up * butt_zoom);
		
		// Yeah seperate the view, projection, and world
		// This helps keep precision really high, especially with you know... large levels
		world.translate(test_thirdperson);
		
		if (count_time >= 1) {
			//printf("FRAMES: %d, delta: %f\n", count_frames, time_delta);
			// Need a debug overlay!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			count_time = 0;
			count_frames = 0;

			//printf("position: %f %f %f\n", main_camera.position.x, main_camera.position.y, main_camera.position.z);
			// Z is Y
		};
		
		v3 ppos_mod = player.position;
		
		v3 debug_x1_dir = ppos_mod + v3(0.5f, 0.0f, 0.0f);
		v3 debug_y1_dir = ppos_mod + v3(0.0f, 0.0f, 0.5f);
		v3 debug_x2_dir = ppos_mod - v3(0.5f, 0.0f, 0.0f);
		v3 debug_y2_dir = ppos_mod - v3(0.0f, 0.0f, 0.5f);
		
		v3 shit_color = v3(0.0f, 1.0f, 1.0f);
		
		debug::add_line(debug_x1_dir, debug_x2_dir, shit_color);
		debug::add_line(debug_y1_dir, debug_y2_dir, shit_color);
		
		// TODO::
		// Area Portal Thinking... And only get adjacent room's boundries from AP's
		// Then only change the room the player is actually in by Area Portal transitions
		/*bool use_old_room = false;
		
		auto *own_room = &rooms[temp_in_room_index];
		
		if (main_camera.position.x < own_room->bounds_max.x && main_camera.position.y < own_room->bounds_max.y && main_camera.position.z < own_room->bounds_max.z &&
			main_camera.position.x > own_room->bounds_min.x && main_camera.position.y > own_room->bounds_min.y && main_camera.position.z > own_room->bounds_min.z) {
			
			use_old_room = true;
		};
		
		if (!use_old_room) {
			for (unsigned int i = 0; i < rooms[temp_in_room_index].adjacent_rooms_count; i++) {
				unsigned int room_index = rooms[temp_in_room_index].adjacent_rooms[i];
				auto *r = &rooms[room_index];
				
				if (main_camera.position.x < r->bounds_max.x && main_camera.position.y < r->bounds_max.y && main_camera.position.z < r->bounds_max.z &&
					main_camera.position.x > r->bounds_min.x && main_camera.position.y > r->bounds_min.y && main_camera.position.z > r->bounds_min.z) {
					
					temp_in_room_index = room_index;
					printf("In room: %d\n", temp_in_room_index);
				};
			};
		};*/
		
		glUseProgram(shader_program);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// (2022) WAIT WHAT
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), 0);
		glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(7 * sizeof(float)));
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(10 * sizeof(float)));
		
		glUniformMatrix4fv(Matrix_World, 1, GL_FALSE, (GLfloat *)world);
		glUniformMatrix4fv(Matrix_View, 1, GL_FALSE, (GLfloat *)view);
		glUniformMatrix4fv(Matrix_Proj, 1, GL_FALSE, (GLfloat *)projection);
		
		// Depth is disabled to allow other drawing functions to work
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_GREATER);

		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		//glPolygonMode(GL_FRONT, GL_LINE);
		glCullFace(GL_FRONT);

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
		//glClearColor(0.3569f, 0.7059f, 0.9765f, 0.0f);
		//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		// RZ
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearDepth(0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		unsigned int vertice_count = 0;
		unsigned int surface_count = 0;

		// DRAW A MESH BABY!
		// Later: Actually use area portals to sort the rooms and render them in order.
		for (unsigned int room_index = 0; room_index < room_count; room_index++) {
			vertice_count = 0;
			surface_count = 0;

			auto current_room = &rooms[room_index];

			if (current_room->elements_size < 1 || current_room->elements_size > V_MAX_VERTICES) {
#ifdef V_DEBUG
				printf("BROKE! room: %d, verts: %d\n", room_index, current_room->elements_size);
				assert(false);
#endif
				continue;
			};
			
			glBufferSubData(GL_ARRAY_BUFFER, NULL, buffer_size_test * current_room->elements_size, current_room->elements);
			
			// TEST OF DRAWING A MESH
			for (unsigned int group_index = 0; group_index < current_room->groups_count; group_index++) {
				group_info *current_group = &current_room->groups[group_index];
				
				glBindTexture(GL_TEXTURE_2D, materials[current_group->material_id].gl_texture);
				
				for (unsigned int surface_index = 0; surface_index < current_group->strips_count; surface_index++) {
					surface_info *current_surface = &current_room->surfaces[surface_count + surface_index];

					glCullFace(GL_FRONT);
					
					if ((current_surface->flags_b & 1) == 1) {
						glCullFace(GL_BACK);
					};

					glDrawArrays(GL_TRIANGLE_STRIP, vertice_count, current_surface->vert_count);

					vertice_count += current_surface->vert_count;
				};

				surface_count += current_group->strips_count;
			};
		};

		// DRAWING OF DEBUG
		glUseProgram(shader_simple_program);
		glBindBuffer(GL_ARRAY_BUFFER, simp_vbo);
		// (2022) WAIT WHAT, Okay so we have to do this for every buffer we bind to... which maybe creating loads of vbos is bad
		glVertexAttribPointer(posSimpAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
		glVertexAttribPointer(colSimpAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		
		glUniformMatrix4fv(Matrix_Simp_World, 1, GL_FALSE, (GLfloat *)world);// Hmmm seems cruddy
		glUniformMatrix4fv(Matrix_Simp_View, 1, GL_FALSE, (GLfloat *)view);
		glUniformMatrix4fv(Matrix_Simp_Proj, 1, GL_FALSE, (GLfloat *)projection);
		
		glDisable(GL_DEPTH_TEST);
		
		glBufferSubData(GL_ARRAY_BUFFER, NULL, debug_size_test * 1024, debug_lines);
		
		debug::get_line_count(debug_line_count);
		
		for (unsigned int i = 0; i < debug_line_count; i+=2) {
			glDrawArrays(GL_LINES, i, 2);
		};
		
		glEnable(GL_DEPTH_TEST);
		
		// RZ
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(
			0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
			0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
			GL_COLOR_BUFFER_BIT, GL_LINEAR);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDepthFunc(GL_LESS);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		count_frames++;
		
#ifndef V_GLVSYNC
		SDL_Delay(1);
#endif
		time_last = time_now;

		v_input_clearkeys();
		debug::reset();

		SDL_GL_SwapWindow(Window);// Blocks thread with vsync
	};

	glDeleteTextures(1, &tex);
	glDeleteTextures(1, &color);// MaYBE??
	glDeleteTextures(1, &depth);

	for (unsigned int i = 0; i < material_count; i++) {
		glDeleteTextures(1, &materials[i].gl_texture);
	};

	cleanup_level();
	debug::shutdown();

	glDisableVertexAttribArray(posAttrib);
	glDisableVertexAttribArray(colAttrib);
	glDisableVertexAttribArray(texAttrib);

	glUseProgram(NULL);

	SDL_GL_DeleteContext(Context);
	SDL_DestroyWindow(Window);
	SDL_Quit();

	//vmem::shutdown();
	
	return 0;
}
