#include "app.h"
#include "task_list.h"
#include "fb_bird.h"


static void flap();
static void tick();

flappy_bird_t flappy_bird;


void fb_bird_handle(ak_msg_t *msg){
    switch(msg->sig){
        case FB_BIRD_INIT:{
            flappy_bird.visible = true;
            flappy_bird.pos_x = FLAPPY_BIRD_START_X;
            flappy_bird.pos_y = FLAPPY_BIRD_START_Y;
            flappy_bird.action_image = 0;
            flappy_bird.velocity = 0;
            flappy_bird.state = BirdState::READY;
            flappy_bird.position = FLAPPY_BIRD_START_Y * FLAPPY_BIRD_SCALE;
        }break;
        case  FB_BIRD_FLAP:{
            if (flappy_bird.state == BirdState::READY || flappy_bird.state == BirdState::FLYING) {
                flappy_bird.state = BirdState::FLYING;
                flap();
            } else if (flappy_bird.state == BirdState::DEAD) {
                task_post_pure_msg(FB_GAME_ID, FB_GAME_RESTART);
            }
        }break;
        case FB_BIRD_TICK:{
            if (flappy_bird.state == BirdState::FLYING) {
                tick();
                task_post_pure_msg(FB_PIPE_ID, FB_PIPE_UPDATE);
            }
        }break;
        case FB_BIRD_TOGGLE_PAUSE:{
            if (flappy_bird.state == BirdState::FLYING)
                flappy_bird.state = BirdState::PAUSED;
            else if (flappy_bird.state == BirdState::PAUSED)
                flappy_bird.state = BirdState::FLYING;
        }break;
        case FB_BIRD_PAUSE:{
            if (flappy_bird.state == BirdState::FLYING) {
                flappy_bird.state = BirdState::PAUSED;
            }
        }break;
        case FB_BIRD_RESUME:{
            if (flappy_bird.state == BirdState::PAUSED) {
                flappy_bird.state = BirdState::FLYING;
            }
        }break;
        case FB_BIRD_COLLISION:{
            if (flappy_bird.state == BirdState::FLYING) {
                flappy_bird.state = BirdState::DEAD;
                flappy_bird.velocity = 0;
            }
        }break;
        default: break;
    }
}

void flap(){
    flappy_bird.velocity = FLAPPY_BIRD_FLAP_VELOCITY;
}


void tick() {
    flappy_bird.velocity += FLAPPY_BIRD_GRAVITY;
    if (flappy_bird.velocity > FLAPPY_BIRD_MAX_FALL_VELOCITY) flappy_bird.velocity = FLAPPY_BIRD_MAX_FALL_VELOCITY;
    flappy_bird.position += flappy_bird.velocity;
    flappy_bird.pos_y = fb_bird_y();
}

int fb_bird_y() {
    // Floor negative fixed-point positions as well as positive positions.
    const int32_t value = flappy_bird.position;
    return value / FLAPPY_BIRD_SCALE - (value % FLAPPY_BIRD_SCALE < 0 ? 1 : 0);
}
