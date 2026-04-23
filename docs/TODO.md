# Missing Features for Classic Pacman Experience

## What's Implemented

| Feature | Status |
|---------|--------|
| Basic movement & collision | Working |
| 4 ghosts with distinct AI (Blinky, Pinky, Inky, Clyde) | Working |
| Lives system (3 lives) | Working |
| Score tracking | Working |
| Power pellets → frightened mode (10s) | Working |
| Ghost multiplier scoring (100→200→400→800→1600) | Working |
| Death animation (8 frames) | Working |
| Pacman mouth animation | Working |
| Ghost respawn from house | Working |
| Cherries (2 fixed positions, 100 pts) | Working |
| Client/server multiplayer architecture | Working |
| Tunnel wrapping (left/right edge wrap) | Working |

---

### Architecture Tasks (from reviewer feedback)

- [ ] **PowerUps ownership** — Power pellets are still a raw `TileType` in `LevelManager`; needs an `ICollectible` abstraction
- [ ] **Match / GameSession** — Extract game-phase state (waiting, playing, game-over, level-complete) from `Simulation` into a `Match` class

### Core Gameplay

- [ ] **Ghosts should blink from blue to white** for the last few seconds of the being frightened.
- [ ] **Level Progression** - Multiple levels with increasing difficulty (faster ghosts, shorter frightened time)
- [ ] **Ghost Speed in Tunnels** - Ghosts slow down in tunnels

### Ghost Behavior

- [ ] **Scatter Mode Corners** - Each ghost retreats to their "home corner" during scatter phases
- [ ] **Direction Reversal on Mode Change** - Ghosts immediately reverse when switching chase↔scatter
- [ ] **Elroy Mode** - Blinky speeds up when few pellets remain

### Visual/UI

- [ ] **Lives Display** - Visual icons showing remaining lives
- [ ] **Ready/Start Screen** - "READY!" text before level starts
- [ ] **Game Over Screen** - Victory/defeat screen with retry option
- [ ] **Main Menu** - Start screen, options
- [ ] **Pause Menu** - Ability to pause mid-game
- [ ] **Power Pellet Flash Warning** - Ghosts blink white/blue as frightened time runs out
- [ ] **Level Number Display** - "Level X" indicator

### Audio

- [ ] **Sound Effects** - Waka-waka, ghost eaten, death, power pellet activation, siren
- [ ] **Background Music** - Intro jingle, gameplay siren

### Bonus Items

- [ ] **Dynamic Fruit Spawning** - Fruit appears twice per level at specific pellet counts
- [ ] **Fruit Variety** - Cherry → Strawberry → Orange → Apple → Melon → Galaxian → Bell → Key

### Polish

- [ ] **Intermission Cutscenes** - Short animations between certain levels
- [ ] **Ghost "Eyes" Return** - After being eaten, ghost eyes travel back to house
- [ ] **Pre-game Ghost Bounce** - Ghosts bounce in house before release
- [ ] **High Score Tracking** - Persistent best score

---

## Suggested Priority Order

### High Priority (Core Experience)

1. ~~Level completion detection (win condition)~~ ✅
2. ~~Tunnel wrapping~~ ✅
3. ~~Scatter/chase mode cycling~~ ✅
4. Lives display in HUD
5. Game over / victory screen

### Medium Priority (Polish)

1. Ready screen before level start
2. Power pellet flash warning
3. Ghost eyes returning to house
4. Sound effects
5. Main menu / pause

## DONE

- [x] **Refactor `main_client.cpp` / `main_server.cpp`** — extracted into `GameClient` and `Server` classes; `main_*` files are now thin arg-parsing wrappers
- [x] **Separate interface from implementation** — `IEntity` and `IGhostAI` are now pure-virtual interfaces; concrete classes implement them
- [x] **Use polymorphism in Simulation** — `Simulation` uses `std::array<unique_ptr<Ghost>, 4>` with AI injected via strategy pattern
- [x] **Player representation** — `Player.hpp` encapsulates all per-player state (score, lives, timers, spawn, `PacmanLogic`)
- [x] **AI algorithm separation** — Ghost targeting split into `BlinkyAI`, `PinkyAI`, `InkyAI`, `ClydeAI` strategy classes
- [x] **Interaction/conflict resolution** — `InteractionResolver` handles all lethal and frightened collisions
- [x] **Ghosts render blue (frightened) after being eaten** — fixed: server now sends per-ghost `active` + `frightened` flags in SNAPSHOT; client uses them instead of inferring from player power state
- [x] **Scatter/Chase Mode Cycling** - Classic ghosts alternate between chase and scatter phases on a timer
        (7s scatter → 20s chase → 7s scatter → 20s chase → 5s scatter → 20s chase → 5s scatter → ∞ chase;
        ghosts reverse direction on each transition; frightened state overrides)
- [x] **Tunnel Wrapping** - Pac-Man and ghosts wrap horizontally through left/right tunnels
- [x] **Level Completion** - Detection for "all pellets eaten" → win condition