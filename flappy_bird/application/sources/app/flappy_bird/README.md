# Event-driven Flappy Bird

Tasks communicate through AK message queues (`task_post_pure_msg`). No task
calls another task's handler. Game input is sent to `FB_GAME_ID`.

| Input | Screen event | Game command | Bird event |
| --- | --- | --- | --- |
| Entry / DOWN | `SCREEN_ENTRY` / DOWN pressed | `FB_GAME_SETUP` | `FB_BIRD_INIT` (also `FB_PIPE_INIT`) |
| UP | UP pressed | `FB_GAME_FLAP` | `FB_BIRD_FLAP` |
| MODE | MODE pressed | `FB_GAME_TOGGLE_PAUSE` | `FB_BIRD_TOGGLE_PAUSE` |
| Timer (50 ms) | `AC_DISPLAY_FLAPPY_TICK` | `FB_GAME_TICK` | `FB_BIRD_TICK` |

`fb_game_handle` routes commands and coordinates reset/restart. It does not
read the bird's state. The bird task owns READY / FLYING / PAUSED / DEAD:

| State | Flap | Toggle pause | Tick | Collision |
| --- | --- | --- | --- | --- |
| READY | Start flying | Ignore | Ignore | Ignore |
| FLYING | Apply flap velocity | PAUSED | Update physics, post pipe update | DEAD |
| PAUSED | Ignore | FLYING | Ignore | Ignore |
| DEAD | Request `FB_GAME_RESTART` | Ignore | Ignore | Ignore |

Restart queues bird init, pipe init, then flap, preserving the reset order.
Each flying tick follows:

`FB_GAME_TICK → FB_BIRD_TICK → FB_PIPE_UPDATE → FB_COLLISION_CHECK`

Collision reads model positions without changing them. A hit posts
`FB_BIRD_COLLISION`; the bird task changes its own state. Pipe only moves on
`FB_PIPE_UPDATE`; it does not read bird state to decide when to run.

Game priority is 5, Bird/Pipe priority is 6 and Collision priority is 7. The
worker chain finishes before the next queued game input. Bird/Pipe commands
share a FIFO priority queue, preserving reset-before-flap ordering.

The screen handler only converts UI events to commands and manages its timer.
The render callback reads models to draw the screen. Timer is removed on
`SCREEN_EXIT`. Scoring and game-over audio are not implemented.

Test handlers with `python3 application/tests/run_flappy_events_test.py`.
The test queue models the worker priorities and FIFO ordering. It does not
run the AK scheduler or hardware. The desktop preview below uses a legacy API.

---

## Previous class-based implementation (historical)

## Flappy Bird

The game runs on the STM32L151 kit's 128×64 OLED and opens automatically after the startup logo.

### Game rules

Guide the bird through the gaps between pipe pairs to achieve the highest score.

| Button | Ready | Playing | Paused | GameOver |
| --- | --- | --- | --- | --- |
| **UP** | Start and flap | Flap | Ignored | Reset the round, start, and flap |
| **MODE** | Ignored | Pause | Resume | Ignored |
| **DOWN** | Reset the ready screen | Reset to the ready screen | Reset to the ready screen | Reset to the ready screen |

- The bird stays at horizontal position `x = 28`, has a radius of 3 pixels,
  and starts at `y = 32`.
- Gravity makes the bird fall on each 50 ms tick. UP resets its upward velocity;
  holding the button does not generate repeated flap events.
- There are 3 pipe pairs. Each pipe is 12 pixels wide, and each gap is 25 pixels
  high. Pipe pairs are spaced 58 pixels apart and move left by 2 pixels per tick
  during gameplay.
- Gap positions are pseudorandom. Pipes that leave the screen are repositioned
  after the rightmost pipe pair.
- Each pipe pair awards **1 point**, exactly once, when its right edge passes
  the bird's left edge. No points are awarded if a collision occurs on the same tick.
- Hitting a pipe, crossing the top edge of the screen, or touching the ground at
  `y = 62` ends the round. Collision detection uses the bird's bounding box.
  The game enters `GameOver`, the bird enters `Dead`, and the buzzer plays 3 beeps.
- Pausing preserves the bird's position, pipe positions, and score. The timer
  continues to send ticks, but movement is not updated in `Paused` or `GameOver`.
- Resetting clears the current score and recreates the pipes. The `best` score
  remains in RAM across retries and is lost when the board resets or powers off.

### Sequence diagram

Mermaid source: [sequence_diagram.mmd](sequence_diagram.mmd).

Firmware follows the Archery example: each module exposes a
`fb_*_handle(ak_msg_t*)` handler and is registered in `task_list`.
The screen posts pure messages instead of updating game logic directly.

| Task | Handler | Responsibility |
| --- | --- | --- |
| `FB_GAME_ID` | `fb_game_handle` | Setup, flap, pause/resume, start each tick |
| `FB_BIRD_ID` | `fb_bird_handle` | Update bird physics |
| `FB_PIPE_ID` | `fb_pipe_handle` | Move and recycle pipes |
| `FB_COLLISION_ID` | `fb_collision_handle` | Check collision, score, play game-over sound |

All four tasks run at priority 5, above the display task at priority 4.
Each phase posts the next phase so physics, pipes, and collision finish in
order before the display handles another button or timer message. Rendering
only reads the shared game state. The portable C++ classes retain the same
logic for desktop tests; the simulator does not emulate the AK scheduler.

```mermaid
sequenceDiagram
    participant Screen as Display / scr_flappy_handle
    participant Game as FB_GAME_ID
    participant Bird as FB_BIRD_ID
    participant Pipe as FB_PIPE_ID
    participant Collision as FB_COLLISION_ID
    Screen->>Game: FB_GAME_SETUP / FLAP / TOGGLE_PAUSE
    loop Every 50 ms
        Screen->>Game: FB_GAME_TICK
        Game->>Bird: FB_BIRD_UPDATE (when Playing)
        Bird->>Pipe: FB_PIPE_UPDATE
        Pipe->>Collision: FB_COLLISION_CHECK
        Collision->>Collision: Check collision and score; beep on game over
    end
```

`scr_flappy_handle` removes the demo `AC_DISPLAY_SHOW_IDLE` timer so the game
stays on screen. Ready, Paused, and GameOver states do not advance physics.

### Bird events

| BirdEvent | Accepted states | Result |
| --- | --- | --- |
| `Reset` | Any state | Enter `Ready`, y = 32, velocity = 0 |
| `Flap` | `Ready`, `Flying` | Enter `Flying`, velocity = −280 units |
| `Tick` | `Flying` | Increase velocity by 40 units, cap falling velocity at 400, update position |
| `Pause` | `Flying` | Enter `Paused` |
| `Resume` | `Paused` | Enter `Flying` |
| `Collision` | `Flying` | Enter `Dead`, velocity = 0 |

Events received in unsupported states are ignored. Physics uses integers with
100 units = 1 pixel; each `Tick` represents 50 ms.

### Build and test

Build from `application` with `make -j4`. The firmware is generated at
`build_ak-base-kit-stm32l151-application/ak-base-kit-stm32l151-application.bin`.
Flash using the existing `make flash dev=/dev/ttyUSB0` target (adjust the port).

Run the game logic checks from `application`:

```sh
g++ -std=c++11 -Wall -Wextra -pedantic tests/flappy_game_test.cpp sources/app/flappy_bird/fb_game.cpp sources/app/flappy_bird/fb_bird.cpp -o /tmp/flappy-game-test
/tmp/flappy-game-test
```

### Playable desktop preview

From `application/tests`, run `python3 play_flappy.py` to play using the same
C++ game logic. Requires g++, Python 3, Tkinter, and a graphical desktop.
Use Up/Space to flap, P/Enter to pause, R/Down to reset, and Esc to quit.
See [the tests README](../../../tests/README.md) for setup and test commands.
