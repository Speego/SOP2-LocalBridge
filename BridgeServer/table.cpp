#include "table.h"

Table::Table(int id) {
    tableReady = false;
    gameOn = false;
    players.reserve(4);
    cards.reserve(52);
    dealer = 0;
    tableID = id;
    getSignalCards();
    getSignalAcceptance();
    getSignalBid();
    getSignalPlaceNumber();
    getSignalBidToSend();
    getSignalScores();
    createSignalBiddingEnd();
    biddingOn = false;
    biddingEnd = false;
    endOn = false;
}

bool Table::getSignalCards() {
    messageCardID = msgget(keyCards, 0600 | IPC_CREAT);
    if (messageCardID < 0) return false;
    return true;
}
void Table::sendSignalDummyCards(int playerNumber, int add) {
    struct messageCard msgc;

    msgc.messageType = players[(playerNumber+add)%4].id;
    for (int i=0; i<(int)players[(biddingWinner+2)%4].cards.size(); i++) {
        msgc.messageCardType = players[(biddingWinner+2)%4].cards[i].type;
        msgc.messageCardSuit = players[(biddingWinner+2)%4].cards[i].suit;
        msgsnd(messageCardID, &msgc, sizeof(char)+2*sizeof(int), 0);
    }
}
void Table::sendSignalCard(int playerID, char cardType, int cardSuit) {
    struct messageCard msgc;

    msgc.messageType = playerID;
    msgc.messageCardType = cardType;
    msgc.messageCardSuit = cardSuit;
    msgsnd(messageCardID, &msgc, sizeof(char)+2*sizeof(int), 0);
}

bool Table::getSignalAcceptance() {
    messageAcceptanceID = msgget(keyAcceptance, 0600 | IPC_CREAT);
    if (messageAcceptanceID < 0) return false;
    return true;
}
void Table::sendSignalAcceptance(int playerID, bool acceptance) {
    struct messageAcceptance msga;

    msga.messageType = (long)playerID;
    msga.messageAcceptance = acceptance;
    msgsnd(messageAcceptanceID, &msga, sizeof(bool), 0);
}

bool Table::getSignalBid() {
    messageBidID = msgget(keyBid, 0600 | IPC_CREAT);
    if (messageBidID < 0) return false;
    return true;
}

bool Table::getSignalPlaceNumber() {
    messagePlaceNumberID = msgget(keyPlaceNumber, 0600 | IPC_CREAT);
    if (messagePlaceNumberID < 0) return false;
    return true;
}
void Table::sendSignalPlaceNumber(int playerID, int place) {
    struct messagePlaceNumber msgpn;

    msgpn.messageType = playerID;
    msgpn.placeNumber = place;
    msgsnd(messagePlaceNumberID, &msgpn, 2*sizeof(int), 0);
}

bool Table::getSignalBidToSend() {
    messageBidToSendID = msgget(keyBidToSend, 0600 | IPC_CREAT);
    if (messageBidToSendID < 0) return false;
    return true;
}
void Table::sendSignalBidToSend(int playerID) {
    struct messageBidToSend msgb;

    msgb.messageType = playerID;
    msgb.trumpsHeight = trumpsHeight;
    msgb.trumpsSuit = trumpsSuit;
    msgsnd(messageBidToSendID, &msgb, 2*sizeof(int), 0);
}

bool Table::createSignalBiddingEnd() {
    messageBiddingEndID = msgget(keyBiddingEnd, 0600 | IPC_CREAT);
    if (messageBiddingEndID < 0) return false;
    return true;
}
void Table::sendSignalBiddingEnd(bool isend) {
    struct messageBiddingEnd msgbe;

    msgbe.isEnd = isend;
    msgbe.player = 0;
    for (int i=0; i<4; i++) {
        msgbe.messageType = players[i].id;
        msgsnd(messageBiddingEndID, &msgbe, sizeof(bool)+sizeof(int), 0);
    }
}
void Table::sendSignalBiddingWon(int playerID, bool won, int playerOrPartner) {
    struct messageBiddingEnd msgbe;

    msgbe.messageType = playerID;
    msgbe.isEnd = won;
    msgbe.player = playerOrPartner;
    msgsnd(messageBiddingEndID, &msgbe, sizeof(bool)+sizeof(int), 0);
}

bool Table::getSignalScores() {
    messageScoresID = msgget(keyScores, 0600 | IPC_CREAT);
    if (messageScoresID < 0) return false;
    return true;
}
void Table::sendSignalScores(int playerID) {
    struct messageScores msgs;

    msgs.messageType = (long)playerID;
    msgs.score = bidderScore;
    msgsnd(messageScoresID, &msgs, 2*sizeof(int), 0);
}

void Table::createCards() {
    char type;

    cards.clear();

    for (int i=3; i<7; i++)
        for (int j=0; j<13; j++) {
            switch (j) {
                case 0: type = '2'; break;
                case 1: type = '3'; break;
                case 2: type = '4'; break;
                case 3: type = '5'; break;
                case 4: type = '6'; break;
                case 5: type = '7'; break;
                case 6: type = '8'; break;
                case 7: type = '9'; break;
                case 8: type = 'T'; break;
                case 9: type = 'J'; break;
                case 10: type = 'Q'; break;
                case 11: type = 'K'; break;
                case 12: type = 'A'; break;
            }

            cards.push_back(*(new Card(type, i)));
        }
}
void Table::writeCards() {
    for (int i=0; i<(int)cards.size(); i++)
        cout << cards[i].type << ";" << cards[i].suit << endl;
}
void Table::randomShuffle() {
    for (int i=(int)cards.size()-1; i>0; i--) {
        swap(cards[i], cards[rand()%i]);
    }
}
void Table::dealCards() {
    for (int i=0; i<4; i++) {
        players[i].cards.clear();

        for (int j=0; j<13; j++)
            players[i].cards.push_back(cards[i*13+j]);
    }
}

void Table::prepareForBidding() {
    biddingOn = true;
    tableReady = false;
    numberOfPlayersResponses = 0;
    vectorIterator = 0;
    player = dealer;
    for (int i=player; i<player+4; i++) {
        sendSignalPlaceNumber(players[i%4].id, i-player);
    }
    sendSignalAcceptance(players[player].id, true);
    trumpsHeight = 0;
    trumpsSuit = 0;
    numberOfPasses = 0;
    sendSignalBiddingEnd(true);
}

void Table::bidding() {
    bool messageBidOK;
    struct messageBid msgb;

    if (msgrcv(messageBidID, &msgb, 2*sizeof(int), players[player].id, IPC_NOWAIT) > -1) {
        if (msgb.trumpsHeight == 0) {
            numberOfPasses++;
            sendSignalAcceptance(players[player].id, true);
            for (int i=1; i<=3; i++)
                sendSignalBidToSend(players[(player+i)%4].id);

            player = (player+1) % 4;

            if (numberOfPasses == 3 && trumpsHeight > 0)
                numberOfPasses = 4;

            messageBidOK = true;
        }
        else if ((msgb.trumpsHeight == trumpsHeight && msgb.trumpsSuit > trumpsSuit && msgb.trumpsSuit < 6) || (msgb.trumpsHeight > trumpsHeight && msgb.trumpsHeight < 8 && msgb.trumpsSuit > 0 && msgb.trumpsSuit < 6)) {
            trumpsHeight = msgb.trumpsHeight;
            trumpsSuit = msgb.trumpsSuit;
            numberOfPasses = 0;
            sendSignalAcceptance(players[player].id, true);
            for (int i=1; i<=3; i++)
                sendSignalBidToSend(players[(player+i)%4].id);

            player = (player+1) % 4;

            if (trumpsHeight == 7 && trumpsSuit == 5) {
                numberOfPasses = 4;
                player = (player+3) % 4;
            }

            messageBidOK = true;
        }
        else {
            sendSignalAcceptance(players[player].id, false);
            messageBidOK = false;
        }

        if (numberOfPasses == 4) {
            biddingOn = false;
            sendSignalBiddingEnd(false);
            sendSignalBiddingWon(players[player].id, true, 1);
            sendSignalBiddingWon(players[(player+2)%4].id, true, 2);
            sendSignalBiddingWon(players[(player+1)%4].id, false, 1);
            sendSignalBiddingWon(players[(player+3)%4].id, false, 2);
            biddingWinner = player;

            if (trumpsHeight > 0)
                biddingEnd = true;
            else endOn = true;

        }
        else if (messageBidOK)
            sendSignalAcceptance(players[player].id, true);

        if (biddingOn && messageBidOK)
            sendSignalBiddingEnd(true);
    }
}

void Table::prepareForGame() {
    gameOn = true;
    biddingEnd = false;
    player = (biddingWinner + 1) % 4;
    sendSignalDummyCards(biddingWinner, 0);
    winSuit = 0;
    winType = '0';
    bidderScore = 0;
    roundWinner = -1;
    cardCounter = 0;
    roundCounter = 0;
    dummyPlaying = false;

    if (trumpsSuit == 3)
        trumpsSuit = 1;
    else if (trumpsSuit == 1)
        trumpsSuit = 3;
}

void Table::game() {
    struct messageCard msgc;
    int isCardOK;

    if (msgrcv(messageCardID, &msgc, sizeof(char)+2*sizeof(int), players[player].id, IPC_NOWAIT) > -1) {
        isCardOK = checkCard(msgc.messageCardType, msgc.messageCardSuit);

        if (isCardOK > -1) {
            sendSignalAcceptance(players[player].id, true);
            cardCounter++;

            if (dummyPlaying == true) {
                player = (player + 2) % 4;
                dummyPlaying = false;
            }

            players[player].removeCard(isCardOK);
            player = (player + 1) % 4;

            if (cardCounter == 4) {
                roundCounter++;
                for (int i=0; i<4; i++)
                    sendSignalPlaceNumber(players[i].id, roundWinner);

                if (roundWinner == biddingWinner || roundWinner == biddingWinner+2)
                    bidderScore++;

                cardCounter = 0;
                player = roundWinner;
            }

            if (cardCounter == 1 && roundCounter == 0) {
                sendSignalDummyCards(biddingWinner, 1);
                sendSignalDummyCards(biddingWinner, 3);
            }

            if (player == (biddingWinner+2) % 4) {
                dummyPlaying = true;
                player = (player + 2) % 4;
            }

            if (roundCounter == 13) {
                gameOn = false;
                endOn = true;
                player = 0;
                for (int i=0; i<4; i++)
                    sendSignalScores(players[i].id);
            }
        }
        else {
            sendSignalAcceptance(players[player].id, false);
        }
    }
}

int Table::endGame() {
    struct messageAcceptance msga;
    bool wannaPlay;
    int playerToRemove = -1;

    if (msgrcv(messageAcceptanceID, &msga, sizeof(bool), players[vectorIterator].id, IPC_NOWAIT) > -1) {
        wannaPlay = msga.messageAcceptance;

        if (wannaPlay)
            vectorIterator++;
        else {
            removePlayer(vectorIterator);
            playerToRemove = players[vectorIterator].id;
        }

        numberOfPlayersResponses++;

        if (numberOfPlayersResponses == 4) {
            if ((int)players.size() > 3)
                tableReady = true;

            endOn = false;
        }
    }

    return playerToRemove;
}

void Table::removePlayer(int playerPosition) {
    players.erase(players.begin() + playerPosition);
}

int Table::checkCard(char cardType, int cardSuit) {
    int cardPosition = -1;

    if (dummyPlaying == true)
        player = (player + 2) % 4;

    if (cardCounter != 0 && cardSuit != firstSuit) {
        for (int i=0; i<(int)players[player].cards.size(); i++)
            if (players[player].cards[i].suit == firstSuit+2) {
                if (dummyPlaying == true)
                    player = (player + 2) % 4;

                return -1;
            }

    }

    for (int i=0; i<(int)players[player].cards.size(); i++)
        if (players[player].cards[i].suit == cardSuit+2 && players[player].cards[i].type == cardType)
            cardPosition = i;

    if (cardPosition == -1) {
        if (dummyPlaying == true)
            player = (player + 2) % 4;

        return -1;
    }

    if (cardCounter == 0) {
        firstSuit = cardSuit;
        roundWinner = player;
        winSuit = cardSuit;
        winType = cardType;
    }
    else if ((winSuit != cardSuit && cardSuit == trumpsSuit) || (winSuit == cardSuit && checkCardType(cardType))) {
            roundWinner = player;
            winSuit = cardSuit;
            winType = cardType;
        }

    if (dummyPlaying == true)
        player = (player + 2) % 4;

    for (int i=1; i<=3; i++) {
        sendSignalCard(players[(player+i)%4].id, cardType, cardSuit);
    }

    return cardPosition;
}

bool Table::checkCardType(char cardType) {
    bool isNewGreater = false;

    switch(cardType) {
            case 'T': cardType = 'B'; break;
            case 'J': cardType = 'C'; break;
            case 'Q': cardType = 'D'; break;
            case 'K': cardType = 'E'; break;
            case 'A': cardType = 'F'; break;
            default: break;
        }
    switch(winType) {
            case 'T': winType = 'B'; break;
            case 'J': winType = 'C'; break;
            case 'Q': winType = 'D'; break;
            case 'K': winType = 'E'; break;
            case 'A': winType = 'F'; break;
            default: break;
        }

    if (cardType > winType)
        isNewGreater = true;

    switch(cardType) {
            case 'B': cardType = 'T'; break;
            case 'C': cardType = 'J'; break;
            case 'D': cardType = 'Q'; break;
            case 'E': cardType = 'K'; break;
            case 'F': cardType = 'A'; break;
            default: break;
        }
    switch(winType) {
            case 'B': winType = 'T'; break;
            case 'C': winType = 'J'; break;
            case 'D': winType = 'Q'; break;
            case 'E': winType = 'K'; break;
            case 'F': winType = 'A'; break;
            default: break;
        }

    return isNewGreater;
}

