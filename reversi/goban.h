#include <sys/types.h>
#include <stdlib.h>
#include "mylib.h"

#define PORT (in_port_t)50000
#define HOSTNAME_LENGTH 64

extern void goban_init(int soc, char my_stone, char peer_stone);
extern void goban_show();
extern void  goban_place_record(int, int, char);
extern int  goban_is_place(int, int, char, char, int);
extern int  goban_is_pass(char, char);
extern int  goban_peer_turn();
extern int  goban_my_turn();
extern void goban_destroy();