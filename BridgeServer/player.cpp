#include "player.h"

bool compareCardsSuit(Card x, Card y) {
    return x.suit < y.suit;
}
bool compareCardsType(Card x, Card y) {
    return x.type < y.type;
}

Player::Player(int playerID, string playerName, int tableNumber) {
    id = playerID;
    name = playerName;
    table = tableNumber;
    cards.reserve(13);
    getSignalCards();
}

bool Player::getSignalCards() {
    messageCardID = msgget(keyCards, 0600 | IPC_CREAT);
    if (messageCardID < 0) return false;
    return true;
}
void Player::sendSignalCards() {
    struct messageCard msgc;

    msgc.messageType = id;
    for (int i=0; i<(int)cards.size(); i++) {
        msgc.messageCardType = cards[i].type;
        msgc.messageCardSuit = cards[i].suit;
        msgsnd(messageCardID, &msgc, sizeof(char)+2*sizeof(int), 0);
    }

}

void Player::removeCard(int cardPosition) {
    cards.erase(cards.begin() + cardPosition);
}

void Player::sortCardsSuit() {
    sort(cards.begin(), cards.end(), compareCardsSuit);
}
void Player::sortCardsType() {
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

void Player::writeCards() {
    int i = 0;

    sortCardsType();
    sortCardsSuit();

    if (i<(int)cards.size() && cards[i].suit == 3) {
        cout << "Hearts: ";
        while (i<(int)cards.size() && cards[i].suit == 3) {
            cout << cards[i].type << ", ";
            i++;
        }
        cout << endl;
    }
    if (i<(int)cards.size() && cards[i].suit == 4) {
        cout << "Diamonds: ";
        while (i<(int)cards.size() && cards[i].suit == 4) {
            cout << cards[i].type << ", ";
            i++;
        }
        cout << endl;
    }
    if (i<(int)cards.size() && cards[i].suit == 5) {
        cout << "Clubs: ";
        while (i<(int)cards.size() && cards[i].suit == 5) {
            cout << cards[i].type << ", ";
            i++;
        }
        cout << endl;
    }
    if (i<(int)cards.size() && cards[i].suit == 6) {
        cout << "Spades: ";
        while (i<(int)cards.size() && cards[i].suit == 6) {
            cout << cards[i].type << ", ";
            i++;
        }
        cout << endl;
    }

    cout.flush();
}
