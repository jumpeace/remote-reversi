// 3目並べの後手(サーバー)プログラム
#include "goban.h"

int main(void) {
    int soc;
    char my_stone = 'o';
    char peer_stone = 'x';

    // 接続受付完了まで
    if ((soc = setup_server(PORT)) == -1) {
        exit(1);
    }

    // 基盤の初期化
    goban_init(soc, my_stone, peer_stone);

    int result = 0;
    while (1)
    {
        if (result != -2) goban_show();
        printf("Wait.\n");
        while (1) {
            result = goban_peer_turn();
            if (result != -3) break;
            printf("Wait again.\n");
        }
        if (result == -1) break;

        if (result != -2) goban_show();
        printf("Go ahead.\n");
        while (1) {
            result = goban_my_turn();
            if (result != -3) break;
            printf("Go again.\n");
        }
        if (result == -1) break;
    }
    goban_show();

    goban_destroy();
}