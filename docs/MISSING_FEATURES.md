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

---

## Missing Features

### Core Gameplay
- [ ] **Scatter/Chase Mode Cycling** - Classic ghosts alternate between chase and scatter phases on a timer
- [ ] **Tunnel Wrapping** - Tunnels on left/right edges where Pacman/ghosts wrap around
- [x] **Level Completion** - Detection for "all pellets eaten" → win condition
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
1. Level completion detection (win condition)
2. Tunnel wrapping
3. Scatter/chase mode cycling
4. Lives display in HUD
5. Game over / victory screen

### Medium Priority (Polish)
6. Ready screen before level start
7. Power pellet flash warning
8. Ghost eyes returning to house
9. Sound effects
10. Main menu / pause

### Lower Priority (Full Authenticity)
11. Multiple levels with difficulty scaling
12. Dynamic fruit spawning
13. Elroy mode
14. High score persistence
15. Intermissions
