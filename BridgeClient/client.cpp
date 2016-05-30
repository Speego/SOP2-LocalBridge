#include "client.h"

bool compareCardsSuit(Card x, Card y) {
    return x.suit < y.suit;
}
bool compareCardsType(Card x, Card y) {
    return (int)x.type < (int)y.type;
}

Client::Client() {
    cards.reserve(13);
    dummyCards.reserve(13);
}

void Client::setPlayerName() {
    cout << "Write your nickname (max 15 characters) : ";
    cin >> playerName;
}

bool Client::getSignalPlayers() {
    messagePlayersID = msgget(keyPlayers, 0600 | IPC_CREAT);
    if (messagePlayersID < 0) return false;
    return true;
}
bool Client::sendSignalPlayer(int numberOfTables) {
    struct messagePlayer msgp;
    struct messageAcceptance msga;
    int numberOfTable = -1;

    while (numberOfTable < 1 || numberOfTable > numberOfTables+1) {
        cout << "Write a number of the table you want to join in. If you want to create new one, write a number one greater than the highest table.\n";
        cin >> numberOfTable;
    }

    table = numberOfTable;

    msgp.messageType = 1;
    msgp.messageTable = numberOfTable;
    for (int i=0; i<(int)playerName.size(); i++)
        msgp.messageName[i] = playerName[i];
    memset(msgp.messageName+(int)playerName.size(), 0, 20-(int)playerName.size());
    msgp.nameLength = (int)playerName.size();

    msgsnd(messagePlayersID, &msgp, 20*sizeof(char)+2*sizeof(int), 0);

    msgrcv(messageAcceptanceID, &msga, sizeof(bool), numberOfTable, 0);

    return msga.acceptance;
}
void Client::receiveSignalPlayers(int numberOfTable) {
    struct messagePlayer msgp;

    msgrcv(messagePlayersID, &msgp, 20*sizeof(char)+2*sizeof(int), numberOfTable+2, 0);
    for (int i=0; i<msgp.nameLength; i++)
        cout << msgp.messageName[i];
    cout << ", ";
}

bool Client::getSignalTables() {
    messageTablesID = msgget(keyTables, 0600 | IPC_CREAT);
    if (messagePlayersID < 0) return false;
    return true;
}
int Client::receiveSignalTables() {
    struct messageTables msgt;
    int numberOfTables;

    msgrcv(messageTablesID, &msgt, sizeof(int), 2, 0);
    numberOfTables = msgt.message;

    cout << "Number of tables: " << numberOfTables << endl;
    for (int i=0; i<numberOfTables; i++) {
        msgrcv(messageTablesID, &msgt, sizeof(int), i+3, 0);
        cout << "Table " << i+1 << " has " << msgt.message << " players: ";
        for (int j=0; j<msgt.message; j++)
            receiveSignalPlayers(i+1);

        cout << endl;
    }

    return numberOfTables;
}
void Client::receiveSignalTableReady() {
    struct messageTables msgt;

    msgrcv(messageTablesID, &msgt, sizeof(int), table+102, 0);
}

bool Client::getSignalCards() {
    messageCardsID = msgget(keyCards, 0600 | IPC_CREAT);
    if (messageCardsID < 0) return false;
    return true;
}
void Client::sendSignalCard() {
    struct messageCard msgc;

    msgc.messageType = id;
    cin >> cardType >> cardSuit;
    msgc.messageCardType = cardType;
    msgc.messageCardSuit = cardSuit;
    msgsnd(messageCardsID, &msgc, sizeof(char)+2*sizeof(int), 0);
}
void Client::receiveSignalCard() {
    struct messageCard msgc;

    msgrcv(messageCardsID, &msgc, sizeof(char)+2*sizeof(int), id, 0);
    cardType = msgc.messageCardType;
    cardSuit = msgc.messageCardSuit;

    cout << "Player " << currentPlayer+1 << " plays " << cardType;
    switch (cardSuit) {
        case 1: cout << " HEART\n"; break;
        case 2: cout << " DIAMOND\n"; break;
        case 3: cout << " CLUB\n"; break;
        case 4: cout << " SPADE\n"; break;
    }
    cout.flush();
}
void Client::receiveSignalCards(int playerOrPartner) {
    struct messageCard msgc;

    if (playerOrPartner == 1)
        cards.clear();
    else dummyCards.clear();

    for (int i=0; i<13; i++) {
        msgrcv(messageCardsID, &msgc, sizeof(char)+2*sizeof(int), id, 0);
        if (playerOrPartner == 1)
            cards.push_back(*(new Card(msgc.messageCardType, msgc.messageCardSuit)));
        else
            dummyCards.push_back(*(new Card(msgc.messageCardType, msgc.messageCardSuit)));
    }
}

bool Client::getSignalID() {
    messageIDID = msgget(keyID, 0600 | IPC_CREAT);
    if (messageIDID < 0) return false;
    return true;
}
void Client::receiveSignalID() {
    struct messageID msgi;

    msgrcv(messageIDID, &msgi, sizeof(int), 1, 0);
    id = msgi.messageid;
}

bool Client::getSignalAcceptance() {
    messageAcceptanceID = msgget(keyAcceptance, 0600 | IPC_CREAT);
    if (messageAcceptanceID < 0) return false;
    return true;
}
bool Client::receiveSignalAcceptance() {
    struct messageAcceptance msga;

    msgrcv(messageAcceptanceID, &msga, sizeof(bool), id, 0);
    return msga.acceptance;
}
void Client::sendSignalPlayAgain(bool wannaPlay) {
    struct messageAcceptance msga;

    msga.messageType = id;
    msga.acceptance = wannaPlay;
    msgsnd(messageAcceptanceID, &msga, sizeof(bool), 0);
}

bool Client::getSignalBid() {
    messageBidID = msgget(keyBid, 0600 | IPC_CREAT);
    if (messageBidID < 0) return false;
    return true;
}
void Client::sendSignalBid(int height, int suit) {
    struct messageBid msgb;

    msgb.messageType = id;
    msgb.trumpsHeight = height;
    msgb.trumpsSuit = suit;
    msgsnd(messageBidID, &msgb, 2*sizeof(int), 0);
}

bool Client::getSignalPlaceNumber() {
    messagePlaceNumberID = msgget(keyPlaceNumber, 0600 | IPC_CREAT);
    if (messagePlaceNumberID < 0) return false;
    return true;
}
int Client::receiveSignalPlaceNumber() {
    struct mesagePlaceNumber msgpn;

    msgrcv(messagePlaceNumberID, &msgpn, 2*sizeof(int), id, 0);
    return msgpn.placeNumber;
}

bool Client::getSignalBidAnotherPlayers() {
    messageBidAnotherPlayersID = msgget(keyBidAnotherPlayers, 0600 | IPC_CREAT);
    if (messageBidAnotherPlayersID < 0) return false;
    return true;
}
void Client::receiveSignalBidAnotherPlayers() {
    struct messageBidAnotherPlayers msgbap;

    msgrcv(messageBidAnotherPlayersID, &msgbap, 2*sizeof(int), id, 0);
    if (msgbap.trumpsHeight == trumpsHeight && msgbap.trumpsSuit == trumpsSuit)
        cout << "pass.\n";
    else {
        cout << msgbap.trumpsHeight;
        switch (msgbap.trumpsSuit) {
            case 1: cout << " CLUBS\n"; break;
            case 2: cout << " DIAMONDS\n"; break;
            case 3: cout << " HEARTS\n"; break;
            case 4: cout << " SPADES\n"; break;
            case 5: cout << " NO TRUMPS\n"; break;
        }
    }

    trumpsHeight = msgbap.trumpsHeight;
    trumpsSuit = msgbap.trumpsSuit;
    cout.flush();
}

bool Client::getSignalBiddingEnd() {
    messageBiddingEndID = msgget(keyBiddingEnd, 0600 | IPC_CREAT);
    if (messageBiddingEndID < 0) return false;
    return true;
}
bool Client::receiveSignalBiddingEnd() {
    struct messageBiddingEnd msgbe;

    msgrcv(messageBiddingEndID, &msgbe, sizeof(bool)+sizeof(int), id, 0);
    declarerOrPartner = msgbe.player;
    if (msgbe.isEnd == true) return true;
    return false;
}

bool Client::getSignalScore() {
    messageScoreID = msgget(keyScore, 0600 | IPC_CREAT);
    if (messageScoreID < 0) return false;
    return true;
}
int Client::receiveSignalScore() {
    struct messageScore msgs;

    msgrcv(messageScoreID, &msgs, 2*sizeof(int), id, 0);
    return msgs.score;
}


void Client::prepareToGame() {
    bool isChosenTableOK = false;
    int numberOfTables;

    setPlayerName();

    getSignalPlayers();
    getSignalTables();
    getSignalCards();
    getSignalID();
    getSignalAcceptance();
    getSignalBid();
    getSignalPlaceNumber();
    getSignalBidAnotherPlayers();
    getSignalBiddingEnd();
    getSignalScore();

    while (isChosenTableOK == false) {
        numberOfTables = receiveSignalTables();
        isChosenTableOK = sendSignalPlayer(numberOfTables);
        if (isChosenTableOK == false)
            cout << "You can't join chosen table. Choose another one.\n";
    }
    receiveSignalID();
    cout << "Your ID is: " << id << endl;
    cout.flush();
}

void Client::waitForGame() {
    int partner;

    receiveSignalTableReady();
    cout << "Your table is ready.\n\nYour cards:\n";
    cout.flush();

    receiveSignalCards(1);
    writeCards();

    placeNumber = receiveSignalPlaceNumber();
    partner = placeNumber + 2;
    if (partner > 3)
        partner -= 4;
    cout << "\nYour place at table has number " << placeNumber+1 << ". You play with player number " << partner+1 << ".\n";
    cout.flush();

    currentPlayer = 0;
    biddingOn = true;
    biddingWon = false;
    gameOn = true;
}

void Client::bidding() {
    int tmpHeight;
    int tmpSuit;

    biddingOn = receiveSignalBiddingEnd();
    while (currentPlayer != placeNumber && biddingOn == true) {
        cout << "Player " << currentPlayer+1 << " bidded: ";
        receiveSignalBidAnotherPlayers();
        currentPlayer = (currentPlayer+1) % 4;
        biddingOn = receiveSignalBiddingEnd();
    }

    if (biddingOn == true) {
        receiveSignalAcceptance();

        isBidOK = false;
        cout << "Make a bid: ";
        while(!isBidOK) {
            cin >> tmpHeight;
            cin >> tmpSuit;
            sendSignalBid(tmpHeight, tmpSuit);
            isBidOK = receiveSignalAcceptance();
            if (!isBidOK)
                cout << "You've bidded wrong. Try again: ";
            else if (tmpHeight != 0) {
                trumpsHeight = tmpHeight;
                trumpsSuit = tmpSuit;
            }
        }

        currentPlayer = (currentPlayer+1) % 4;
    }
}

void Client::game() {
    bool isCardOK;

    for (int i=0; i<13; i++) {
        for (int j=0; j<4; j++) {
            if (j == 1 && i == 0 && (declarerOrPartner == 2 || declarerOrPartner == 4)) {
                receiveSignalCards(2);
                cout << "\nDummy's cards:\n";
                writeDummyCards();
                cout << endl;
            }

            if (currentPlayer == placeNumber && declarerOrPartner != 3) {
                cout << "Write a card you want to play: ";
                sendSignalCard();
                isCardOK = receiveSignalAcceptance();

                while (!isCardOK) {
                    cout << "You don't know who you are trying to deceive. Write a card again: ";
                    sendSignalCard();
                    isCardOK = receiveSignalAcceptance();
                }

                removeCard();
            }
            else if (declarerOrPartner == 1 && currentPlayer == (placeNumber+2) % 4) {
                    cout << "Write a card you want to play as a dummy: ";
                    sendSignalCard();
                    isCardOK = receiveSignalAcceptance();

                    while (!isCardOK) {
                        cout << "You don't know who you are trying to deceive. Write a card again: ";
                        sendSignalCard();
                        isCardOK = receiveSignalAcceptance();
                    }
                }
                else receiveSignalCard();

            if (currentPlayer == (placeNumber+3-declarerOrPartner+4)%4)
                removeDummyCard();

            currentPlayer = (currentPlayer + 1) % 4;
        }

        currentPlayer = receiveSignalPlaceNumber();
        cout << "\nPlayer " << currentPlayer+1 << " won the round.\n";
        cout.flush();

        if (declarerOrPartner != 3) {
            cout << "\nYour cards:\n";
            writeCards();
            cout << "\nDummy's cards:\n";
            writeDummyCards();
            cout << endl;
        }

    }

}

void Client::endGame() {
    int bidderScore;
    int playerScore;

    bidderScore = receiveSignalScore();
    if (declarerOrPartner == 2 || declarerOrPartner == 4)
        playerScore = 13 - bidderScore;
    else playerScore = bidderScore;

    cout << "Bidding result was: " << trumpsHeight << " ";
    switch (trumpsSuit) {
        case 1: cout << "CLUB\n"; break;
        case 2: cout << "DIAMOND\n"; break;
        case 3: cout << "HEART\n"; break;
        case 4: cout << "SPADE\n"; break;
        case 5: cout << "NO TRUMPS\n"; break;
    }

    cout << "You won " << playerScore << " tricks.\n";

    if (declarerOrPartner == 2 || declarerOrPartner == 4) {
        if (bidderScore < 6+trumpsHeight)
            cout << "You won!\n";
        else
            cout << "You lose!\n";
    }
    else if (bidderScore >= 6+trumpsHeight)
            cout << "You won!\n";
        else
            cout << "You lose!\n";

    cout.flush();
}
void Client::playingAgain() {
    char wannaPlayChar;
    bool wannaPlay;

    wannaPlayChar = 'x';
    while (wannaPlayChar != 'y' && wannaPlayChar != 'n') {
        cout << "Do you want to play again? (y/n)\n";
        cout.flush();
        cin >> wannaPlayChar;
    }

    if (wannaPlayChar == 'y')
        wannaPlay = true;
    else wannaPlay = false;

    sendSignalPlayAgain(wannaPlay);
    if (!wannaPlay)
        exit(1);
}

void Client::gameManager() {
    waitForGame();

    while (biddingOn == true)
        bidding();

    biddingWon = receiveSignalBiddingEnd();
    cout << "\nBidding is done. ";
    if (trumpsHeight == 0) {
        cout << "Nobody have won.\n";
        gameOn = false;
    }
    else if (biddingWon == false)
        cout << "Opponents have won.\n";
    else cout << "You've won.\n";
    if (trumpsHeight != 0) {
        cout << "Bidding result is: " << trumpsHeight << " ";
        switch (trumpsSuit) {
            case 1: cout << "CLUB\n"; break;
            case 2: cout << "DIAMOND\n"; break;
            case 3: cout << "HEART\n"; break;
            case 4: cout << "SPADE\n"; break;
            case 5: cout << "NO TRUMPS\n"; break;
        }


        cout << endl << "Your cards:\n";
        writeCards();
        if (biddingWon == true && declarerOrPartner == 2) {
            cout << "\nYou're a dummy. Just sit and watch.\n";
            declarerOrPartner = 3;
        }

        if (biddingWon == true && declarerOrPartner == 1) {
            cout << "\nYou're a declarer. Your partner's cards:\n";
            receiveSignalCards(2);
            writeDummyCards();
        }

        if (biddingWon == false)
            declarerOrPartner *= 2;

        currentPlayer = placeNumber - declarerOrPartner + 2;
        if (currentPlayer < 0)
            currentPlayer += 4;

        cout << endl;
        game();

        endGame();
    }

    playingAgain();
}

void Client::removeCard() {
    int cardPosition;

    for (int i=0; i<(int)cards.size(); i++)
        if (cards[i].type == cardType && cards[i].suit == cardSuit+2)
            cardPosition = i;

    cards.erase(cards.begin() + cardPosition);
}
void Client::removeDummyCard() {
    int cardPosition;

    for (int i=0; i<(int)dummyCards.size(); i++)
        if (dummyCards[i].type == cardType && dummyCards[i].suit == cardSuit+2)
            cardPosition = i;

    dummyCards.erase(dummyCards.begin() + cardPosition);
}

void Client::sortCardsSuit() {
    sort(cards.begin(), cards.end(), compareCardsSuit);
}
void Client::sortCardsType() {
    for (int i=0; i<(int)cards.size(); i++)
        switch(cards[i].type) {
            case 'T': cards[i].type = 'B'; break;
            case 'J': cards[i].type = 'C'; break;
            case 'Q': cards[i].type = 'D'; break;
            case 'K': cards[i].type = 'E'; break;
            case 'A': cards[i].type = 'F'; break;
            default: break;
        }

    sort(cards.begin(), cards.end(), compareCardsType);

    for (int i=0; i<(int)cards.size(); i++)
        switch(cards[i].type) {
            case 'B': cards[i].type = 'T'; break;
            case 'C': cards[i].type = 'J'; break;
            case 'D': cards[i].type = 'Q'; break;
            case 'E': cards[i].type = 'K'; break;
            case 'F': cards[i].type = 'A'; break;
            default: break;
        }
}
void Client::sortDummyCardsSuit() {
    sort(dummyCards.begin(), dummyCards.end(), compareCardsSuit);
}
void Client::sortDummyCardsType() {
    for (int i=0; i<(int)dummyCards.size(); i++)
        switch(dummyCards[i].type) {
            case 'T': dummyCards[i].type = 'B'; break;
            case 'J': dummyCards[i].type = 'C'; break;
            case 'Q': dummyCards[i].type = 'D'; break;
            case 'K': dummyCards[i].type = 'E'; break;
            case 'A': dummyCards[i].type = 'F'; break;
            default: break;
        }

    sort(dummyCards.begin(), dummyCards.end(), compareCardsType);

    for (int i=0; i<(int)dummyCards.size(); i++)
        switch(dummyCards[i].type) {
            case 'B': dummyCards[i].type = 'T'; break;
            case 'C': dummyCards[i].type = 'J'; break;
            case 'D': dummyCards[i].type = 'Q'; break;
            case 'E': dummyCards[i].type = 'K'; break;
            case 'F': dummyCards[i].type = 'A'; break;
            default: break;
        }
}

void Client::writeCards() {
    int i = 0;
    cout << (int)cards.size() << endl;
    sortCardsType();
    sortCardsSuit();

    if (i<(int)cards.size() && cards[i].suit == 3) {
        cout << "Hearts (1): ";
        while (i<(int)cards.size() && cards[i].suit == 3) {
            cout << cards[i].type << " ";
            i++;
        }
        cout << endl;
    }
    if (i<(int)cards.size() && cards[i].suit == 4) {
        cout << "Diamonds (2): ";
        while (i<(int)cards.size() && cards[i].suit == 4) {
            cout << cards[i].type << " ";
            i++;
        }
        cout << endl;
    }
    if (i<(int)cards.size() && cards[i].suit == 5) {
        cout << "Clubs (3): ";
        while (i<(int)cards.size() && cards[i].suit == 5) {
            cout << cards[i].type << " ";
            i++;
        }
        cout << endl;
    }
    if (i<(int)cards.size() && cards[i].suit == 6) {
        cout << "Spades (4): ";
        while (i<(int)cards.size() && cards[i].suit == 6) {
            cout << cards[i].type << " ";
            i++;
        }
        cout << endl;
    }

    cout.flush();
}

void Client::writeDummyCards() {
    int i = 0;

    sortDummyCardsType();
    sortDummyCardsSuit();

    if (i<(int)dummyCards.size() && dummyCards[i].suit == 3) {
        cout << "Hearts (1): ";
        while (i<(int)dummyCards.size() && dummyCards[i].suit == 3) {
            cout << dummyCards[i].type << " ";
            i++;
        }
        cout << endl;
    }
    if (i<(int)dummyCards.size() && dummyCards[i].suit == 4) {
        cout << "Diamonds (2): ";
        while (i<(int)dummyCards.size() && dummyCards[i].suit == 4) {
            cout << dummyCards[i].type << " ";
            i++;
        }
        cout << endl;
    }
    if (i<(int)dummyCards.size() && dummyCards[i].suit == 5) {
        cout << "Clubs (3): ";
        while (i<(int)dummyCards.size() && dummyCards[i].suit == 5) {
            cout << dummyCards[i].type << " ";
            i++;
        }
        cout << endl;
    }
    if (i<(int)dummyCards.size() && dummyCards[i].suit == 6) {
        cout << "Spades (4): ";
        while (i<(int)dummyCards.size() && dummyCards[i].suit == 6) {
            cout << dummyCards[i].type << " ";
            i++;
        }
        cout << endl;
    }

    cout.flush();
}
