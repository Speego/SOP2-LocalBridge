#ifndef TABLE_H
#define TABLE_H

#include "card.h"
#include "player.h"

#include <cstdlib>
#include <vector>
#include <iostream>

using namespace std;

/*
    key:
        104
    types:
        1, 2, 3... -> id of each player
        OR (whether sent by server class)
        1, 2, 3... -> number of table
*/
struct messageAcceptance {
    long messageType;
    bool messageAcceptance;
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
struct messagePlaceNumber {
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
struct messageBidToSend {
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
struct messageScores {
    long messageType;
    int score;
};

class Table {
public:
    static const key_t keyCards = 102;
    static const key_t keyAcceptance = 104;
    static const key_t keyBid = 105;
    static const key_t keyPlaceNumber = 106;
    static const key_t keyBidToSend = 107;
    static const key_t keyBiddingEnd = 108;
    static const key_t keyScores = 109;
    int messageCardID;
    int messageAcceptanceID;
    int messageBidID;
    int messagePlaceNumberID;
    int messageBidToSendID;
    int messageBiddingEndID;
    int messageScoresID;

    bool tableReady;
    bool gameOn;
    bool biddingOn;
    bool biddingEnd;
    bool endOn;
    int tableID;
    int numberOfPasses;
    int dealer;
    int player;
    int trumpsSuit;
    int trumpsHeight;
    int biddingWinner;

    int bidderScore;
    int roundWinner;
    int winSuit;
    int firstSuit;
    int cardCounter;
    int roundCounter;
    char winType;
    bool dummyPlaying;

    int numberOfPlayersResponses;
    int vectorIterator;

    vector <Card> cards;
    vector <Player> players;

    Table(int);

    bool getSignalCards();
    void sendSignalDummyCards(int, int);
    void sendSignalCard(int, char, int);

    bool getSignalAcceptance();
    void sendSignalAcceptance(int, bool);

    bool getSignalBid();

    bool getSignalPlaceNumber();
    void sendSignalPlaceNumber(int, int);

    bool getSignalBidToSend();
    void sendSignalBidToSend(int);

    bool createSignalBiddingEnd();
    void sendSignalBiddingEnd(bool);
    void sendSignalBiddingWon(int, bool, int);

    bool getSignalScores();
    void sendSignalScores(int);

    void createCards();
    void writeCards();
    void randomShuffle();
    void dealCards();

    void prepareForBidding();
    void bidding();
    void prepareForGame();
    void game();
    int endGame();

    void removePlayer(int);

    int checkCard(char, int);
    bool checkCardType(char);
};

#endif // TABLE_H
