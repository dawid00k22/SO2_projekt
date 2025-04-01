Dokumentacja projektu "Problem filozofów"
 1. Instrukcja uruchomienia

 Wymagania
- Kompilator zgodny ze standardem C++14 lub nowszym (np. g++, clang++).
- System operacyjny obsługujący wątki POSIX lub Windows z biblioteką `std::thread`.

Kompilacja i uruchomienie
1) Skompiluj kod:

g++ -std=c++14 philosophers.cpp -o philosophers -pthread

2) Uruchom program, podając liczbę filozofów jako argument:

philosophers 5

3) Program będzie działać przez 10 sekund, po czym zakończy działanie i wyświetli statystyki.


2. Opis problemu

Problem filozofów dotyczy synchronizacji wątków w systemach wielozadaniowych. N filozofów siedzi przy okrągłym stole i na zmianę myśli oraz je. Każdy filozof potrzebuje dwóch widelców do jedzenia, ale widelce są współdzielone przez sąsiadujących filozofów. Główne problemy, jakie mogą wystąpić:

1) Zakleszczenie (deadlock) - jeśli wszyscy filozofowie jednocześnie podniosą lewy widelec i będą czekać na prawy, żaden z nich nie zacznie jeść.
2) Głodzenie (starvation) - jeśli priorytetowi filozofowie częściej zdobywają widelce, inni mogą nigdy nie dostać szansy na jedzenie.

W tym programie zakleszczenie jest rozwiązane poprzez semafor ograniczający liczbę filozofów mogących jeść jednocześnie do N-1. Pozwala to uniknąć sytuacji, w której wszyscy filozofowie jednocześnie podniosą tylko jeden widelec i zablokują się nawzajem.


3. Wątki i ich reprezentacja

- Każdy filozof jest reprezentowany jako osobny wątek (std::thread).
- Każdy wątek wykonuje funkcję philosopher, w której filozof na zmianę myśli i je.

Lista wątków
1) Wątel: philosopher(i, num_philosophers, table_semaphore)
2) Opis wątku: Reprezentuje filozofa numer i. Wykonuje cyklicznie operacje "myślenia" i "jedzenia".

4. Sekcje krytyczne i ich rozwiązanie

Sekcje krytyczne
1) Dostęp do widelców (mutexy) - Każdy filozof musi zdobyć dwa widelce (mutexy) przed jedzeniem.
2) Wypisywanie na konsolę (mutex) - Ponieważ wielu filozofów może jednocześnie pisać do konsoli, konieczne jest zablokowanie tego zasobu.

Rozwiązanie synchronizacji
1) Unikanie zakleszczenia - Program stosuje semafor (table_semaphore), który ogranicza liczbę filozofów mogących jednocześnie podnosić widelce.
2) Unikanie konfliktów przy dostępie do zasobów:
   - Widelce - Każdy filozof blokuje lewy i prawy widelec (std::mutex).
   - Konsola - Dostęp do std::cout jest chroniony przez cout_mutex, aby uniknąć pomieszanych wydruków.



Projekt spełnia wymagania poprzez własnoręcznie zaimplementowane mechanizmy synchronizacji, zapewniając poprawne działanie wątków oraz unikanie zakleszczenia.

