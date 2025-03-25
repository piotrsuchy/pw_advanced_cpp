# Diagram klas i opisy - Sieciowy PacMan

Piotr Suchy 310310

## Opis klas

### Game

Zarządza głównym cyklem gry – inicjalizacją, przetwarzaniem zdarzeń, aktualizacją stanu oraz renderowaniem. Koordynuje pracę całej aplikacji, delegując zadania do zarządzanego przez siebie systemu scen (SceneManager) oraz ładowania zasobów (ResourceManager).

### Scene (klasa abstrakcyjna)

Definiuje interfejs dla różnych stanów gry, takich jak menu, rozgrywka czy pauza. Umożliwia oddzielenie logiki specyficznej dla danego stanu od ogólnego cyklu gry, zapewniając metody do obsługi zdarzeń, aktualizacji i renderowania.

### InGameScene

Reprezentuje tryb rozgrywki. Implementuje mechanikę gry, zarządzając planszą (Maze), listą obiektów (entities), głównym graczem (Player), duchami (Ghost) oraz komunikacją sieciową (NetworkManager). To tutaj odbywa się większość operacji związanych z rozgrywką.

### Maze

Odpowiada za przechowywanie i zarządzanie układem planszy gry. Zawiera informacje o ścianach, pelletach i power-pelletach oraz udostępnia metody do sprawdzania, czy dany kafelek jest przeszkodą lub zawiera pellet. Ponadto umożliwia konwersję współrzędnych pomiędzy logiką gry a interfejsem graficznym.

### Entity (klasa abstrakcyjna)

Stanowi bazę dla wszystkich dynamicznych obiektów w grze. Definiuje podstawowe metody aktualizacji, renderowania oraz obsługi kolizji, a także przechowuje pozycję i prędkość. Klasy takie jak Player, Ghost i Pellet dziedziczą z tej klasy.

#### Player

Reprezentuje gracza (Pac-Mana) w grze. Odpowiada za sterowanie, aktualizację stanu (punktacja, power-up) oraz reagowanie na kolizje z innymi obiektami. Dodatkowo, obsługuje zmiany kierunku ruchu i zbieranie pelletów.

#### Ghost

Reprezentuje przeciwników w grze (duchy). Zawiera logikę AI, umożliwiającą śledzenie gracza oraz zmianę stanów (np. przechodzenie w tryb przestraszenia, kiedy gracz zbierze power-pellet). Steruje ruchem duchów i ich interakcją z innymi obiektami.

#### Pellet

Odpowiada za kuleczki, które gracz zbiera, aby zdobywać punkty. Może występować jako zwykły pellet lub power-pellet, który dodatkowo włącza tryb wzmacniający u gracza. Obsługuje podstawową logikę aktualizacji i reagowania na kolizje.

### NetworkManager

Singleton odpowiedzialny za obsługę komunikacji sieciowej. Zarządza inicjalizacją serwera i klienta, synchronizacją stanu gry między graczami oraz przesyłaniem pakietów (pozycje graczy, stan punktacji, zbierane power-upy i pelletki). Odpowiada także za kolejkowanie i buforowanie wiadomości.

## Podsumowanie

TODO
