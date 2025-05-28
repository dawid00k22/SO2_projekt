Dokumentacja projektu "Czat w C++ (multi-klientowy)"

1. Instrukcja uruchomienia
Wymagania

Kompilator zgodny ze standardem C++11 lub nowszym (np. g++, clang++).

System operacyjny Windows z biblioteką ws2_32 do obsługi gniazd sieciowych.

Kompilacja i uruchomienie

Skompiluj kod:


g++ chat_server.cpp -o server.exe -lws2_32 -std=c++11

g++ chat_client.cpp -o client.exe -lws2_32 -std=c++11

Uruchom program serwera:

./server.exe
Uruchom program klienta (w nowym terminalu – można uruchomić kilka instancji klienta):

./client.exe
Po uruchomieniu klienta program poprosi o wpisanie nicku, który będzie wyświetlany przy każdej wysyłanej wiadomości.

2. Opis problemu
Celem projektu jest stworzenie prostego czatu w architekturze klient-serwer, w którym użytkownicy mogą wysyłać wiadomości tekstowe. Projekt ma na celu implementację wielowątkowego serwera, który obsługuje wiele połączeń klientów. Każdy klient może wysyłać wiadomości publiczne, prywatne oraz wykonywać inne operacje, takie jak uzyskanie listy dostępnych użytkowników.

Główne wyzwania to:

Obsługa wielu klientów – serwer musi obsługiwać jednocześnie wielu klientów i zapewniać odpowiednią synchronizację dostępu do wspólnych zasobów.

Zarządzanie stanem użytkowników – konieczność utrzymania listy połączonych użytkowników i ich nazw, a także umożliwienie komunikacji między nimi.

3. Wątki i ich reprezentacja
Serwer:
Serwer uruchamia wątek dla każdego połączonego klienta, obsługujący komunikację z danym klientem. Funkcja handle_client odpowiada za odbieranie i przesyłanie wiadomości od klienta.

Klient:
Klient uruchamia dwa wątki: jeden do wysyłania wiadomości, a drugi do odbierania wiadomości od serwera. Wątek odbierający wiadomości odpowiada za ciągłe nasłuchiwanie przychodzących wiadomości i wyświetlanie ich w konsoli.

Lista wątków:

Wątek w serwerze:

Funkcja: handle_client(client_socket)

Opis: Obsługuje wiadomości wysyłane przez jednego klienta, odbiera wiadomości i przesyła je do innych połączonych użytkowników.

Wątek w kliencie (odbierający wiadomości):

Funkcja: receive_messages(sock)

Opis: Nasłuchuje przychodzących wiadomości od serwera i wyświetla je na ekranie.

4. Sekcje krytyczne i ich rozwiązanie
Sekcje krytyczne:

Zarządzanie połączeniami użytkowników (w serwerze):
Lista połączonych klientów musi być modyfikowana w sposób synchroniczny, aby unikać konfliktów przy dodawaniu lub usuwaniu połączeń.

Wysłanie wiadomości do wszystkich klientów (broadcasting):
Wysłanie wiadomości do wszystkich połączonych klientów wymaga dostępu do listy klientów w sposób synchroniczny, aby uniknąć wyścigu przy dodawaniu nowych użytkowników.

Rozwiązanie synchronizacji:

Użycie spinlocka do synchronizacji dostępu do listy klientów:
Program wykorzystuje mechanizm spinlocka przy pomocy funkcji _InterlockedExchange (w API Windows) w celu synchronizacji dostępu do listy połączeń.

Zarządzanie wiadomościami:
Funkcja broadcast_message używa blokady przy wysyłaniu wiadomości do wszystkich klientów, aby uniknąć problemów z równoczesnym dostępem.

5. Dodatkowe informacje
Projekt realizuje podstawowe funkcje czatu w architekturze klient-serwer i demonstruje zarządzanie wieloma połączeniami jednocześnie za pomocą wątków. Synchronizacja dostępu do wspólnych zasobów została zaimplementowana przy użyciu mechanizmów Windows API, co pozwala na prawidłowe działanie serwera i klientów.









