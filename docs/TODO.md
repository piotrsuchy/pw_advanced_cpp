# Missing Features for Classic Pacman Experience

## What's Implemented

| Feature | Status |
| --- | --- |
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
| Main menu, READY!, pause, game over / level win + retry | Working |
| HUD lives + scores | Working (text) |
| Tunnel wrapping (left/right edge wrap) | Working |
| Ghosts slower in side tunnel (horizontal wrap row) | Working |
| Scatter corners + chase/scatter reversal + Blinky Cruise Elroy | Working |
| Power pellet end flash (ghosts blue/white in last 3s) | Working |

---

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

### Medium Priority (Polish)

1. Power pellet flash warning
2. Ghost eyes returning to house
3. Sound effects
4. Main menu / pause

## DONE

- [x] **Match / `MatchPhase`** — `Match` holds flow (`Waiting`, `Playing`, `LevelComplete`, `GameOver`); `GameServer` calls `updateAfterStep` after each tick. `Simulation` exposes `isMatchLost()` / `isLevelCleared()`; win/lose flags no longer live on `Simulation`
- [x] **PowerUps / `ICollectible`** — Collectibles (pellet, power pellet, cherry) are described by `ICollectible` with points, power duration, level-clear counting, and network clear tags; `Simulation` no longer branches on `TileType` for scoring
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
- [x] **Menus & match flow (client + server)** — Main menu (Play / Options with server address / Quit); "READY!" countdown after `LEVEL` sync; **P** or **Esc** toggles **authoritative pause** (server stops stepping); `SNAPSHOT` includes pause + match outcome; **Game over** and **level complete** overlays with **R** (restart) and **M** (main menu, disconnect); server no longer `exit(0)` on match end; `RESTART` runs `Simulation::resetForNewMatch(1)` and re-broadcasts `LEVEL`
- [x] **Lives Display** - Lives per player in HUD (icons optional / polish)
- [x] **Ready/Start Screen** - "READY!" text before level starts
- [x] **Game Over Screen** - Victory/defeat screen with retry option
- [x] **Main Menu** - Start screen, options
- [x] **Pause Menu** - Ability to pause mid-game
- [x] **Ghost Speed in Tunnels** — `LevelManager` picks the longest open-end row as the wrap tunnel; ghosts use 50% speed there when moving left/right (stacks with frightened 60%)
- [x] **Scatter corners & chase↔scatter reverse** — Already implemented via per-ghost `getScatterTile()` and `updateGhostMode()` reversing roaming non-frightened ghosts
- [x] **Cruise Elroy (Blinky)** — `Simulation` passes `cruiseElroyChaseSpeedMul` into `Ghost::updateLogic` for slot 0 only in chase when not frightened; tiers at 20 and 10 remaining pellets
- [x] **Power pellet flash warning** — `SNAPSHOT` sends `powerTimeLeft` per player; client uses `max(P0,P1)` in the last **3s** to toggle `GhostRenderer::setFrightenedEndFlash` and alternate `ghosts_power_pellet_1/2` textures (~2.5 Hz); non-textured fallback toggles blue/white
- [x] **Server starts the game** and on connect the client can move after ready screen, but the game has already started and ghosts moved in already - the game should start after client joins / both clients join.
