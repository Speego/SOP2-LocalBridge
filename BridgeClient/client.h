#ifndef CLIENT_H
#define CLIENT_H

#include "card.h"

#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/msg.h>

#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/*
    key:
        100
    types:
        1 -> number of tables
        2 and next -> number of people at the table 1, 2, ...
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
        102
    types:
        id -> id of each player
*/
struct messageCard {
    long messageType;
    char messageCardType;
    int messageCardSuit;
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

/*
    key:
        104
    types:
        1, 2, 3... -> id of each player
*/
struct messageAcceptance {
    long messageType;
    bool acceptance;
};

/*
    key:
        105
    types:
        1, 2, 3... -> id of each player
    trumpsHeight:
        0 -> pass
        1 -> trefl
        2 -> karo
        3 -> kier
        4 -> pik
        5 -> BA
*/
struct messageBid {
    long messageType;
    int trumpsHeight;
    int trumpsSuit;
};

/*
    key:
        106
    types:
        1, 2, 3... -> id of each player
*/
struct mesagePlaceNumber {
    long messageType;
    int placeNumber;
};

/*
    key:
        107
    types:
        1, 2, 3... -> id of each player
    trumpsHeight:
        0 -> pass
        1 -> trefl
        2 -> karo
        3 -> kier
        4 -> pik
        5 -> BA
*/
struct messageBidAnotherPlayers {
    long messageType;
    int trumpsHeight;
    int trumpsSuit;
};

/*
    key:
        108
    types:
        1 and next -> id of each player
    player -> 1 - bidding winner, 2 -> partner
*/
struct messageBiddingEnd {
    long messageType;
    bool isEnd;
    int player;
};

/*
    key:
        109
    types:
        1 and next -> id of each player
*/
struct messageScore {
    long messageType;
    int score;
};

class Client {
public:
    static const key_t keyTables = 100;
    static const key_t keyPlayers = 101;
    static const key_t keyCards = 102;
    static const key_t keyID = 103;
    static const key_t keyAcceptance = 104;
    static const key_t keyBid = 105;
    static const key_t keyPlaceNumber = 106;
    static const key_t keyBidAnotherPlayers = 107;
    static const key_t keyBiddingEnd = 108;
    static const key_t keyScore = 109;
    int messageTablesID;
    int messagePlayersID;
    int messageCardsID;
    int messageIDID;
    int messageAcceptanceID;
    int messageBidID;
    int messagePlaceNumberID;
    int messageBidAnotherPlayersID;
    int messageBiddingEndID;
    int messageScoreID;

    bool isBidOK;
    bool biddingOn;
    bool biddingWon;
    bool gameOn;
    bool wantsToPlay;
    int table;
    int id;
    int placeNumber;
    int currentPlayer;
    int trumpsHeight;
    int trumpsSuit;
    int cardSuit;
    int declarerOrPartner;
    char cardType;

    string playerName;
    vector <Card> cards;
    vector <Card> dummyCards;

    Client();

    void setPlayerName();

    bool getSignalPlayers();
    bool sendSignalPlayer(int);
    void receiveSignalPlayers(int);

    bool getSignalTables();
    int receiveSignalTables();
    void receiveSignalTableReady();

    bool getSignalCards();
    void sendSignalCard();
    void receiveSignalCard();
    void receiveSignalCards(int);

    bool getSignalID();
    void receiveSignalID();

    bool getSignalAcceptance();
    bool receiveSignalAcceptance();
    void sendSignalPlayAgain(bool);

    bool getSignalBid();
    void sendSignalBid(int, int);

    bool getSignalPlaceNumber();
    int receiveSignalPlaceNumber();

    bool getSignalBidAnotherPlayers();
    void receiveSignalBidAnotherPlayers();

    bool getSignalBiddingEnd();
    bool receiveSignalBiddingEnd();

    bool getSignalScore();
    int receiveSignalScore();

    void prepareToGame();
    void waitForGame();
    void bidding();
    void game();
    void endGame();
    void playingAgain();

    void gameManager();

    void removeCard();
    void removeDummyCard();

    void sortCardsSuit();
    void sortCardsType();
    void sortDummyCardsSuit();
    void sortDummyCardsType();
    void writeCards();
    void writeDummyCards();
};

bool compareCardsSuit(Card, Card);
bool compareCardsType(Card, Card);

#endif // CLIENT_H

