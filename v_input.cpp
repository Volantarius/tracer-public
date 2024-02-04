#include "general.h"

#include <iostream>
#include <SDL2/SDL.h>

#include "v_input_buttons.h"

unsigned int g_keys_pressed = 0;
unsigned int g_keys_down = 0;
unsigned int g_keys_up = 0;

// V_INPUT.h
// To handle input and controllers
// Basically we need a way of mapping keyboards and controllers

void key_down(unsigned int key) {
	g_keys_pressed |= key;
	g_keys_down |= key;
};

void key_up(unsigned int key) {
	g_keys_down ^= key;
	g_keys_up |= key;
};

/*void v_input_init() {
	printf("HELLO\n");
};*/

// For now this will atleast make it simple to remap keys
void v_input_event(SDL_Event &Event) {
	if (Event.type == SDL_MOUSEWHEEL) {
		if (Event.wheel.y > 0) {
			g_keys_pressed |= IN_WHEEL_UP;
		} else if (Event.wheel.y < 0) {
			g_keys_pressed |= IN_WHEEL_DOWN;
		};
	};

	g_keys_up = 0;

	if (Event.type == SDL_KEYDOWN) {
		switch (Event.key.keysym.scancode) {
			case SDL_SCANCODE_UP:
				key_down(IN_LOOK_UP);
				break;
			case SDL_SCANCODE_DOWN:
				key_down(IN_LOOK_DOWN);
				break;
			case SDL_SCANCODE_LEFT:
				key_down(IN_LOOK_LEFT);
				break;
			case SDL_SCANCODE_RIGHT:
				key_down(IN_LOOK_RIGHT);
				break;
			
			case SDL_SCANCODE_W:
				key_down(IN_FORWARD);
				break;
			case SDL_SCANCODE_S:
				key_down(IN_BACKWARD);
				break;
			case SDL_SCANCODE_A:
				key_down(IN_STRAFE_LEFT);
				break;
			case SDL_SCANCODE_D:
				key_down(IN_STRAFE_RIGHT);
				break;
				
			case SDL_SCANCODE_LSHIFT:
				key_down(IN_SPRINT);
				break;
			case SDL_SCANCODE_LCTRL:
				key_down(IN_CROUCH);
				break;
			case SDL_SCANCODE_LALT:
				key_down(IN_SLOW);
				break;
			case SDL_SCANCODE_SPACE:
				key_down(IN_JUMP);
				break;
		};
	};

	if (Event.type == SDL_KEYUP) {
		switch (Event.key.keysym.scancode) {
			case SDL_SCANCODE_UP:
				key_up(IN_LOOK_UP);
				break;
			case SDL_SCANCODE_DOWN:
				key_up(IN_LOOK_DOWN);
				break;
			case SDL_SCANCODE_LEFT:
				key_up(IN_LOOK_LEFT);
				break;
			case SDL_SCANCODE_RIGHT:
				key_up(IN_LOOK_RIGHT);
				break;
			
			case SDL_SCANCODE_W:
				key_up(IN_FORWARD);
				break;
			case SDL_SCANCODE_S:
				key_up(IN_BACKWARD);
				break;
			case SDL_SCANCODE_A:
				key_up(IN_STRAFE_LEFT);
				break;
			case SDL_SCANCODE_D:
				key_up(IN_STRAFE_RIGHT);
				break;
				
			case SDL_SCANCODE_LSHIFT:
				key_up(IN_SPRINT);
				break;
			case SDL_SCANCODE_LCTRL:
				key_up(IN_CROUCH);
				break;
			case SDL_SCANCODE_LALT:
				key_up(IN_SLOW);
				break;
			case SDL_SCANCODE_SPACE:
				key_up(IN_JUMP);
				break;
		};
	};
};

void v_input_clearkeys() {
	g_keys_up = 0;
	g_keys_pressed = 0;
};

/*void v_input_tick() {
	
};

void v_input_close() {
	
};*/
