# Analiza funkcjonalna - Sieciowy PacMan

Piotr Suchy 310310

## Opis ogólny

Projekt zakłada stworzenie gry wzorowanej na klasycznym PacManie, z dodatkową funkcjonalnością rozgrywki wieloosobowej na tej samej planszy, zbierając punkty i unikając przeciwników.

## Podstawowa funkcjonalność

Istnieją dwa tryby gry:

1. Tryb jednoosobowy
    - klasyczna rozgrywka na wzór PacMana
    - system punktacji
    - zbieranie kulek (pellets) i power-upów
    - interakcja ze ścianami i elementami planszy
2. Tryb wieloosobowy
    - możliwość hostowania gry (serwer)
    - dołączanie do istniejących gier (klient)
    - synchronizacja stanu gry między graczami
    - współdzielona plansza, zasoby i punktacja

## Mechanika gry

1. Poruszanie się
    - sterowanie za pomocą strzałek
    - kolizje ze ścianami
    - płynny ruch w korytarzach
    - przechodzenie przez tunele
2. System punktacji
    - użytkownik otrzymuje punkty za zbieranie kulek (np. 10 pkt)
    - bonusowe punkty za power-pellets (wyróżniające się wyglądem), np. 50 pkt
    - tablica wyników dla wszystkich graczy
3. "Power-upy"
    - czasowe wzmocnienia - "pożeranie" duchów + dodatkowe punkty podczas wzmocnienia
    - specjalne efekty wizualne
    - odporność na 'kolizję' z duchem

## Architektura systemu

### Komponenty główne

Główna logika gry będzie podzielona na (plan, który może się zmienić):

1. Warstwa logiki gry:

    - klasa `Game` zarządzająca stanem gry
    - system podzielony na `Entity`, `Player`, `Wall`, `Pellet`, `Ghost` etc.
    - zarządzanie kolizjami i punktacją

2. Warstwa sieciowa

    - NetworkManager obsługujący komunikację
    - synchronizacja stanu gry między klientami, a serwerem
    - obsługa połączeń TCP

3. Warstwa graficzna

    - interfejs graficzny (SFML)
    - renderowanie objektów gry
    - wyświetlanie punktacji i stanu

## Zamysł na wykorzystanie wzorców i technik

Dziedziczenie i polimorfizm zostanie wykorzystany w hierarchicznej klasie Entity. Szablony wykorzystane zostaną przy klasie Grid\<T\> do zarządzania planszą. Wzorzec programistyczny singleton wykorzystany do klasy `NetworkManager`. Reszta aplikacji wykorzystująca architekturę klient-serwer do obsługi wielu graczy.

## Środowisko programistyczne

- MacOS - Sequoia 15.3
- Kompilator - AppleClang 16.0.0
- System budowania: CMake 3.10+
- Biblioteki:
  - SFML (grafika i sieć)
  - STL (kontenery i algorytmy)
