#ifndef SERVER_H
#define SERVER_H

#include "table.h"

#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/msg.h>

#include <string>
#include <unistd.h>

using namespace std;

/*
    key:
        100
    types:
        2 -> number of tables
        3 and next -> number of people at the table 1, 2, ...
        103 and next -> table 1, 2, ... is ready
*/
struct messageTables {
    long messageType;
    int message;
};

/*
    key:
        101
    types:
        1 -> player that wants to join
        3 and next -> players at the table 1, 2, ...
*/
struct messagePlayer {
    long messageType;
    char messageName[20];
    int messageTable;
    int nameLength;
};

/*
    key:
        103
    types:
        1
*/
struct messageID {
    long messageType;
    int messageid;
};

class Server {
public:
    static const key_t keyTables = 100;
    static const key_t keyPlayers = 101;
    static const key_t keyCards = 102;
    static const key_t keyID = 103;
    static const key_t keyAcceptance = 104;
    int messageTablesID;
    int messagePlayersID;
    int messageCardID;
    int messageIDID;
    int messageAcceptanceID;

    int playerID;
    int playerToRemoveID;
    vector <Player> players;
    vector <Table> tables;

    Server();

    bool createSignalTables();
    void removeSignalTables();
    void sendSignalNumberOfTables();
    void sendSignalNumberOfPlayersAtTable();
    void sendSignalTableReady(int);

    bool createSignalPlayers();
    void removeSignalPlayers();
    void sendSignalPlayers();
    bool receiveSignalPlayer();

    bool createSignalCards();

    bool createSignalID();
    void sendSignalID();

    bool createSignalAcceptance();
    void sendSignalAcceptance(int, bool);

    void setServer();
    void manageGames();

    void removePlayer(int);
};

#endif // SERVER_H
