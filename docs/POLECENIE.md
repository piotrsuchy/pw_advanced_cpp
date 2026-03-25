# POLECENIE

## Etap I – wybór tematu i wstępny opis (2. tydzień zajęć)

W 2. tygodniu zajęć prosimy zamieścić w Lei, w sekcji „wstępny opis projektu”:

- wybrany temat,
- uszczegółowienie (czyli doprecyzowanie funkcjonalności) – prosimy opisać na ok. jedną stronę, jak według Was program ma działać,
- szkic interfejsu użytkownika (może być odręczny, jako obrazek lub opis ekranów), a jeśli wolicie – szkic głównych klas / modułów.

Chodzi o to, żebyśmy od początku widzieli, co dokładnie planujecie zrobić, i mogli szybko podpowiedzieć korekty, zanim pójdziecie w złą stronę.

## Etap II – diagram klas (5. tydzień zajęć)

W 5. tygodniu zajęć prosimy zamieścić w Lei co najmniej diagram klas programu. Może to być diagram wstępny – ważne, żeby było widać planowany podział na klasy oraz relacje między nimi.

Dokładny prompt dla etapu II:
"Waszym zadaniem jest przygotowanie i przesłanie diagramu klas, wraz ze skrótowym opisem każdej z klas (jej odpowiedzialności, na max kilka zdań), w formacie pdf. Proszę pamiętać o rozdzieleniu logiki i interfejsu użytkownika w Waszej propozycji.
Dokumentację proszę zamieszczać w sposób przyrostowy - tzn tu wgrywacie dokument z poprzedniego etapu rozszerzony o nowe elementy."

## Etap III – oddanie projektu i zgłoszenie obrony

Gotowy projekt należy zamieścić w Lei najpóźniej 3 dni przed planowanym terminem obrony. W przesyłce powinny znaleźć się:

a) Kody źródłowe programu (wraz z plikiem projektu i plikami zasobów), bez wyników kompilacji (czyli bez katalogów typu build, bin, obj itp.) oraz dodatkowo link do repozytorium z kodem.

b) Raport z wykonania programu, zawierający:

- aktualny diagram klas,
- instrukcję obsługi programu (diagram interakcji / opis kroków) – o ile obsługa nie jest trywialna.

Dodatkowo prosimy przesłać mail do prowadzącego z informacją, że chcecie przystąpić do obrony.

### Ważne: kolejność etapów

Obowiązuje zachowanie kolejności oddawania etapów:

- nie można oddać Etapu II, jeśli Etap I nie został oceniony,
- nie można oddać Etapu III, jeśli Etap II nie został oceniony.

Dlatego zachęcamy do regularnej pracy – to naprawdę ułatwia życie zarówno Wam, jak i prowadzącym.

### Wymagania wspólne dla wszystkich projektów

W każdym projekcie wymagamy następujących elementów:

- Prawidłowe wykorzystanie klas – program ma być napisany obiektowo, a podział na klasy i przypisanie im odpowiedzialności ma być sensowne (w razie wątpliwości – konsultujcie pomysły z prowadzącymi).
- Dziedziczenie – musi występować w projekcie; w projektach ocenianych na więcej niż 3.0 musi pojawić się także polimorfizm.
- Dobry styl organizacji kodu – deklaracje klas w plikach nagłówkowych (.h), definicje metod w plikach implementacji (.cpp), każda klasa w swoich plikach. Szablony umieszczamy w całości w nagłówkach.
- Dokumentacja doxygen – wymagany jest udokumentowany interfejs publiczny klas: opis przeznaczenia klasy oraz opis metod publicznych i chronionych (do czego służą, parametry, wartość zwracana).
- Wykorzystanie STL (lub innej biblioteki opartej na szablonach) – nawet jeśli w danym projekcie będzie to „na siłę”.
- Rozdzielenie logiki od interfejsu użytkownika – o ile nie ma ku temu uzasadnionych przeciwwskazań. W idealnym przypadku program powinien dać się zbudować i uruchomić także w wersji bez GUI.
- Dla maksymalnej oceny: przygotowanie i wykorzystanie własnego szablonu / własnych szablonów (nawet jeśli będą proste i nie w pełni optymalne).
- Dla maksymalnej oceny: korzystanie z repozytorium (git lub svn) od początku pracy, również przy pracy indywidualnej.
