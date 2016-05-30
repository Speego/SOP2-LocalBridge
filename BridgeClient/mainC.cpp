#include "client.h"

using namespace std;

int main()
{
    Client *c = new Client();
    c->prepareToGame();
    while (1) {
        c->gameManager();
    }
    return 0;
}
