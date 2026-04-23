# Missing / implemented features (concise)

Mirror of `docs/TODO.md` for high-level tracking. See **TODO** for the full list.

## Recently implemented

- [x] **Ghost speed in tunnels** — Side wrap row is derived from the maze (longest row with both map edges non-wall). Ghosts moving left/right on that row use 50% of their normal step speed (multiplies with frightened mode). Implementation: `LevelManager::recomputeDerivedFields()` + `isHorizontalGhostTunnelRow()`; `Ghost::updateLogic()` in `GhostLogic.cpp`.

- [x] **Scatter corners** — Each ghost uses `IGhostAI::getScatterTile()` in scatter mode (Blinky top-right, Pinky top-left, Inky bottom-right, Clyde bottom-left on the classic maze).

- [x] **Reverse on chase↔scatter** — `Simulation::updateGhostMode()` reverses roaming, non-frightened ghosts when the mode flips.

- [x] **Cruise Elroy (Blinky)** — In chase, Blinky (ghost slot 0) gets `cruiseElroyChaseSpeedMul` 1.08 when ≤20 pellets remain, 1.12 when ≤10; not applied in scatter or when frightened. Other ghosts always use 1.0.

- [x] **Power pellet flash warning** — Last 3s of power: frightened ghosts alternate blue/white textures (or circle colors); needs `powerTimeLeft` in `SNAPSHOT` from `PlayerStateView`.
