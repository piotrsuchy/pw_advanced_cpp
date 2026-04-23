# Sieciowy PacMan — raport końcowy (Etap III)

Piotr Suchy 310310 — Zaawansowane C++

## Opis projektu

Projekt to sieciowa implementacja gry PacMan w C++17 z architekturą klient–serwer opartą o SFML (grafika, audio, sieć TCP). Serwer jest autorytatywny: utrzymuje pełną symulację rozgrywki (plansza, gracze, duchy, kolekty, timery trybów scatter/chase/frightened, zdarzenia tikowe) i rozsyła snapshoty do maksymalnie dwóch klientów. Klient renderuje tylko stan otrzymany z serwera i wysyła wciskane kierunki. Obsługiwane są między innymi: czterech duchów z różnymi AI (Blinky, Pinky, Inky, Clyde), tryb frightened z mnożnikiem punktów (100/200/400/800/1600), bonusowe owoce spawnujące się przy progach 70/170 zebranych kulek, tunel boczny z wrappingiem, animacja śmierci, HUD, menu główne, ekrany READY/PAUSE/GAME OVER/LEVEL WIN oraz warstwa audio (efekty, pacman theme, syrena trybu 'frightened', podkład muzyczny).

**Repozytorium**: [github.com/piotrsuchy/pw_advanced_c++](https://github.com/piotrsuchy/pw_advanced_cpp)

## Dokumentacja i diagram klas

Pełna dokumentacja projektu znajduje się w folderze `docs/` w archiwum `.zip`:

- `docs/class_diagram.puml` — aktualny, ręcznie utrzymywany diagram klas (PlantUML).
- `docs/images/class_diagram.png` oraz `docs/images/class_diagram.svg` — wyrenderowana wersja graficzna tego diagramu.
- `docs/doxygen/html/index.html` — pełna dokumentacja API wygenerowana z komentarzy Doxygen (niedostępna w .zip, do wygenerowania na podstawie Doxyfile)

Diagram klas wiernie odzwierciedla bieżący stan kodu (`include/`, `src/`). Klasy rozłożone są na trzy logiczne pakiety:

- **Core / Shared** — `Simulation` (autorytatywna pętla), `Match` (faza rozgrywki), `Player`, `PacmanLogic`, `LevelManager` + `TileType`, `ICollectible` (interfejs dla pelletów, power‑pelletów i owoców bonusowych), `Ghost` + strategie `BlinkyAI`/`PinkyAI`/`InkyAI`/`ClydeAI` (interfejs `IGhostAI`), `InteractionResolver` (rozstrzyga kolizje pacman–duch), `EventQueue<T>` oraz `PlayerStateView`, `ConsumedPellet`, `EatenGhostEvent`, `GridTileUpdate`, `GhostEatenEvent`.
- **Server** — `GameServer` (akceptacja klientów, stała częstotliwość tików, budowanie i broadcast pakietów `level` i `snapshot`).
- **Client** — `GameClient` (pętla okna, UI, sieć), `InputManager`, `AudioManager`, `LevelRenderer`, `PacmanRenderer`, `GhostRenderer`, enumy `GhostSlot`/`ClientFlow`/`SubMenu`.

## Scenariusze uruchomienia

**Scenariusz 1 — lokalnie (localhost).** Uruchomienie jednym poleceniem startuje serwer i automatycznie dwóch klientów na tej samej maszynie:

```bash
./rebuild_and_run.sh localhost
```

**Scenariusz 2 — serwer na VM + klient na laptopie.** Na maszynie wirtualnej (Ubuntu 24.04, publiczny adres `<VM_IP>`) raz instalujemy zależności, a następnie uruchamiamy serwer:

```bash
./scripts/install_deps_ubuntu.sh
./rebuild_and_run.sh server --port 54000
```

Na laptopie (lub drugim komputerze) uruchamiamy klienta wskazującego na VM:

```bash
./rebuild_and_run.sh client --ip <VM_IP> --port 54000
```

Dwa klienty łączące się do tego samego serwera dołączą do tej samej partii (slot 0 i 1). Serwer utrzymuje stan gry i rozsyła snapshoty oboma klientom.

## Wymagania i krótka instrukcja

- macOS lub Linux, kompilator z obsługą C++17 (AppleClang 16 / clang / g++), CMake co najmniej 3.10.
- SFML (grafika, audio, sieć). Na macOS: `brew install sfml cmake`. Na Ubuntu: `./scripts/install_deps_ubuntu.sh`.
- Build i uruchomienie: `./rebuild_and_run.sh localhost` — skrypt wykonuje konfigurację CMake, build, opcjonalnie `clang-format`/`clang-tidy`, a następnie uruchamia serwer i klientów.
- Sterowanie: strzałki (ruch), ENTER (READY / potwierdzenie w menu), ESC (pauza / powrót do menu).
- Kod źródłowy w archiwum `.zip` nie zawiera wyników kompilacji (brak `build/`, `bin/`, `obj/`); pełny kod jest także dostępny w repozytorium na github: [pw-zaawansowany-C++](https://github.com/piotrsuchy/pw_advanced_cpp)
