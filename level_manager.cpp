#include "general.h"
#include "vector.h"
#include "level_manager.h"// TODO: REMOVE, this is just for the structs

#include <iostream>
#include <cstdio>

// Invalidate whenever there is a external change, or unfinished changes
// >> Maybe??
///bool level_valid = false;

// Setting up meshs I guess could be done elsewhere, but for now we should just do it here
// Pass a pointer to the data so that a external thing can setup a mesh and yeah

// Yup swapping textures and DrawArrays can be easily offset
// So we can have the mesh in strip groups that we can just iterate over carefully

unsigned int level_room_count;
unsigned int level_material_count;
unsigned int level_areaportal_count;

room_info *rooms;
material *materials;
areaportal_info *areaportals;

room_info* init_level_rooms(unsigned int room_count) {
	level_room_count = room_count;
	
	// Should I free the old data before allocating more?
	// Actually we can do that with cleanup_level
	rooms = (room_info*)malloc(sizeof(room_info) * room_count);
	
	if (rooms == NULL) {
		printf("Can't allocate room info!\n");
		return rooms;
	};
	
	return rooms;
};

material* init_level_materials(unsigned int material_count) {
	level_material_count = material_count;
	
	materials = (material*)malloc(sizeof(material) * material_count);
	
	if (materials == NULL) {
		printf("Can't allocate materials!\n");
		return materials;
	};

	return materials;
};

areaportal_info* init_level_areaportals(unsigned int areaportal_count) {
	level_areaportal_count = areaportal_count;
	
	// Should I free the old data before allocating more?
	// Actually we can do that with cleanup_level
	areaportals = (areaportal_info*)malloc(sizeof(areaportal_info) * areaportal_count);
	
	if (areaportals == NULL) {
		printf("Can't allocate area portals!\n");
		return areaportals;
	};
	
	return areaportals;
};

void get_rooms_count(unsigned int &in) {
	in = level_room_count;
};

void get_materials_count(unsigned int &in) {
	in = level_material_count;
};

material* get_level_materials() {
	return materials;
};

room_info* get_rooms() {
	return rooms;
};

/*
	Elements >> Surfaces >> Groups >> Mesh
*/
void cleanup_level() {
	for (unsigned int i = 0; i < level_room_count; i++) {
		free(rooms[i].elements);
		free(rooms[i].surfaces);
		free(rooms[i].groups);
	};
	
	free(areaportals);
	
	free(rooms);
	
	free(materials);
};