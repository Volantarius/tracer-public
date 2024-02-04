#include "general.h"
#include "level_manager.h"

#include <iostream>
#include <cstdio>
#include <assert.h>

unsigned char* ts_load_texture(int *width, int *height, const char *filename) {
	FILE* fh = std::fopen(filename, "rb");
	long lSize;
	unsigned char* buffer;
	size_t result;

	unsigned int readpos;

	if (!fh) {
		printf("Can't load texture!\n");
		return 0;
	}

	// Get size of file
	std::fseek(fh, 0, SEEK_END);
	lSize = ftell(fh);
	std::rewind(fh);

	buffer = (unsigned char*)malloc(sizeof(unsigned char)*lSize);
	if (buffer == NULL) {
		printf("Memory error!\n");
		return 0;
	}

	// result is how many bytes read
	// the buffer is still holding all the data!
	result = std::fread(buffer, 1, lSize, fh);

	if (result != lSize) {
		printf("Reading error!\n");
		std::fclose(fh);
		return 0;
	}

	std::fclose(fh);

	if ('Q' != ((char)buffer[0]) && 'M' != ((char)buffer[0])) {
		std::free(buffer);
		return 0;
	}

	// Q6 or M6 are raw images with no palette!
	// Q8 or M8 are paletted images
	//bool raw = '6' == ((char)buffer[1]);

	readpos = 3;

	// TEXTURE WIDTH
	int digits_width = read_until_from_right(buffer, readpos, '\x20');

	//int width = read_as_number_from_right(buffer, digits_width, readpos);
	*width = read_as_number_from_right(buffer, digits_width, readpos);

	readpos += digits_width + 1;

	// TEXTURE HEIGHT
	int digits_height = read_until_from_right(buffer, readpos, '\x0A');

	//int height = read_as_number_from_right(buffer, digits_height, readpos);
	*height = read_as_number_from_right(buffer, digits_height, readpos);

	readpos += digits_height + 1;

	// COLOR
	int digits_color = read_until_from_right(buffer, readpos, '\x20');

	int color_depth = read_as_number_from_right(buffer, digits_color, readpos);

	readpos += digits_color + 1;

	// Confirmation of timesplitters texture
	assert('v' == (char)buffer[readpos]);

	readpos += 2;

	// PALETTE SIZE
	int digits_palette = read_until_from_right(buffer, readpos, '\x20');

	int palette_size = read_as_number_from_right(buffer, digits_palette, readpos);

	readpos += digits_palette + 1;

	// Adjust only if we are actually off by mod 4 bytes
	if (readpos % 4 > 0) { readpos += 4 - (readpos % 4); }

	int palette_readpos = readpos;
	int index_readpos = readpos + (palette_size * 4);

	unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char) * *width * *height * 4);

	if (data == NULL) {
		printf("Memory error!\n");
		return 0;
	}

	// From the buffer being signed, the bits are in half when put into the final
	// so keep all of it unsigned and handle them as bytes...
	// May have to change the type and make sure other platforms stay with bytes..
	// unsigned char can be different on platforms
	for (int i = 0; i < *width * *height; i++) {
		int index = ((int)buffer[index_readpos + i]) * 4;

		int ii = i * 4;

		data[ii + 0] = (unsigned char)buffer[palette_readpos + index + 0];
		data[ii + 1] = (unsigned char)buffer[palette_readpos + index + 1];
		data[ii + 2] = (unsigned char)buffer[palette_readpos + index + 2];
		data[ii + 3] = ((unsigned char)buffer[palette_readpos + index + 3]) * 2;
	}

	free(buffer);

	//free(data);
	// Gotta free the data thats returned...

	return data;
};

inline void fill_element(
	unsigned char *buffer,
	level_mesh_element *element,
	unsigned int &vertice_index,
	unsigned int &buffer_offset,
	unsigned int *address_verts,
	unsigned int *address_uvs,
	unsigned int *address_vcolor
) {
	unsigned int base_index = vertice_index + buffer_offset;
	
	unsigned int index = base_index * 12;// 4 * 3 (Stride!)
	
	unsigned int readpos = *address_verts;
	
	v_read_float(buffer, element->position.x, readpos + 0 + index);
	v_read_float(buffer, element->position.y, readpos + 4 + index);
	v_read_float(buffer, element->position.z, readpos + 8 + index);
	
	index = base_index * 16;// 4 * 4
	
	readpos = *address_uvs;
	
	v_read_float(buffer, element->uv.x, readpos + 0 + index);
	v_read_float(buffer, element->uv.y, readpos + 4 + index);
	
	index = base_index * 4;// 4 bytes
	
	readpos = *address_vcolor;
	
	unsigned char red = (unsigned char)buffer[readpos + 0 + index];
	unsigned char gre = (unsigned char)buffer[readpos + 1 + index];
	unsigned char blu = (unsigned char)buffer[readpos + 2 + index];
	unsigned char alp = (unsigned char)buffer[readpos + 3 + index];
	
	// Need to be converted to floats...
	// PS2 is half intesity
	element->color.x = (float) (red / 127.0f);
	element->color.y = (float) (gre / 127.0f);
	element->color.z = (float) (blu / 127.0f);
	element->alpha   = (float) (alp / 127.0f);
};

//unsigned char* ts_LoadLevel(const char* filename)
bool ts_load_level(const char* filename) {
	FILE* fh = std::fopen(filename, "rb");
	
	long lSize;
	unsigned char* buffer;
	size_t result;

	unsigned int readpos;

	if (!fh) {
		printf("Can't load level!\n");
		return false;
	};

	// Get size of file
	std::fseek(fh, 0, SEEK_END);
	lSize = ftell(fh);
	std::rewind(fh);

	buffer = (unsigned char*)malloc(sizeof(unsigned char)*lSize);
	if (buffer == NULL) {
		printf("Memory error!\n");
		return false;
	};

	// result is how many bytes read
	// the buffer is still holding all the data!
	result = std::fread(buffer, 1, lSize, fh);

	if (result != lSize) {
		printf("Reading error!\n");
		std::fclose(fh);
		return false;
	};

	std::fclose(fh);

	// **** Addresses! ****
	// Addresses at the header
	unsigned int a_materials;
	unsigned int a_rooms;
	unsigned int a_areaportals;
	//unsigned int a_unknown;
	
	v_read_uint32(buffer, a_materials, 0);
	v_read_uint32(buffer, a_rooms, 4);
	v_read_uint32(buffer, a_areaportals, 8);
	
	int material_count = (a_rooms - a_materials) / 16;
	int room_count = ((a_areaportals - a_rooms) / 4 / 11) - 1;
	
	#ifdef V_DEBUG
	printf("Level: Materials: %d Rooms: %d\n", material_count, room_count);
	#endif
	
	// Initialize the level for the engine
	room_info* level_rooms = init_level_rooms(room_count - 1);// level_manager
	material* materials = init_level_materials(material_count - 1);
	
	// **** Textures ****
	
	readpos = a_materials;
	
	for (unsigned int material_index = 0; material_index < (material_count - 1); material_index++) {
		unsigned int texture_index;
		
		v_read_uint32(buffer, texture_index, readpos);
		readpos += 4;
		v_read_uint32(buffer, materials[material_index].flags_a, readpos);
		readpos += 4;
		v_read_uint32(buffer, materials[material_index].flags_b, readpos);
		readpos += 4;
		v_read_uint32(buffer, materials[material_index].flags_c, readpos);
		readpos += 4;
		
		
		materials[material_index].name = new char[20];
		
		// Eehh ugggh idk about this function
		snprintf(materials[material_index].name, 20, "tstextures\\%04d.qpm", texture_index);
	};
	
	// **** Load Meshs! ****
	
	//for (int room_index = 0; room_index < (room_count - 1); room_index++) {
	for (int room_index = 1; room_index < room_count; room_index++) {
		// Setup room info
		readpos = a_rooms + (room_index * 4 * 11);
		
		readpos += 20;// 4 * 5
		
		room_info *current_room = &level_rooms[room_index - 1];
		
		v_read_float(buffer, current_room->bounds_min.x, readpos);
		readpos += 4;
		v_read_float(buffer, current_room->bounds_min.y, readpos);
		readpos += 4;
		v_read_float(buffer, current_room->bounds_min.z, readpos);
		readpos += 4;
		
		v_read_float(buffer, current_room->bounds_max.x, readpos);
		readpos += 4;
		v_read_float(buffer, current_room->bounds_max.y, readpos);
		readpos += 4;
		v_read_float(buffer, current_room->bounds_max.z, readpos);
		readpos += 4;
		// End of room info

		// Initialize stuff
		current_room->adjacent_rooms_count = 0;
		current_room->areaportals_count = 0;

		//if (room_index != 2) continue;// TEMP: Let's parse one room please!
		
		// Mesh address
		unsigned int a_mesh;
		
		unsigned int pri_groups;
		unsigned int sec_groups;
		unsigned int unknown_info;
		
		unsigned int pri_strips;
		unsigned int pri_verts;
		unsigned int pri_uvs;
		unsigned int pri_vcols;
		
		unsigned int sec_strips;
		unsigned int sec_verts;
		unsigned int sec_uvs;
		unsigned int sec_vcols;
		
		// Don't write to readpos
		v_read_uint32(buffer, a_mesh, a_rooms + (4 * 11 * room_index));
		
		//readpos = a_mesh - 10;// Weird UInt32 number
		
		// Primary mesh info address
		readpos = a_mesh - 8 - 64;
		
		// **** Addresses of groups and data ****
		v_read_uint32(buffer, pri_groups, readpos); readpos += 4;
		v_read_uint32(buffer, sec_groups, readpos); readpos += 4;
		
		// Unknown area, we need this address
		v_read_uint32(buffer, unknown_info, readpos); readpos += 4;
		
		v_read_uint32(buffer, pri_strips, readpos); readpos += 4;
		v_read_uint32(buffer, pri_verts, readpos); readpos += 4;
		v_read_uint32(buffer, pri_uvs, readpos); readpos += 4;
		v_read_uint32(buffer, pri_vcols, readpos); readpos += 4;
		
		readpos += 8;// skip two unknowns..
		
		v_read_uint32(buffer, sec_strips, readpos); readpos += 4;
		v_read_uint32(buffer, sec_verts, readpos); readpos += 4;
		v_read_uint32(buffer, sec_uvs, readpos); readpos += 4;
		v_read_uint32(buffer, sec_vcols, readpos); readpos += 4;
		
		int pri_vert_count = (pri_groups - pri_vcols) / 4;
		int sec_vert_count = (sec_groups - sec_vcols) / 4;
		
		int pri_strip_size = (pri_verts - pri_strips) / 16;
		int sec_strip_size = (sec_verts - sec_strips) / 16;
		
		// Primary mesh info address - 8
		int end_of_mesh_address = a_mesh - 8 - 64 - 8;
		
		// **** GROUPS ****
		
		int pri_group_size = 0;
		int sec_group_size = 0;
		
		// There can be additional info for a mesh, and we need to adjust the calculation of size
		if (pri_groups > 0) {
			pri_group_size = end_of_mesh_address - pri_groups;
			
			if (sec_strips > 0) {
				pri_group_size = sec_strips - pri_groups;
			} else if (unknown_info > 0) {
				pri_group_size = unknown_info - pri_groups;
			};
			
			pri_group_size /= 24;// 4 * 6
			
			pri_group_size -= 1;
			
			current_room->groups = (group_info*)malloc(sizeof(group_info) * pri_group_size);
		};
		
		/*if (sec_groups > 0) {
			sec_group_size = end_of_mesh_address - sec_groups;
			
			if (unknown_info > 0) {
				sec_group_size = unknown_info - sec_groups;
			};
			
			sec_group_size /= 24;// 4 * 6
			
			sec_group_size -= 1;
		};*/
		
		// Make sure we don't allocate zero memory lol
		if (pri_strips > 0) {
			current_room->surfaces = (surface_info*)malloc(sizeof(surface_info) * pri_strip_size);
		} else {
			current_room->surfaces = NULL;
			current_room->elements = NULL;
			current_room->groups = NULL;
			current_room->elements_size = 0;
			current_room->groups_count = 0;// CLEAN THIS UP
		};
		
		// TODO: AAAA
		//if (current_room->surfaces == NULL || sec_strips_buffer == NULL)
		if (current_room->surfaces == NULL) {
			printf("OOPS! Room: %d, Strips: %d, Groups: %d\n", room_index, pri_strip_size, pri_group_size);
			printf("Memory error!\n");
			
			continue;
		};
		
		int final_vert_count = 0;
		
		if (pri_strips > 0) {
			for (unsigned int i = 0; i < pri_strip_size; i++) {
				readpos = pri_strips + (i * 16);// 4 * 4
				
				surface_info* strip = &current_room->surfaces[i];
				
				// BYTES!
				v_read_uint4(buffer, strip->vert_count, readpos);
				readpos += 1;
				v_read_uint4(buffer, strip->flags_a, readpos);
				readpos += 1;
				v_read_uint4(buffer, strip->flags_b, readpos);
				readpos += 1;
				
				final_vert_count += strip->vert_count;
			};
			
			// TODO: V_MAX_VERTICES, check if we are going to make something way to big for what we need
			
			current_room->elements = (level_mesh_element*)malloc(sizeof(level_mesh_element) * final_vert_count);
			
			current_room->elements_size = final_vert_count;
			
			// >>>> GENERATE MESH <<<<
			if (pri_groups > 0) {
				int strip_index = 0;
				int new_vert_index = 0;// For our final element buffer
				
				current_room->groups_count = pri_group_size;
				
				for (unsigned int group_index = 0; group_index < pri_group_size; group_index++) {
					readpos = pri_groups + (group_index * 24);// 6 * 4
					
					group_info *current_group = &current_room->groups[group_index];
					
					//unsigned int material_id;
					unsigned int group_strips_count;
					unsigned int group_vertices_start;
					//unsigned int group_vertices_end;
					
					v_read_uint32(buffer, current_group->material_id, readpos);
					readpos += 4;
					readpos += 4;// skip unknown
					v_read_uint32(buffer, group_strips_count, readpos);
					readpos += 4;
					v_read_uint32(buffer, group_vertices_start, readpos);
					//readpos += 4;
					//v_read_uint32(buffer, group_vertices_end, readpos);
					// another unknown skipped
					
					// VOLANTARIUS probably put this into the read func itself!
					current_group->strips_count = group_strips_count;
					
					unsigned int offset = group_vertices_start;
					
					for (unsigned int k = 0; k < group_strips_count; k++)
					{
						surface_info *strip = &current_room->surfaces[strip_index];
						
						for (unsigned int vert_index = 0; vert_index < strip->vert_count; vert_index++)
						{
							level_mesh_element *element = &current_room->elements[new_vert_index];
							
							fill_element(buffer, element, vert_index, offset, &pri_verts, &pri_uvs, &pri_vcols);
							
							// *************************
							v3 normal = v3(0.0f, 1.0f, 0.0f);
							
							int ind_a = 0;
							int ind_b = 0;
							int ind_c = 0;
							
							int odd_order = 1;
							
							// Flip strip
							// TODO: make simplier lol
							if ((strip->flags_b & 1) == 1) {
								odd_order = 0;
							};
							
							if (vert_index > 1)
							{
								if (vert_index % 2 == odd_order)
								{
									ind_a = new_vert_index - 0;
									ind_b = new_vert_index - 1;
									ind_c = new_vert_index - 2;
								} else {
									ind_a = new_vert_index - 2;
									ind_b = new_vert_index - 1;
									ind_c = new_vert_index - 0;
								};
								
								v3_face_normal(current_room->elements[ind_a].position, current_room->elements[ind_b].position, current_room->elements[ind_c].position, normal);
							};
							
							element->normal = normal;
							
							if (vert_index == 2) {
								current_room->elements[new_vert_index - 1].normal = normal;
								current_room->elements[new_vert_index - 2].normal = normal;
							};
							// *************************
							
							new_vert_index++;
						};
						
						offset += strip->vert_count;
						strip_index += 1;
					};
				};//groups
			};//pri_groups
		};
		
		// TODO: I guess throw the secondaries at the end of the elements buffer
		
		/*if (sec_strips > 0)
		{
			for (int i = 0; i < sec_strip_size; i++)
			{
				readpos = sec_strips;
				
				surface_info* strip = &sec_strips_buffer[i];
				
				// BYTES!
				v_read_uint4(buffer, strip->vert_count, readpos);
				readpos += 1;
				v_read_uint4(buffer, strip->flags_a, readpos);
				readpos += 1;
				v_read_uint4(buffer, strip->flags_b, readpos);
				readpos += 1;
				
				final_vert_count += strip->vert_count;
			};
			
			// >>> GENERATE _secondary_ MESH <<<<
		};*/
		
		/*#ifdef V_DEBUG
		printf("Room %d: %d\n", room_index, final_vert_count);
		#endif*/
	};
	
	// **** Area Portals ****
	// Area portals are fan meshs
	
	unsigned a_areaportal_room = 0;
	
	readpos = a_rooms + (1 * 4 * 11);
	
	// Gotta read the first room's mesh address because the areaportal list is above it
	// Probably another way of getting the area portal count, but this works for now
	v_read_uint32(buffer, a_areaportal_room, readpos);
	
	readpos = a_areaportal_room - 60;
	
	unsigned int areaportal_end = 0;
	v_read_uint32(buffer, areaportal_end, readpos);
	
	unsigned int areaportal_count = (areaportal_end - a_areaportals) / 16;
	areaportal_count -= 1;
	areaportal_count *= 4;
	
	// setup area portals for the level manager
	areaportal_info* areaportals = init_level_areaportals(areaportal_count - 1);
	
	unsigned int areaportal_vertice_count = 0;
	
	for (unsigned int areaportal_index = 1; areaportal_index < areaportal_count; areaportal_index++) {
		readpos = a_areaportals + (areaportal_index * 4);
		
		unsigned int areaportal_address;
		v_read_uint32(buffer, areaportal_address, readpos);
		
		readpos = areaportal_address;
		
		unsigned int room_a;
		unsigned int room_b;
		unsigned int unknown_a;
		unsigned int unknown_b;
		unsigned int unknown_c;
		unsigned int vert_count;
		
		v_read_uint32(buffer, room_a, readpos);
		readpos += 4;
		v_read_uint32(buffer, room_b, readpos);
		readpos += 4;
		
		// Room indices are off by one
		// BUTT we have a zero room
		room_a -= 1;
		room_b -= 1;
		
		// Unknowns look like maybe 16 bit ints for flags...
		// Left side is always pretty large
		v_read_uint32(buffer, unknown_a, readpos);
		readpos += 4;
		v_read_uint32(buffer, unknown_b, readpos);
		readpos += 4;
		v_read_uint32(buffer, unknown_c, readpos);
		readpos += 4;
		
		v_read_uint32(buffer, vert_count, readpos);
		
		areaportal_info *level_ap = &areaportals[areaportal_index - 1];
		
		level_ap->room_a = room_a;
		level_ap->room_b = room_b;
		level_ap->vert_count = vert_count;
		level_ap->offset = areaportal_vertice_count;
		
		room_info *level_room_a = &level_rooms[room_a];
		room_info *level_room_b = &level_rooms[room_b];
		
		bool portal_a_good = true;
		bool portal_b_good = true;
		
		// Assign adjacent rooms!
		for (unsigned int i = 0; i < level_room_a->adjacent_rooms_count; i++) {
			if (room_b == level_room_a->adjacent_rooms[i]) {
				portal_a_good = false;
				break;
			};
		};
		
		for (unsigned int i = 0; i < level_room_b->adjacent_rooms_count; i++) {
			if (room_a == level_room_b->adjacent_rooms[i]) {
				portal_b_good = false;
				break;
			};
		};
		
		if (portal_a_good) {
			level_room_a->adjacent_rooms[level_room_a->adjacent_rooms_count] = room_b;
			level_room_a->adjacent_rooms_count += 1;
		};
		
		if (portal_b_good) {
			level_room_b->adjacent_rooms[level_room_b->adjacent_rooms_count] = room_a;
			level_room_b->adjacent_rooms_count += 1;
		};
		
		//printf("pp %d, %d %d verts: %d\n", areaportal_index, room_a, room_b, vert_count);
		// Alright AP's loading up!
		//printf("%x %x %x\n", unknown_a, unknown_b, unknown_c);
		
		// Put all the vertices into a buffer for the level
		// Need to count them all
		
		// Then add the area portal to the level
		// Right now doesn't need the vertices and screen checking stuffs
		
		areaportal_vertice_count += vert_count;
	};

	free(buffer);

	// We are going to setup a level manager for handling the meshs and all the other yummy level stuff

	return true;
};
