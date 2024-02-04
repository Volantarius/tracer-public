#ifndef V_INPUT_BUTTONS_H
#define V_INPUT_BUTTONS_H
#ifdef _WIN32
#pragma once
#endif

#define IN_FORWARD      (1 << 0)
#define IN_BACKWARD     (1 << 1)
#define IN_STRAFE_LEFT  (1 << 2)
#define IN_STRAFE_RIGHT (1 << 3)
#define IN_LOOK_UP      (1 << 4)
#define IN_LOOK_DOWN    (1 << 5)
#define IN_LOOK_RIGHT   (1 << 6)
#define IN_LOOK_LEFT    (1 << 7)
#define IN_SPRINT       (1 << 8)
#define IN_CROUCH       (1 << 9)
#define IN_JUMP         (1 << 10)
#define IN_SLOW         (1 << 11)

#define IN_WHEEL_UP     (1 << 12)
#define IN_WHEEL_DOWN   (1 << 13)

#endif