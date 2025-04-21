# Pac-Man–Style Game Development Checklist

## 1. Setup & Project Initialization

### Decide on Game Framework / Library

- Select environment: Unity, Unreal, SDL, SFML, OpenGL, etc. 🟩
- Configure development tools (IDE, build system). 🟩

### Create Basic Project Structure

Organize folders/modules for:

- Core game loop / engine code 🟩
- Assets (images, sounds) 🟧 $TODO
- Scripts or code for entities (Pac-Man, ghosts) 🟧 $TODO
- UI and menus (if applicable) 🟧 $TODO

### Implement a Game Loop

Tasks:

- Create `GameController` (or main loop class) with `update(float deltaTime)` and `render()` methods. 🟩
- Clear the screen, draw something basic (e.g., background or placeholder sprite), and present the frame each cycle. 🟩

---

## 2. Input Management

### Implement `InputManager`

Tasks:

- Detect keyboard/joystick events. 🟩
- Provide method like `getDirection()` to query direction input. 🟩
- Test by logging inputs (up, down, left, right). 🟩

### Hook Input to `GameController`

Tasks:

- Call `InputManager.pollInputs()` during the update phase. 🟩
- Pass direction commands to a test sprite or player object. 🟩

---

## 3. Maze & Level Layout

### Create a Maze or TileMap Class

Tasks:

- Use a 2D array or tile-based system. 🟩
- Store data: walls, paths, pellets. 🟩
- Provide collision checks like `isWall(TilePosition pos)`. 🟩

### Load or Hardcode a Level

Tasks:

- Use a text/JSON file or hardcoded 2D array (e.g., 28×31 grid). 🟩
- Implement `loadLevel(int levelNumber)` in `LevelManager`. 🟩
- Render maze (walls, floor). 🟩

### Place Pellets and Power Pellets

Tasks:

- Mark tiles for pellets / power pellets. 🟩
- Render them with sprites (small/large dots). 🟩
- Implement `collectPellet(TilePosition pos)`. 🟩

---

## 4. Entity System: Pac-Man & Ghosts

### Create Base `Entity` Class

Tasks:

- Fields: position, direction, speed, sprite. 🟩
- Methods: `update(float deltaTime)`, `draw()`, `move(direction)`. 🟩

### Implement `PacMan` Class

Tasks:

- Extend `Entity`, handle input. 🟩
- Check for collisions. 🟩
- Track lives. $TODO

### Test Pac-Man Movement

Tasks:

- Hook `PacMan.handleInput(input)` to `InputManager`. 🟩
- Use `Maze.isWall(pos)` for movement restriction. 🟩
- Render position each frame. 🟩

### Create `Ghost` Class

Tasks:

- Extend `Entity`.
- Fields: AI state, frightened timer, home corner.
- Method: `updateAI(float deltaTime)`.

### Ghost AI Strategies (Optional)

Tasks:

- Implement different ghosts (Blinky, Pinky, etc.).
- Use `getNextMove()` based on AI state and Pac-Man's position.
- Start with random movement or row/column logic.

---

## 5. Collision & Scoring Systems

### Implement `CollisionManager` (Optional)

Tasks:

- Detect collisions: Pac-Man vs. ghosts, Pac-Man vs. pellets.
- Define outcomes:
  - Normal ghost → lose a life.
  - Frightened ghost → add score, reset ghost.

### Scoring & `ScoreManager`

Tasks:

- `addScore(int points)` for total score.
- Pellet collision → `addScore(10)`.
- Ghost-eating → bonus points.

### Lives & `GameController`

Tasks:

- Decrement lives when caught.
- Trigger game over when lives = 0.

---

## 6. Power Pellets & Edible Ghost Mode

### Extend Collisions

Tasks:

- Detect power pellet consumption.
- Trigger `ghost.enterFrightenedMode()`.

### Frightened Mode Timer

Tasks:

- Add timer per ghost.
- Call `ghost.exitFrightenedMode()` after time expires.

### Ghost Reset After Being Eaten

Tasks:

- Respawn ghost at “ghost house”.
- Optional: delay, animation, or pause before reentering normal mode.

---

## 7. Level Completion & Transitions

### Check All Pellets Eaten

Tasks:

- `LevelManager.checkLevelCompletion()`.
- Move to next level or end game.

### Game Over / Restart

Tasks:

- Show “Game Over” or main menu.
- Optional: prompt for initials for high score.

---

## 8. Menus & UI (Optional / Enhancements)

### Main Menu Screen

Tasks:

- Show “Press Start” or “Play Game”.
- Options for level select or high scores.

### In-Game UI

Tasks:

- Show current score, high score, lives.
- Draw UI in corner or top bar.

### Pause / Resume

Tasks:

- Implement pause state.
- Only update pause menu logic.

---

## 9. Audio & Visual Polish

### Implement `AudioManager`

Tasks:

- Load sounds: wakka, ghost siren, power-up.
- Play sounds on events.

### Animations & Sprites

Tasks:

- Animate Pac-Man (mouth).
- Animate ghosts (legs, frightened color).
- Optional: Pac-Man death animation.

### Bonus Items / Fruit (Optional)

Tasks:

- Occasionally spawn fruit.
- Award bonus points on collection.

---

## 10. Testing & Tuning

### Test Collisions Extensively

Tasks:

- Check edge cases (same tile, tile boundaries).

### Adjust Speeds & Difficulty

Tasks:

- Tune Pac-Man vs. ghost speed.
- Tune frightened mode and timers.

### Playtest for Gameplay Feel

Tasks:

- Ensure gameplay is fair and fun.
- Adjust scoring, lives, and AI.

---

## 11. High Scores / Persistence (Optional)

### Create a Storage System

Tasks:

- Use file-based or DB storage.
- Record top scores.

### Implement `ScoreManager.getHighScore()`

Tasks:

- Load best score at game start.
- Save new high score after game over.

### High-Score Table

Tasks:

- Prompt for initials.
- Show table after game or in menu.

---

## 12. Final Integration & Cleanup

### Consolidate Code & Class Interactions

Tasks:

- Ensure `GameController` orchestrates all components.
- Keep classes single-responsibility.

### Optimize (If Needed)

Tasks:

- Fix performance issues, memory leaks.
- Optimize collision checks.

### Final Build or Deployment

Tasks:

- Package for platform (Windows, Mac, Web, etc.).
- Share or release.

---

## ✅ Checklist Summary

- [X] Project Setup – Game loop, framework
- [X] Input Management – Directions, hooking up controls
- [X] Maze / TileMap – Walls, pellets, rendering
- [.] Entities – Pac-Man, Ghosts, base Entity structure
- [ ] Collision & Scoring – CollisionManager, ScoreManager, lives system
- [ ] Power Pellets – Frightened ghost mode, timers
- [ ] Level Transitions – Win conditions, game over conditions
- [ ] UI & Menus – Score display, pause, main menu
- [ ] Audio & Visual – Sound effects, animations
- [ ] Testing & Tuning – Collision corner cases, speed balancing
- [ ] High Scores – Optional persistence
- [ ] Polish & Release – Final code cleanup, build, and deployment

## AIM

We are in the middle of rewriting one of the features, with the aim being:

- pacman keeps on going in one direction until some key is pressed or is stopped by a wall
- if a wall is in the direction in which the key is pressed it doesn't try to go there, but the last pressed direction is stored in case there is empty space and then it changes that direction (so let's say we are going right, we press up, but there's a wall. It keeps going in the same direction it was going, and then a few frames later there is no wall UP so we change the direction and go there
- we are in the center of a lane most of the time
- it's possible to go 'into' a wall and stop going in some direction ONLY if there was no additional input from the user and we run into the end of the 'lane'
