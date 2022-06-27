#include "goban.h"
#define SEND_GOBAN_DATA_USIZE 3
#define SEND_GOBAN_DATA_NUM_MAX 10
#define SEND_GOBAN_DATA_SIZE SEND_GOBAN_DATA_USIZE * SEND_GOBAN_DATA_NUM_MAX

static char goban_my_stone;
static char goban_peer_stone;
static char goban_plane[6][7] = {"+1234+", "a....|", "b....|", "c....|", "d....|", "+----+"};

static int goban_placed[SEND_GOBAN_DATA_NUM_MAX][2];
static char goban_placed_stone[3];
static int goban_placed_num;

static int goban_soc;

static int is_again;
static int is_pass;
static int is_fin;

void goban_init(int soc, char my_stone, char peer_stone)
{
    goban_soc = soc;
    goban_my_stone = my_stone;
    goban_peer_stone = peer_stone;
    goban_plane[2][2] = 'o';
    goban_plane[2][3] = 'x';
    goban_plane[3][2] = 'x';
    goban_plane[3][3] = 'o';
    goban_placed_num = 0;
    is_again = 0;
    is_pass = 0;
    is_fin = 0;
}

void goban_show()
{
    printf("\n");
    for (int i = 0; i < 6; i++)
        printf("%s\n", goban_plane[i]);
    if (is_fin) {
        int goban_my_stone_num = 0, goban_peer_stone_num = 0;
        char win_lose_val;
        for (int y = 1; y <= 4; y++) {
            for (int x = 1; x <= 4; x++) {
                if (goban_plane[y][x] == goban_my_stone) goban_my_stone_num++;
                if (goban_plane[y][x] == goban_peer_stone) goban_peer_stone_num++;
            }
        }
        win_lose_val = goban_my_stone_num == goban_peer_stone_num ? 0 : (goban_my_stone_num > goban_peer_stone_num ? 1 : -1);
        printf("<Final Result>\n");
        printf("%c:%d, %c:%d\n", 
            goban_my_stone, goban_my_stone_num, goban_peer_stone, goban_peer_stone_num);
        switch (win_lose_val) {
        case 1:
            printf("You win.\n");
            break;
        case 0:
            printf("You draw.\n");
            break;
        case -1:
            printf("You lose.\n");
            break;
        }
    }
}

void goban_place_record(int x, int y, char stone) {
    goban_placed[goban_placed_num][0] = x;
    goban_placed[goban_placed_num][1] = y;
    goban_placed_stone[goban_placed_num] = stone;
    goban_placed_num++;
}

int goban_is_place(int x, int y, char my_stone, char peer_stone, int is_ref) {
    if (goban_plane[y][x] != '.') return 0;
    int is_place = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (x + i < 0 || x + i > 4) break;
            if (y + j < 0 || y + j > 4) break;
            if (goban_plane[y + j][x + i] == peer_stone) {
                int i2 = i * 2, j2 = j * 2;
                if (x + i2 < 0 || x + i2 > 4) break;
                if (y + j2 < 0 || y + j2 > 4) break;
                if (goban_plane[y + j2][x + i2] == peer_stone) {
                    int i3 = i * 3, j3 = j * 3;
                    if (x + i3 < 0 || x + i3 > 4) break;
                    if (y + j3 < 0 || y + j3 > 4) break;
                    if (goban_plane[y + j3][x + i3] == my_stone) {
                        is_place = 1;
                        if (is_ref == 1) {
                            goban_plane[y + j][x + i] = my_stone;
                            goban_place_record(x + i, y + j, my_stone);
                            goban_plane[y + j2][x + i2] = my_stone;
                            goban_place_record(x + i2, y + j2, my_stone);
                        } 
                    } 
                }
                else if (goban_plane[y + j2][x + i2] == my_stone) {
                    is_place = 1;
                    if (is_ref == 1) {
                        goban_plane[y + j][x + i] = my_stone;
                        goban_place_record(x + i, y + j, my_stone);
                    } 
                }
            }
        }
    }
    return is_place;
}

int goban_is_pass(char my_stone, char peer_stone) {
    for (int y = 1; y <= 4; y++) {
        for (int x = 1; x <= 4; x++) {
            if (goban_is_place(x, y, goban_my_stone, goban_peer_stone, 0) == 1) {
                return 0;
            }
        }
    }
    return 1;
}

// 相手の番の処理
int goban_peer_turn()
{
    char data[SEND_GOBAN_DATA_SIZE + 5];
    int x, y;

    // ソケットから読む
    read(goban_soc, data, SEND_GOBAN_DATA_SIZE + 5);

    // 先頭バイトを見て終了か判定
    if (data[0] == 'q')
        return -1;

    is_again = data[0] - '0';
    is_pass = data[1] - '0';
    is_fin = data[2] - '0';
    int i = 0;
    for (int i = 0; i * SEND_GOBAN_DATA_USIZE + 3 <= SEND_GOBAN_DATA_SIZE + 2 && data[i * SEND_GOBAN_DATA_USIZE + 3] != '-'; i++) 
        goban_plane[data[i * SEND_GOBAN_DATA_USIZE + 4] - '0'][data[i * SEND_GOBAN_DATA_USIZE + 3] - '0'] = data[i * SEND_GOBAN_DATA_USIZE + 5];

    return is_fin == 1 ? -1 : (is_pass == 1 ? -2 : (is_again == 1) ? -3 : 1);
}

// 自分の番の処理
int goban_my_turn()
{
    char input[10];
    char data[SEND_GOBAN_DATA_SIZE + 5];
    int x, y;

    printf("my_goban: %c\n", goban_my_stone);
    if (!is_fin) {
        if (goban_is_pass(goban_my_stone, goban_peer_stone)) {
            is_pass = 1;
            if (goban_is_pass(goban_peer_stone, goban_my_stone)) {
                is_fin = 1;
            } else
                printf("You cannot place.");
        }
        else  {
            is_pass = 0;

            printf("You can place: ");
            for (int y = 1; y <= 4; y++) {
                for (int x = 1; x <= 4; x++) {
                    if (goban_is_place(x, y, goban_my_stone, goban_peer_stone, 0))
                        printf("%c%d ", y + 'a' - 1, x);
                }
            }
            printf("\n");

            // キーボード入力
            fgets(input, 10, stdin);
            if (input[0] == 'q')
            {
                write(goban_soc, input, 1);
                return -1;
            }
            if ((input[0] < 'a' || input[0] > 'd') || (input[1] < '1' || input[1] > '4'))
                return -3;

            y = (int)input[0] - (int)'a' + 1;
            x = (int)input[1] - (int)'0';

            goban_placed_num = 0;

            int is_place = goban_is_place(x, y, goban_my_stone, goban_peer_stone, 1);
            if (is_place == 1) {
                goban_plane[y][x] = goban_my_stone;
                goban_place_record(x, y, goban_my_stone);
                is_again = 0;
            }
            else is_again = 1;
        }
    }

    data[0] = is_again + '0';
    data[1] = is_pass + '0';
    data[2] = is_fin + '0';
    for (int i = 0; i < SEND_GOBAN_DATA_NUM_MAX; i++) {
        if (i < goban_placed_num) {
            data[i * SEND_GOBAN_DATA_USIZE + 3] = goban_placed[i][0] + '0';
            data[i * SEND_GOBAN_DATA_USIZE + 4] = goban_placed[i][1] + '0';
            data[i * SEND_GOBAN_DATA_USIZE + 5] = goban_placed_stone[i];
        } else {
            data[i * SEND_GOBAN_DATA_USIZE + 3] = '-';
            data[i * SEND_GOBAN_DATA_USIZE + 4] = '-';
            data[i * SEND_GOBAN_DATA_USIZE + 5] = '-';
        }
    }
    data[SEND_GOBAN_DATA_SIZE + 3] = '-';
    // 送信
    write(goban_soc, data, SEND_GOBAN_DATA_SIZE + 5);

    return is_fin == 1 ? -1 : (is_pass == 1 ? -2 : (is_again == 1) ? -3 : 1);
}

// 終了処理
void goban_destroy()
{
    close(goban_soc);
}