# Analiza funkcjonalna - Sieciowy PacMan

Piotr Suchy 310310

Podchodzę drugi raz do kursu `Zaawansowane C++`, w poprzednim roku również tworzyłem ten projekt, ale nie udało mi się go ukończyć na czas.

## Opis ogólny

Projekt zakłada stworzenie gry wzorowanej na klasycznym PacManie, z dodatkową funkcjonalnością rozgrywki wieloosobowej na tej samej planszy, zbierając punkty i unikając przeciwników. Dodatkowo, w grze pojawią się nowe efekty związane z różnymi typami power-upów. Zamierzam również zaimplementować dokładnie działające algorytmy poruszania się duchów, które wpłyną na zwiększenie trudności gry. Potencjalnie wykorzystam klasyczne algorytmy, lub zaimplementuję własne.

## Podstawowa funkcjonalność

Istnieją dwa tryby gry:

1. Tryb jednoosobowy
    - klasyczna rozgrywka na wzór PacMana
    - system punktacji
    - zbieranie kulek (pellets) i power-upów
    - interakcja ze ścianami i elementami planszy
2. Tryb wieloosobowy
    - możliwość stworzenia gry, do której dołączają inni gracze
    - dołączanie do istniejących gier jako gracz
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
    - bonusowe punkty za power-pellets (wyróżniające się wyglądem), np. 50 pkt
    - tablica wyników dla wszystkich graczy
    - koniec gry w przypadku kolizji z duchem

3. “Power-upy”
    - czasowe wzmocnienia - “pożeranie” duchów + dodatkowe punkty podczas wzmocnienia
    - specjalne efekty wizualne
    - odporność na ‘kolizję’ z duchem
    - inny pomysł: przyspieszenie rozgrywki
    - inny pomysł: zmiana architektury mapy po zebraniu innego pelletu typu 'power-up'

## Komponenty główne

Główna logika gry będzie podzielona na warstwę logiki gry, warstwę sieciową i warstwę graficzną.

## Interfejs graficzny

Klasyczny interfejs graficzny PacMana:

![Klasyczny interfejs graficzny PacMana](images/report_1_gui.png)

Po wstępnej wyszukaniu Sprite'ów do gry, prawdopodobnie użyte zostaną:

![Zrzut ekranu przygotowanych sprite'ów do duchów](images/ghosts.png)

![Zrzut ekranu przygotowanych sprite'ów do planszy i pellet'ów](images/map_pellets.png)

![Zrzut ekranu przygotowanych sprite'ów do PacMana](images/pacman.png)

\clearpage

## Zamysł na wykorzystanie wzorców i technik

Dziedziczenie i polimorfizm zostanie wykorzystany w hierarchicznej klasie Entity.
Szablony wykorzystane zostaną przy klasie `Grid<T>` do zarządzania planszą.
Wzorzec programistyczny singleton wykorzystany do klasy NetworkManager.
Reszta aplikacji wykorzystująca architekturę klient-serwer do obsługi wielu
graczy.

## Zachowane elementy PacMana

Chciałbym, żeby interfejs graficzny pozostał jak najbardziej zbliżony do klasycznego PacMana, ale sama rozgrywka będzie miała dodatkowe funkcjonalności, takie jak tryb wieloosobowy oraz nowe efekty związane z różnymi typami power-upów.

## Środowisko programistyczne

### Lokalnie

- MacOS - Sequoia 15.3
- Kompilator - AppleClang 16.0.0
- System budowania: CMake 3.10+
- Biblioteki
  - SFML (grafika i sieć)
  - STL (kontenery i algorytmy)

### Do sprawdzania części sieciowej

Maszyna wirtualna z Ubuntu 24.04 LTS hostowana na Linode jako serwer przetrzymujący stan danej rozgrywki. Połączenie się z dwóch różnych komputerów (jako klienci) do maszyny wirtualnej (serwer).

\clearpage

## Etap II – diagramy klasy z opisem ról

Zgodnie z wymaganiami projektowymi i wbudowanym wzorem architektonicznym, główna logika gry została w 100% oddzielona od interfejsu graficznego. Stworzone klasy grupują się na trzy warstwy: implementujące rdzeń decyzyjny **Core/Shared**, powiązane zarządzaniem pętlami klienta i serwera **Network** oraz implementowany po stronie klienta wizualny system **UI/Graphics**.

Poniżej znajdują się zrzuty wygenerowanych schematów (podzielone na poszczególne pakiety dla czytelności) razem z omówieniem.

### Całkowity pogląd na klasy

![Pełny diagram klas projektu](images/uml_diagram_klas.png)

### Warstwa Logiki (Core/Shared)

![Warstwa logiki - diagram](images/uml_logika.png)

Klasy te są całkowicie niezależne od interfejsu graficznego. Odpowiadają za zasady gry oraz stan symulacji:

- **`Entity`** – Abstrakcyjna klasa bazowa dla wszystkich obiektów na planszy. Definiuje pozycję i polimorficzną metodę `update()`.
- **`PacMan`** – Klasa gracza. Przechowuje kierunek ruchu i liczbę punktów. Reaguje na polecenia i zdarzenia na planszy.
- **`GhostBase`** – Abstrakcyjna klasa duchów. Definiuje wspólne stany (np. przerażenie) i wirtualną metodę `calculateTarget()`.
- **`Blinky`, `Pinky`, `Inky`, `Clyde`** – Klasy dziedziczące po `GhostBase`. Implementują unikalne, polimorficzne algorytmy śledzenia gracza.
- **`PowerUp`** – Klasa dziedzicząca po `Entity`, reprezentująca czasowe wzmocnienia do zebrania.
- **`Grid<T>`** – Szablonowa struktura danych do zarządzania siatką komórek labiryntu (2D).
- **`LevelManager`** – Zarządza stanem poziomu bazując na obiekcie `Grid`. Odpowiada za detekcję kolizji i monitoruje zebrane kulki.
- **`Simulation`** – Maszyna stanu i centralna autorytatywna pętla logiki gry, aktualizująca wektory bytów co klatkę.

### Warstwa Sieciowa (Network)

![Warstwa sieciowa - diagram](images/uml_sieciowa.png)

Pakiety klas realizujące architekturę opartą na relacji klient-serwer.

- **`NetworkManager`** – Klasa (wzorzec Singleton) oferująca globalkowy interfejs wysyłania i odbierania pakietów przez SFML Network.
- **`Server`** – Autorytatywny kod serwera, uruchamiany bez grafiki. Utrzymuje główną instancję `Simulation` i propaguje stan gry do połączonych klientów.
- **`Client`** – Klient grającej aplikacji powielający u siebie odbijany od serwerów widok. Przesyła na serwer jedynie wciskane klawisze.
- **`GameState`** – Obiekt strumieni DTO mapujący informacje o graczach, statusie gry i planszy do wektora bajtów ułatwiającego serializację pakietów.

### Warstwa Widoku i Użytkownika (UI/Graphics)

![Warstwa grafiki - diagram](images/uml_interfejs_grafika.png)

Struktury reagujące na stan gry przysłany od serwera, nie wpływające same na zasady rozgrywki i mechanikę.

- **`GameController`** – Inicjalizuje okno klienta i steruje przepływem całego programu graficznego (obsługa renderów, stany i podprocesy).
- **`InputManager`** – Tłumaczy sprzętowe naciśnięcia klawiatury użytkownika na zrzeszony, logiczny model `InputData`.
- **`Renderery (LevelRenderer, PacmanRenderer, GhostRenderer)`** – Niezależne obiekty odpowiedzialne za wyświetlanie grafik (sprite'y). Odczytują pozycje elementów (takich jak `PacMan` czy duchy) i bezpiecznie rysują je w oknie.
