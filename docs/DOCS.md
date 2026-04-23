# Dokumentacja projektu Sieciowy PacMan

Dokumentacja składa się z:

- główne README.md w root projektu
- folder docs/
  - zawierający folder `images/`, czyli zdjęcia użyte do wygenerowania raportu
  - zawierający folder `reports/`, czyli wygenerowane raporty, w `old_version/` - z pierwszego terminu, w `new_version/` - z drugiego terminu
  - zawierający plik `class_diagram.puml`, czyli utrzymywany ręcznie aktualny diagram klas projektu
- REPORT.md na podstawie którego przyrostowo generowana jest dokumentacja

## Zaimplementowane

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
| Dynamic bonus fruit (70 / 170 pellets, random 100/200/400/800, fruit-1…4.png, score popups) | Working |
| Client/server multiplayer architecture | Working |
| Main menu, READY!, pause, game over / level win + retry | Working |
| HUD lives + scores | Working (text) |
| Tunnel wrapping (left/right edge wrap) | Working |
| Ghosts slower in side tunnel (horizontal wrap row) | Working |
| Scatter corners + chase/scatter reversal + Blinky Cruise Elroy | Working |
| Power pellet end flash (ghosts blue/white in last 3s) | Working |
| Audio (SFX + intro jingle, theme, frightened siren) | Working |
