#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// 戻り値 待受ソケットのディスクリプタ
// port 接続先のポート番号
// backlog 待ち受け数
int mserver_socket(in_port_t port, int backlog)
{
    // 変数の宣言
    struct sockaddr_in me;
    int soc_waiting;

    // 自分のアドレスをsockaddr_in 構造体に設定
    memset((char *)&me, 0, sizeof(me));
    me.sin_family = AF_INET;
    me.sin_addr.s_addr = htonl(INADDR_ANY);
    me.sin_port - htons(port);

    // IPv4でストリーム型のソケットの作成
    if((soc_waiting = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return -1;
    }

    // ソケットに自分のアドレスを設定
    if(bind(soc_waiting, (struct sockaddr *)&me, sizeof(me)) == -1)
    {
        perror("bind");
        return -1;
    }

    // ソケットで接続待ちの設定
    if(listen(soc_waiting, backlog) == -1){
        perror("listen");
        return -1;
    }

    fprintf(stderr, "successfully setup, now waiting.\n");

    // 待ち受け用ディスクリプタを返す
    return soc_waiting;
}

// 戻り値: 最大のソケットのディスクリプタ
// soc_waiting: 待ち受けに使うソケット
// limit: acceptの上限
// func: acceptごとに実行する関数へのポインタ
int mserver_maccept(int soc_waiting, int limit, void (*func)() )
{
    int fd; // ディスクリプタを入れる作業用変数

    // accept を limit回実行
    for (int i = 0; i < limit; i++)
    {
        if ((fd = accept(soc_waiting, NULL, NULL)) == -1)
        {
            fprintf(stderr, "accept errer\n");
            return -1;
        }
        (*func)(i, fd);
    }
    
    // 待ち受けソケットを閉じる
    close(soc_waiting);
    
    // 最後のfd(=最大値)を返す
    return fd;
}