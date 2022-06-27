#include "goban.h"

int main(void)
{
    int soc;
    char my_stone = 'x';
    char peer_stone = 'o';
    char hostname[64];

    printf("Input server's hostname: ");
    fgets(hostname, HOSTNAME_LENGTH, stdin);
    chop_newline(hostname, HOSTNAME_LENGTH);

    if ((soc = setup_client(hostname, PORT)) == -1)
    {
        exit(1);
    }

    goban_init(soc, my_stone, peer_stone);

    int result = 0;
    while (1)
    {
        if (result != -2) goban_show();
        printf("Go ahead.\n");
        while (1) {
            result = goban_my_turn();
            if (result != -3) break;
            printf("Go again.\n");
        }
        if (result == -1) break;

        if (result != -2) goban_show();
        printf("Wait.\n");
        while (1) {
            result = goban_peer_turn();
            if (result != -3) break;
            printf("Wait again.\n");
        }
        if (result == -1) break;
    }
    
    goban_show();

    goban_destroy();
}