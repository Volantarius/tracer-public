#ifndef V_INPUT_H
#define V_INPUT_H
#ifdef _WIN32
#pragma once
#endif

#include <SDL2/SDL.h>
#include "v_input_buttons.h"

// We set flags for the game to take input for all the basic shiz

extern unsigned int g_keys_pressed;
extern unsigned int g_keys_down;// Heard you like inputs
extern unsigned int g_keys_up;  // Heard you don't like inputs

//__forceinline void v_input_init();

void v_input_event(SDL_Event &Event);

// Clear all keys from UP value
void v_input_clearkeys();

/*__forceinline void v_input_tick();

__forceinline void v_input_close();*/

#endif
