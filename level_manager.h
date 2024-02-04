#ifndef LEVEL_MANAGER_H
#define LEVEL_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "vector.h"

struct level_mesh_element {
	v3 position;
	v3 color;
	vector_type alpha;
	v3 normal;
	v2 uv;
};

struct areaportal_element {
	v3 position;
};

// We will probably need a different struct for indexed meshs.
// That will contain the indices along with elements.
struct surface_info {
	unsigned int vert_count;
	unsigned int flags_a;// probably should be a unsigned char...
	unsigned int flags_b;
};

struct group_info {
	unsigned int material_id;// This should be handled with a manager?
	unsigned int strips_count;
};

struct areaportal_info {
	unsigned int room_a;
	unsigned int room_b;
	unsigned int vert_count;
	unsigned int offset;
};

// TODO: Replace with our materials
struct material {
	char *name;
	unsigned int flags_a;
	unsigned int flags_b;
	unsigned int flags_c;

	unsigned int gl_texture;//Throw the id for the GL texture here!
};

struct room_info {
	//unsigned int unknown_a;
	//unsigned int collision;// Collision setup address
	//unsigned int unknown_c;
	//unsigned int unknown_d;

	// Address info
	// Unk A: is 0x00 terminated; List of integers?
	// Unk C: is 0x00 terminated; Could be area portal info
	// Unk D: Appears to be fixed stride, given next address size can be determined
	// Unk D could be similar to how area portals are created

	// Collision info
	// These are fan meshs, probably for AI instead of the player
	// It is not confirmed though, but seems likely since --
	// in TS1 the player has a full hull collision system

	// Room bounding box, used for frustrum
	v3 bounds_min;
	v3 bounds_max;

	unsigned int groups_count;
	unsigned int elements_size;

	group_info *groups;
	surface_info *surfaces;
	level_mesh_element *elements;

	// So we should list the area portal indices that connect to this room??
	// Probably one buffer for all of the area portal vertices
	// Need an offset and size so we can index them individually
	unsigned int adjacent_rooms[64]; // directly connected rooms via areaportals
	unsigned int adjacent_rooms_count;
	unsigned int areaportals[64]; // Indices to the areaportal
	unsigned int areaportals_count;
};

// Although not written but these are actually set to extern with the source file
// And it appears that if I dont include the header for the source of a same name, some weirdness happens
// Where the definitions aren't carried and looks weird
room_info* init_level_rooms(unsigned int room_count);
material* init_level_materials(unsigned int material_count);
areaportal_info* init_level_areaportals(unsigned int areaportal_count);

// extern inline does work! although its hard to tell if its nessecary in this
// Also we should probably use namespace so we can make this safer to use, like the global variables
void get_rooms_count(unsigned int &in);
void get_materials_count(unsigned int &in);

material* get_level_materials();
room_info* get_rooms();

void cleanup_level();

#endif