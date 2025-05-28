# Czat w C++ (Multi-Klientowy)

## 1. Instrukcja uruchomienia

### Wymagania
- Kompilator zgodny ze standardem C++11 lub nowszym (np. g++, clang++).
- System operacyjny Windows z biblioteką `ws2_32` do obsługi gniazd sieciowych.

### Kompilacja i uruchomienie

#### Kompilacja
```bash
g++ chat_server.cpp -o chat.exe -lws2_32 -std=c++11
g++ chat_client.cpp -o client.exe -lws2_32 -std=c++11
```

#### Uruchomienie

Uruchom program serwera:
```bash
./chat.exe
```

Uruchom program klienta (w nowym terminalu – można uruchomić kilka instancji klienta):
```bash
./client.exe
```

Po uruchomieniu klienta program poprosi o wpisanie nicku, który będzie wyświetlany przy każdej wysyłanej wiadomości.

---

## 2. Opis problemu

Celem projektu jest stworzenie prostego czatu w architekturze klient-serwer, umożliwiającego przesyłanie wiadomości tekstowych między użytkownikami. 

### Główne wyzwania:
- **Obsługa wielu klientów** – serwer musi obsługiwać jednocześnie wielu klientów.
- **Zarządzanie stanem użytkowników** – konieczne jest utrzymywanie listy połączonych użytkowników oraz ich nicków.

---

## 3. Wątki i ich reprezentacja

### Serwer
Serwer uruchamia **jeden wątek na klienta**, który obsługuje komunikację z danym klientem.

- **Funkcja:** `handle_client(client_socket)`
- **Opis:** Obsługuje wiadomości jednego klienta, odbiera i przekazuje je innym użytkownikom.

### Klient
Klient uruchamia **dwa wątki**:
- jeden do **wysyłania wiadomości**
- drugi do **odbierania wiadomości** od serwera.

- **Funkcja:** `receive_messages(sock)`
- **Opis:** Nasłuchuje wiadomości przychodzących od serwera i wyświetla je w konsoli.

---

## 4. Sekcje krytyczne i ich rozwiązanie

### Sekcje krytyczne:
- **Zarządzanie połączeniami użytkowników:** modyfikacja listy klientów musi być zsynchronizowana.
- **Broadcast wiadomości:** wysyłanie wiadomości do wszystkich klientów wymaga synchronicznego dostępu do listy klientów.

### Rozwiązania synchronizacji:
- **Spinlock:** użycie funkcji `_InterlockedExchange` (API Windows) do synchronizacji dostępu do listy klientów.
- **Blokady przy broadcastowaniu:** funkcja `broadcast_message` używa blokady, aby unikać problemów przy równoczesnym wysyłaniu wiadomości.

---

## 5. Dodatkowe informacje

Projekt realizuje podstawowe funkcje czatu w architekturze klient-serwer i pokazuje, jak obsługiwać wiele połączeń jednocześnie z użyciem wątków. Synchronizacja jest realizowana za pomocą mechanizmów Windows API, co umożliwia stabilną pracę serwera i klientów.
