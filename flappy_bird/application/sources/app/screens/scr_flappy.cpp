#include "scr_flappy.h"
#include "../flappy_bird/fb_bird.h"
#include "../flappy_bird/fb_pipe.h"

static void text(int x, int y, const char *value) {
    view_render.setCursor(x, y);
    view_render.print(value);
}

static void view_scr_flappy() {
    view_render.clear();
    view_render.setTextSize(1);
    view_render.setTextColor(WHITE);
    if (flappy_bird.state == READY) {
        text(31, 3, "FLAPPY BIRD");
        view_render.fillCircle(64, 23, 6, WHITE);
        view_render.drawPixel(66, 21, BLACK);
        text(10, 36, "UP: START / FLAP");
        text(10, 47, "MODE: PAUSE");
        return;
    }

    for (int i = 0; i < FLAPPY_PIPE_COUNT; ++i) {
        const flappy_pipe_t &pipe = flappy_pipes[i];
        view_render.drawRect(pipe.x, 0, FLAPPY_PIPE_WIDTH, pipe.gap_top, WHITE);
        const int gap_bottom = pipe.gap_top + FLAPPY_PIPE_GAP;
        view_render.drawRect(pipe.x, gap_bottom, FLAPPY_PIPE_WIDTH,
                             FLAPPY_GROUND_Y - gap_bottom, WHITE);
    }
    view_render.drawLine(0, FLAPPY_GROUND_Y, 127, FLAPPY_GROUND_Y, WHITE);
    if (flappy_bird.visible)
        view_render.fillCircle(FLAPPY_BIRD_START_X, fb_bird_y(), FLAPPY_BIRD_RADIUS, WHITE);

    if (flappy_bird.state == PAUSED || flappy_bird.state == DEAD) {
        view_render.fillRect(7, 14, 114, 43, BLACK);
        view_render.drawRect(7, 14, 114, 43, WHITE);
        text(37, 18, flappy_bird.state == PAUSED ? "PAUSED" : "GAME OVER");
        text(14, 31, flappy_bird.state == PAUSED ? "MODE: RESUME" : "UP: RETRY");
        text(14, 44, "DOWN: RESET");
    }
}

view_dynamic_t dyn_view_flappy = {{ .item_type = ITEM_TYPE_DYNAMIC }, view_scr_flappy};
view_screen_t scr_flappy = {&dyn_view_flappy, ITEM_NULL, ITEM_NULL, .focus_item = 0};

void scr_flappy_handle(ak_msg_t *msg) {
    switch (msg->sig) {
    case SCREEN_ENTRY:
        task_post_pure_msg(FB_GAME_ID, FB_GAME_SETUP);
        timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_FLAPPY_TICK, 50, TIMER_PERIODIC);
        break;
    case SCREEN_EXIT:
        timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_FLAPPY_TICK);
        break;
    case AC_DISPLAY_FLAPPY_TICK:
        task_post_pure_msg(FB_GAME_ID, FB_GAME_TICK);
        break;
    case AC_DISPLAY_BUTTON_UP_PRESSED:
        task_post_pure_msg(FB_GAME_ID, FB_GAME_FLAP);
        break;
    case AC_DISPLAY_BUTTON_MODE_PRESSED:
        task_post_pure_msg(FB_GAME_ID, FB_GAME_TOGGLE_PAUSE);
        break;
    case AC_DISPLAY_BUTTON_DOWN_PRESSED:
        task_post_pure_msg(FB_GAME_ID, FB_GAME_SETUP);
        break;
    default:
        break;
    }
    // Button callbacks arm the idle timeout; keep gameplay on this screen.
    timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
}
