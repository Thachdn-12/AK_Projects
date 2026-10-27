#ifndef __FLAPPY_BIRD_H__
#define __FLAPPY_BIRD_H__

#include <stdint.h>
#include "ak.h"
#include "message.h"

#define FLAPPY_BIRD_SCALE (100)
#define FLAPPY_BIRD_START_Y (32)
#define FLAPPY_BIRD_START_X (28)
#define FLAPPY_BIRD_RADIUS (3)
#define FLAPPY_BIRD_GRAVITY (40)
#define FLAPPY_BIRD_FLAP_VELOCITY (-280)
#define FLAPPY_BIRD_MAX_FALL_VELOCITY (400)

typedef enum
{
   READY = 0,
   FLYING,
   PAUSED,
   DEAD
}BirdState;

typedef struct
{
    bool visible;
    int32_t pos_x, pos_y; // Current position of the bird
    int32_t velocity;      // Current vertical velocity of the bird
    BirdState state;         // Ready, flying, paused, or dead
    uint8_t action_image;
    int32_t position;
} flappy_bird_t;

extern flappy_bird_t flappy_bird;
void fb_bird_handle(ak_msg_t *msg);
int fb_bird_y();

#endif //__FLAPPY_BIRD_H__
