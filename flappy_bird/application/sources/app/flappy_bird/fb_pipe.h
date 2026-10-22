#ifndef FB_PIPE_H
#define FB_PIPE_H

#include "ak.h"
#include "message.h"

#define FLAPPY_GROUND_Y 62
#define FLAPPY_PIPE_WIDTH 12
#define FLAPPY_PIPE_GAP 25
#define FLAPPY_PIPE_COUNT 3

typedef struct {
    int x;
    int gap_top;
} flappy_pipe_t;

extern flappy_pipe_t flappy_pipes[FLAPPY_PIPE_COUNT];
void fb_pipe_handle(ak_msg_t *msg);

#endif
