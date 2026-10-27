#ifndef FB_GAME_H
#define FB_GAME_H

#include "ak.h"
#include "message.h"

// Public commands: FB_GAME_SETUP, FB_GAME_FLAP, FB_GAME_TOGGLE_PAUSE,
// FB_GAME_TICK. Post them to FB_GAME_ID; never call another task directly.
void fb_game_handle(ak_msg_t *msg);

#endif
