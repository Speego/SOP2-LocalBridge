Projekt ten to zadanie 3. z listy zadań projektowych, czyli symulacja gry w brydża.

INSTRUKCJA KOMPILACJI:
Projekt jest zawarty w dwóch folderach - "BridgeClient" i "BridgeServer". Jak mówią nazwy, jeden z folderów zawiera program klienta, drugi zaś program serwera. W każdym z folderów jest plik "Makefile", który należy włączyć za pomocą polecenia "make" w konsoli dla każdego z modułów projektu.


INSTRUKCJA URUCHOMIENIA I WYŁĄCZENIA:
Aby skorzystać z programu, należy najpierw włączyć serwer (choć w niczym nie przeszkadza włączenie najpierw klientów, gdyż i tak będą oni czekali na komunikat wysłany przez serwer). Każdy z klientów musi być osobno odpalonym procesem, który będzie komunikował się z włączonym serwerem.

UWAGA!
Nie wolno przerywać działania już włączonego programu za pomocą CTRL+C lub innych sposobów. Jeżeli tak się stanie, trzeba będzie zabić wszystkie procesy ręcznie, gdyż projekt przestanie działać poprawnie. Klient ma możliwość wyjścia z programu po zakończonej rozgrywce. W celu wyłączenia serwera należy go zabić ręcznie, gdyż działa w nieskończonej pętli. 

UWAGA 2!
Po zabiciu procesu serwera, należy uruchomić skrypt "ipcRemove" dołączony do projektu, który usuwa stworzone kolejki komunikatów. W przeciwnym wypadku pozostaną w tych kolejkach komunikaty, które po ponownym odpaleniu projektu przestaną być aktualne, co spowoduje niepoprawne działanie gier.

Instrukcja, jak grać, jest załączona w pliku "INSTRUKCJA.txt". Zawiera ona zasady obsługiwania programu. Zakłada się, że gracz zna zasady brydża.


OPIS ZAWARTOŚCI PLIKÓW SERWERA:
Od strony serwera:
"main.cpp" - tworzy klasę serwera i odpala jego działanie.

"server.*" - obsługuje nowych graczy, od których zbiera podstawowe dane, przydziela im unikalny identyfikator. Tworzy podstawowe kolejki komunikatów. Zapisuje graczy w wektorze. Ponadto, tworzy klasy stolików, które będą obsługiwać samą licytację oraz grę. Zarządza wszystkimi stolikami - przechodzi przez wszystkie oraz odpala odpowiednie funkcje w zależności od tego, na jakim etapie czekania lub gry jest dany stolik.

"table.*" - tworzy kolejki komunikatów potrzebne do obsługi licytacji i gry. Po zebraniu czwórki graczy zarządza licytacją, następnie zarządza grą oraz oznajmia wynik gry. Po zakończeniu gry pyta graczy, czy chcą zostać przy stoliku. Jeżeli któryś z graczy odejdzie, czeka na nowych graczy, w przeciwnym wypadku od razu odpala nową licytację.

"player.*" - przechowuje dane o graczach oraz ich kartach.

"card.*" - zawiera klasę pomocniczą przechowującą dane o karcie.

Od strony klienta:
"mainC.cpp" - tworzy klasę klienta i odpala jego działanie.

"client.*" - w sumie jedyna klasa zarządzająca całym klientem. Zawiera wszystkie struktury potrzebne do obsłużenia wszystkich kolejek komunikatów. Nie będę tu więcej opisywał, gdyż było by to powtórzenie zasad działania projektu, które zawarte są w pliku "CONCEPT.txt".

"card.*" - zawiera klasę pomocniczą przechowującą dane o karcie.
