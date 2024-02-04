#ifndef COLLISION_H
#define COLLSIION_H

#ifdef _WIN32
#pragma once
#endif

namespace vcol {

extern inline void init();
extern inline void check_collision(v3 *player_position, v3 *player_impulse, float time_delta);

}// vcol

#endif