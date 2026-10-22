#include "app.h"
#include "task_list.h"
#include "fb_pipe.h"

flappy_pipe_t flappy_pipes[FLAPPY_PIPE_COUNT];
static uint32_t random_state = 0x12345678u;

static int next_gap() {
    random_state = random_state * 1664525u + 1013904223u;
    return 12 + ((random_state >> 16) % 21);
}

void fb_pipe_handle(ak_msg_t *msg) {
    switch (msg->sig) {
    case FB_PIPE_INIT:
        for (int i = 0; i < FLAPPY_PIPE_COUNT; ++i) {
            flappy_pipes[i].x = 140 + i * 58;
            flappy_pipes[i].gap_top = next_gap();
        }
        break;
    case FB_PIPE_UPDATE:
        for (int i = 0; i < FLAPPY_PIPE_COUNT; ++i) flappy_pipes[i].x -= 2;
        for (int i = 0; i < FLAPPY_PIPE_COUNT; ++i) {
            if (flappy_pipes[i].x + FLAPPY_PIPE_WIDTH < 0) {
                int rightmost = 0;
                for (int j = 0; j < FLAPPY_PIPE_COUNT; ++j)
                    if (flappy_pipes[j].x > rightmost) rightmost = flappy_pipes[j].x;
                flappy_pipes[i].x = rightmost + 58;
                flappy_pipes[i].gap_top = next_gap();
            }
        }
        task_post_pure_msg(FB_COLLISION_ID, FB_COLLISION_CHECK);
        break;
    default:
        break;
    }
}
