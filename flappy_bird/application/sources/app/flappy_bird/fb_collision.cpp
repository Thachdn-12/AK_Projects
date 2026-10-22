#include "app.h"
#include "task_list.h"
#include "fb_collision.h"
#include "fb_bird.h"
#include "fb_pipe.h"

void fb_collision_handle(ak_msg_t *msg) {
    if (msg->sig != FB_COLLISION_CHECK || flappy_bird.state != FLYING) return;

    const int top = fb_bird_y() - FLAPPY_BIRD_RADIUS;
    const int bottom = fb_bird_y() + FLAPPY_BIRD_RADIUS;
    const int left = FLAPPY_BIRD_START_X - FLAPPY_BIRD_RADIUS;
    const int right = FLAPPY_BIRD_START_X + FLAPPY_BIRD_RADIUS;
    bool hit = top < 0 || bottom >= FLAPPY_GROUND_Y;

    for (int i = 0; i < FLAPPY_PIPE_COUNT && !hit; ++i) {
        const flappy_pipe_t &pipe = flappy_pipes[i];
        const bool overlaps_x = right >= pipe.x && left < pipe.x + FLAPPY_PIPE_WIDTH;
        hit = overlaps_x && (top < pipe.gap_top || bottom >= pipe.gap_top + FLAPPY_PIPE_GAP);
    }
    if (hit) task_post_pure_msg(FB_BIRD_ID, FB_BIRD_COLLISION);
}
