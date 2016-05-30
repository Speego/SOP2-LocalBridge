#include "server.h"
#include "player.h"

int main()
{
    srand(time(NULL));
    Server *s = new Server();

    s->setServer();
    while (1) {
        s->manageGames();
    }

    delete s;

    return 0;
}
