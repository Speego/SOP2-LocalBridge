#include "server.h"

Server::Server() {
    playerID = 1;
    players.reserve(200);
    tables.reserve(50);
}

bool Server::createSignalTables() {
    messageTablesID = msgget(keyTables, 0600 | IPC_CREAT);
    if (messageTablesID < 0) return false;
    return true;
}
void Server::removeSignalTables() {
    msgctl(messageTablesID, IPC_RMID, NULL);
}
void Server::sendSignalNumberOfTables() {
    struct messageTables msgt;

    msgt.messageType = 2;
    msgt.message = (int)tables.size();
    msgsnd(messageTablesID, &msgt, sizeof(int), 0);
}
void Server::sendSignalNumberOfPlayersAtTable() {
    struct messageTables msgt;

    for (int i=0; i<(int)tables.size(); i++) {
        msgt.messageType = i + 3;
        msgt.message = (int)tables[i].players.size();
        msgsnd(messageTablesID, &msgt, sizeof(int), 0);
    }
}
void Server::sendSignalTableReady(int numberOfTable) {
    struct messageTables msgt;

    msgt.messageType = numberOfTable + 103;
    msgt.message = 1;
    for (int i=0; i<4; i++)
        msgsnd(messageTablesID, &msgt, sizeof(int), 0);
}

bool Server::createSignalPlayers() {
    messagePlayersID = msgget(keyPlayers, 0600 | IPC_CREAT);
    if (messagePlayersID < 0) return false;
    return true;
}
void Server::removeSignalPlayers() {
    msgctl(messagePlayersID, IPC_RMID, NULL);
}
void Server::sendSignalPlayers() {
    struct messagePlayer msgp;
    string playerName;

    for (int i=0; i<(int)tables.size(); i++)
        for (int j=0; j<(int)tables[i].players.size(); j++) {
            playerName = tables[i].players[j].name;
            msgp.messageType = i + 3;
            for (int k=0; k<(int)playerName.size(); k++)
                msgp.messageName[k] = playerName[k];
            msgp.nameLength = (int)playerName.size();

            msgsnd(messagePlayersID, &msgp, 20*sizeof(char)+2*sizeof(int), 0);
        }
}
bool Server::receiveSignalPlayer() {
    struct messagePlayer msgp;
    int isMessage;
    int numberOfTable;

    isMessage = msgrcv(messagePlayersID, &msgp, 20*sizeof(char)+2*sizeof(int), 1, IPC_NOWAIT);
    if (isMessage >= 0) {
        string playerName(msgp.messageName);

        playerName.resize(msgp.nameLength);

        numberOfTable = msgp.messageTable;
        if (numberOfTable > (int)tables.size()) {
            numberOfTable = (int)tables.size() + 1;
            tables.push_back(*(new Table((int)tables.size()+1)));
        }

        if ((int)tables[numberOfTable-1].players.size() == 4) {
            sendSignalAcceptance(numberOfTable, false);
        }
        else {
            sendSignalAcceptance(numberOfTable, true);
            players.push_back(*(new Player(playerID, playerName, numberOfTable)));
            tables[numberOfTable-1].players.push_back(*(new Player(playerID, playerName, numberOfTable)));
            if ((int)tables[numberOfTable-1].players.size() > 3)
                tables[numberOfTable-1].tableReady = true;

            sendSignalID();
            playerID++;
        }

        return true;
    }
    return false;
}

bool Server::createSignalCards() {

    messageCardID = msgget(keyCards, 0600 | IPC_CREAT);
    if (messageCardID < 0) return false;
    return true;
}
bool Server::createSignalID() {
    messageIDID = msgget(keyID, 0600 | IPC_CREAT);
    if (messageIDID < 0) return false;
    return true;
}
void Server::sendSignalID() {
    struct messageID msgi;

    msgi.messageType = 1;
    msgi.messageid = playerID;
    msgsnd(messageIDID, &msgi, sizeof(int), 0);
}

bool Server::createSignalAcceptance() {
    messageAcceptanceID = msgget(keyAcceptance, 0600 | IPC_CREAT);
    if (messageAcceptanceID < 0) return false;
    return true;
}
void Server::sendSignalAcceptance(int numberOfTable, bool acceptance) {
    struct messageAcceptance msga;

    msga.messageType = numberOfTable;
    msga.messageAcceptance = acceptance;
    msgsnd(messageAcceptanceID, &msga, sizeof(bool), 0);
}

void Server::setServer() {
    createSignalTables();
    createSignalPlayers();
    createSignalCards();
    createSignalID();
    createSignalAcceptance();
    sendSignalNumberOfTables();
}

void Server::manageGames() {
    if (receiveSignalPlayer()) {
        sendSignalNumberOfTables();
        sendSignalNumberOfPlayersAtTable();
        sendSignalPlayers();
    }

    for (int i=0; i<(int)tables.size(); i++)
        if (tables[i].tableReady == true) {
            sendSignalTableReady(i);
            tables[i].createCards();
            tables[i].randomShuffle();
            tables[i].dealCards();

            for (int j=0; j<4; j++)
                tables[i].players[j].sendSignalCards();

            tables[i].prepareForBidding();
        }

    for (int i=0; i<(int)tables.size(); i++)
        if (tables[i].biddingOn == true)
            tables[i].bidding();

    for (int i=0; i<(int)tables.size(); i++)
        if (tables[i].biddingEnd == true)
            tables[i].prepareForGame();

    for (int i=0; i<(int)tables.size(); i++)
        if (tables[i].gameOn == true)
            tables[i].game();

    for (int i=0; i<(int)tables.size(); i++)
        if (tables[i].endOn == true) {
            playerToRemoveID = tables[i].endGame();
            if (playerToRemoveID != -1)
                removePlayer(playerToRemoveID);
        }

}

void Server::removePlayer(int playerID) {
    int playerPosition;

    for (int i=0; i<(int)players.size(); i++)
        if (players[i].id == playerID)
            playerPosition = i;

    players.erase(players.begin() + playerPosition);
}
