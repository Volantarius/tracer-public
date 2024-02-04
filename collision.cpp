#include "general.h"
#include "vector.h"
#include "collision.h"

#include "level_manager.h"

#include "debug_overlay.h"

//#include <cstdio>

/*
	IDEAS
	
	Maybe try to translate the level geometry with a matrix and keep things really precise
*/

namespace vcol {

v3 clip_x_axis;
v3 clip_y_axis;
v3 clip_z_axis;

unsigned int in_room;
room_info *current_room;
room_info *rooms;

struct collision_info {
	unsigned int strip_index;
};

collision_info collision_faces[2048];
unsigned int collision_face_count;

// TODO: Use this for testing corners later
struct collision_slice {
	v3 slice_a;
	v3 slice_b;
	v3 normal;
	bool collide;
};

void init() {
	// Used to slice the geometry for our collision
	// Prolly needs to be in the header instead lol
	clip_x_axis = v3(1.0f, 0.0f, 0.0f);
	clip_y_axis = v3(0.0f, 1.0f, 0.0f);
	clip_z_axis = v3(0.0f, 0.0f, 1.0f);
	
	collision_face_count = 0;
	
	// what about rotated collision????
	// The axis's can be changed but maybe we can just translate the whole thing like ive thought about before??
	
	// Completely temporary right now
	in_room = 5;// 5, 28, 26 for Level 9
	
	rooms = get_rooms();
};

// out the vector via parameter
// Not triangle order dependant, other than winding order
inline void slice_triangle(v3 &a, v3 &b, v3 &c, float &d1, float &d2, float &d3, v3 &slice_a, v3 &slice_b) {
	//slice_a = a + (d1 / (d1 - d2)) * (b - a);
	slice_a = b - a;
	v3_multiply(slice_a, d1 / (d1 - d2), slice_a);
	v3_add(slice_a, a, slice_a);
	
	if (d1 < 0.0f) {
		// then b is above
		
		if (d3 < 0.0f) {
			//slice_b = b + (d2 / (d2 - d3)) * (c - b);// BC
			slice_b = c - b;
			v3_multiply(slice_b, d2 / (d2 - d3), slice_b);
			v3_add(slice_b, b, slice_b);
		} else {
			//slice_b = a + (d1 / (d1 - d3)) * (c - a);// CA
			slice_b = c - a;
			v3_multiply(slice_b, d1 / (d1 - d3), slice_b);
			v3_add(slice_b, a, slice_b);
		};
	} else {
		// then b is below
		
		if (d3 < 0.0f) {
			//slice_b = a + (d1 / (d1 - d3)) * (c - a);// AC
			slice_b = c - a;
			v3_multiply(slice_b, d1 / (d1 - d3), slice_b);
			v3_add(slice_b, a, slice_b);
		} else {
			//slice_b = b + (d2 / (d2 - d3)) * (c - b);// BC
			slice_b = c - b;
			v3_multiply(slice_b, d2 / (d2 - d3), slice_b);
			v3_add(slice_b, b, slice_b);
		};
	};
};


inline void check_boundry_collision(v3 *player_position, v3 *player_impulse, unsigned int *room_number) {
	current_room = &rooms[*room_number];
	
	float clip_height_bottom = player_position->y;
	float clip_height_top    = player_position->y + 1.5f;
	float d1;
	float d2;
	float d3;
	v3 slice_a = v3(1.0f, 0.0f, 0.0f);
	v3 slice_b = v3(0.0f, 0.0f, 1.0f);
	v3 *a;
	v3 *b;
	v3 *c;
	v3 *normal;
	
	auto current_face = &collision_faces[0];
	
	/*original_position = *player_position;
	mod_position = *player_position;
	
	original_impulse = *player_impulse;
	mod_impulse = *player_impulse;*/
	
	//collided_collision_slice_count = 0;
	//collision_second_slice_count = 0;
	//collision_corner_count = 0;
	
	//collisions = 0;
	
	// @TODO: AAAAAAAAAAAAAAAAAAAAA
	//mod_position += original_impulse;
	
	// Do a second interation??? and look for shared verts, so that we can compute corners and convexity
	for (unsigned int face_index = 0; face_index < collision_face_count; face_index++) {
		current_face = &collision_faces[face_index];
		
		a      = &current_room->elements[current_face->strip_index + 0].position;
		b      = &current_room->elements[current_face->strip_index + 1].position;
		c      = &current_room->elements[current_face->strip_index + 2].position;
		normal = &current_room->elements[current_face->strip_index + 2].normal;
		
		d1 = v3_dot(*a, clip_y_axis) - clip_height_bottom;
		d2 = v3_dot(*b, clip_y_axis) - clip_height_bottom;
		d3 = v3_dot(*c, clip_y_axis) - clip_height_bottom;
		
		if (d1 * d2 < 0.0f) {
			slice_triangle(*a, *b, *c, d1, d2, d3, slice_a, slice_b);
			
		} else if (d2 * d3 < 0.0f) {
			slice_triangle(*b, *c, *a, d2, d3, d1, slice_a, slice_b);
			
		} else if (d3 * d1 < 0.0f) {
			slice_triangle(*c, *a, *b, d3, d1, d2, slice_a, slice_b);
			
		} else {
			continue;// Skip if we can't clip
		};
		
		//check_cock(true, &slice_a, &slice_b, normal);
		
		// Okay maybe check for closest slices and keep them...
		// Fuck array sizes...
	};
	
	//*player_position = mod_position;
	//*player_impulse = mod_impulse;
};

inline bool get_room_collision_boundry(v3 *player_position, v3 *player_impulse, unsigned int *room_number) {
	current_room = &rooms[*room_number];
	
	unsigned int vertice_count = 0;
	unsigned int surface_count = 0;

	if (current_room->elements_size < 1 || current_room->elements_size > V_MAX_VERTICES) {
		return false;
	};
	
	float da_x_clip;
	float db_x_clip;
	float dc_x_clip;
	
	float da_y_clip;
	float db_y_clip;
	float dc_y_clip;
	
	float da_z_clip;
	float db_z_clip;
	float dc_z_clip;
	
	float da_x_max;
	float db_x_max;
	float dc_x_max;
	
	float da_x_min;
	float db_x_min;
	float dc_x_min;
	
	float da_y_max;
	float db_y_max;
	float dc_y_max;
	
	float da_y_min;
	float db_y_min;
	float dc_y_min;
	
	float da_z_max;
	float db_z_max;
	float dc_z_max;
	
	float da_z_min;
	float db_z_min;
	float dc_z_min;
	
	v3 *a;
	v3 *b;
	v3 *c;
	//v3 *normal;
	
	v3 color = v3(0.0f, 1.0f, 0.0f);
	
	float size = 2.0f;
	
	float clipping_x_max = player_position->x + size;
	float clipping_x_min = player_position->x - size;
	float clipping_y_max = player_position->y + size;
	float clipping_y_min = player_position->y - size;
	float clipping_z_max = player_position->z + size;
	float clipping_z_min = player_position->z - size;
	
	collision_face_count = 0;// Reset
	auto current_face = &collision_faces[collision_face_count];
	
	for (unsigned int group_index = 0; group_index < current_room->groups_count; group_index++) {
		group_info *current_group = &current_room->groups[group_index];
		
		for (unsigned int surface_index = 0; surface_index < current_group->strips_count; surface_index++) {
			surface_info *current_surface = &current_room->surfaces[surface_count + surface_index];

			// Triangle Strip!
			for (unsigned int strip_index = 0; strip_index < (current_surface->vert_count - 2); strip_index++) {
				a      = &current_room->elements[vertice_count + strip_index + 0].position;
				b      = &current_room->elements[vertice_count + strip_index + 1].position;
				c      = &current_room->elements[vertice_count + strip_index + 2].position;
				//normal = &current_room->elements[vertice_count + strip_index + 2].normal;
				
				// Only have to calculate 6 dots per triangle
				// Seems similar to point in triangle calculations..
				da_x_clip = v3_dot(*a, clip_x_axis);
				db_x_clip = v3_dot(*b, clip_x_axis);
				dc_x_clip = v3_dot(*c, clip_x_axis);
				
				da_y_clip = v3_dot(*a, clip_y_axis);
				db_y_clip = v3_dot(*b, clip_y_axis);
				dc_y_clip = v3_dot(*c, clip_y_axis);
				
				da_z_clip = v3_dot(*a, clip_z_axis);
				db_z_clip = v3_dot(*b, clip_z_axis);
				dc_z_clip = v3_dot(*c, clip_z_axis);
				
				// Honestly this looks like a matrix setup lol
				da_x_max = da_x_clip - clipping_x_max;
				db_x_max = db_x_clip - clipping_x_max;
				dc_x_max = dc_x_clip - clipping_x_max;
				
				da_x_min = da_x_clip - clipping_x_min;
				db_x_min = db_x_clip - clipping_x_min;
				dc_x_min = dc_x_clip - clipping_x_min;
				
				da_y_max = da_y_clip - clipping_y_max;
				db_y_max = db_y_clip - clipping_y_max;
				dc_y_max = dc_y_clip - clipping_y_max;
				
				da_y_min = da_y_clip - clipping_y_min;
				db_y_min = db_y_clip - clipping_y_min;
				dc_y_min = dc_y_clip - clipping_y_min;
				
				da_z_max = da_z_clip - clipping_z_max;
				db_z_max = db_z_clip - clipping_z_max;
				dc_z_max = dc_z_clip - clipping_z_max;
				
				da_z_min = da_z_clip - clipping_z_min;
				db_z_min = db_z_clip - clipping_z_min;
				dc_z_min = dc_z_clip - clipping_z_min;
				
				if ((da_x_max * db_x_max < 0.0f) || (da_x_min * db_x_min < 0.0f)) {
					
				} else if ((db_x_max * dc_x_max < 0.0f) || (db_x_min * dc_x_min < 0.0f)) {
					
				} else if ((dc_x_max * da_x_max < 0.0f) || (dc_x_min * da_x_min < 0.0f)) {
					
				} else if (a->x <= clipping_x_max && a->x >= clipping_x_min) {
					
				} else if (b->x <= clipping_x_max && b->x >= clipping_x_min) {
					
				} else if (c->x <= clipping_x_max && c->x >= clipping_x_min) {
					
				} else {
					continue;
				};
				
				if ((da_z_max * db_z_max < 0.0f) || (da_z_min * db_z_min < 0.0f)) {
					
				} else if ((db_z_max * dc_z_max < 0.0f) || (db_z_min * dc_z_min < 0.0f)) {
					
				} else if ((dc_z_max * da_z_max < 0.0f) || (dc_z_min * da_z_min < 0.0f)) {
					
				} else if (a->z <= clipping_z_max && a->z >= clipping_z_min) {
					
				} else if (b->z <= clipping_z_max && b->z >= clipping_z_min) {
					
				} else if (c->z <= clipping_z_max && c->z >= clipping_z_min) {
					
				} else {
					continue;
				};
				
				if ((da_y_max * db_y_max < 0.0f) || (da_y_min * db_y_min < 0.0f)) {
					
				} else if ((db_y_max * dc_y_max < 0.0f) || (db_y_min * dc_y_min < 0.0f)) {
					
				} else if ((dc_y_max * da_y_max < 0.0f) || (dc_y_min * da_y_min < 0.0f)) {
					
				} else if (a->y <= clipping_y_max && a->y >= clipping_y_min) {// We could actually use the da_y_max and min
					
				} else if (b->y <= clipping_y_max && b->y >= clipping_y_min) {
					
				} else if (c->y <= clipping_y_max && c->y >= clipping_y_min) {
					
				} else {
					continue;
				};
				
				// Actually because theres going to be tons of collision let's not find the nearest triangles, --
				// and just cache the whole room for all collision tests
				
				//debug::add_line(*a, *b, color);
				// SO CHECK ALL OF THE AXIS'S and if one at all is below the 0 threshold, then store the triangle indice!
				
				if ((collision_face_count + 1) < 2048) {
					current_face = &collision_faces[collision_face_count];
					
					current_face->strip_index = vertice_count + strip_index;
					
					collision_face_count++;
				};
			};

			vertice_count += current_surface->vert_count;
		};

		surface_count += current_group->strips_count;
	};
	
	// Finally check collision with the boundry
	//check_boundry_collision(player_position, player_impulse, room_number);
	
	return true;
};

void check_collision(v3 *player_position, v3 *player_impulse, float time_delta) {
	// Should loop all the rooms
	
	get_room_collision_boundry(player_position, player_impulse, &in_room);
	
	//@ TODO: lol
	*player_position += *player_impulse;
	
	// Lets do 2048 collision face indices, 8192/3 is around this number..
};

}// namespace vcol
