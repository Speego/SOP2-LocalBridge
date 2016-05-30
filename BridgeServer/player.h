#ifndef PLAYER_H
#define PLAYER_H

#include "card.h"

#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/msg.h>

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

/*
    key:
        102
    types:
        1, 2, 3... -> id of each player
*/
struct messageCard {
    long messageType;
    char messageCardType;
    int messageCardSuit;
};

class Player {
public:
    static const key_t keyCards = 102;
    int messageCardID;

    int id;
    int table;
    string name;
    vector <Card> cards;

    Player(int, string, int);

    bool getSignalCards();
    void sendSignalCards();

    void removeCard(int);

    void sortCardsSuit();
    void sortCardsType();
    void writeCards();
};

bool compareCardsSuit(Card, Card);
bool compareCardsType(Card, Card);

#endif // PLAYER_H
