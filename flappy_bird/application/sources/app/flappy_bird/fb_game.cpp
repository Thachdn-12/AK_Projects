#include "app.h"
#include "task_list.h"
#include "fb_game.h"

static void reset_game() {
    task_post_pure_msg(FB_BIRD_ID, FB_BIRD_INIT);
    task_post_pure_msg(FB_PIPE_ID, FB_PIPE_INIT);
}

void fb_game_handle(ak_msg_t *msg) {
    switch (msg->sig) {
    case FB_GAME_SETUP:
        reset_game();
        break;
    case FB_GAME_RESTART:
        // The bird requests this after an UP event in the DEAD state.
        // Worker messages share a FIFO queue: reset both models before flapping.
        reset_game();
        task_post_pure_msg(FB_BIRD_ID, FB_BIRD_FLAP);
        break;
    case FB_GAME_FLAP:
        task_post_pure_msg(FB_BIRD_ID, FB_BIRD_FLAP);
        break;
    case FB_GAME_TICK:
        task_post_pure_msg(FB_BIRD_ID, FB_BIRD_TICK);
        break;
    case FB_GAME_TOGGLE_PAUSE:
        task_post_pure_msg(FB_BIRD_ID, FB_BIRD_TOGGLE_PAUSE);
        break;
    default:
        break;
    }
}
