#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// 受信関数
int receive() {

    // socket作成
    // socketは通信の出入り口 ここを通してデータのやり取りをする
    // socket(アドレスファミリ, ソケットタイプ, プロトコル)
    int sock;
    sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？
    printf("sock = %d\n", sock);

    // アドレス等格納
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;  //IPv4
    addr.sin_port = htons(9);   //通信ポート番号設定
    //addr.sin_addr.S_un.S_addr = INADDR_ANY; // INADDR_ANYはすべてのアドレスからのパケットを受信する
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANYはすべてのアドレスからのパケットを受信する

    // バインド
    // アドレス等の情報をsocketに登録する
    int res_bind = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    printf("res_bind = %d\n", res_bind);

    // バッファ ここに受信したデータが入る サイズは自由に決められるが、char配列
    char buf[2048];
    memset(buf, 0, sizeof(buf)); // 複数回受信するときは前回値が残らないようにこのように バッファを0でクリアするのが一般的 メモリー操作関数

    // 受信
    // recv(ソケット, 受信するデータの格納先, データのバイト数, フラグ);
    // バインドしていない場合は recvfrom(sock, buf, 5, 0, (struct sockaddr *)&addr, sizeof(addr)); でもOK？
    printf("Waiting...\n");
    int res_recv = recv(sock, buf, sizeof(buf), 0);
    printf("res_recv = %d\n", res_recv);

    // 出力
    printf("%s\n", buf);

    // socketの破棄
    close(sock);

    return 0;
}

//送信関数
int send() {

    // socket作成
    // socketは通信の出入り口 ここを通してデータのやり取りをする
    // socket(アドレスファミリ, ソケットタイプ, プロトコル)
    int sock;
    sock = socket(AF_INET, SOCK_DGRAM, 0);  //AF_INETはIPv4、SOCK_DGRAMはUDP通信、0は？
    printf("sock = %d\n", sock);

    // アドレス等格納
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;  //IPv4
    addr.sin_port = htons(9);   //通信ポート番号設定
//    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); // 送信アドレスを127.0.0.1に設定
    //addr.sin_addr.s_addr = inet_addr("255.255.255.255"); // 送信アドレスを127.0.0.1に設定
    addr.sin_addr.s_addr = inet_addr("192.0.2.255"); // 送信アドレスを127.0.0.1に設定

    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&yes, sizeof(yes));

    // バッファ ここに送信するデータを入れる  サイズは自由に決められるが、char配列
    char buf[2048] = "HELLO";
    
    // 送信
    // sendto(ソケット, 送信するデータ, データのバイト数, フラグ, アドレス情報, アドレス情報のサイズ);
    // 送信するデータに直接文字列 "HELLO" 等を入れることもできる
    // バインドしている場合は send(sock, buf, 5, 0); でもOK？
    int res_sendto = sendto(sock, buf,  sizeof(buf), 0, (struct sockaddr *)&addr, sizeof(addr));
    printf("res_sendto = %d\n", res_sendto);

    // socketの破棄
    close(sock);

    return 0;
}

int main(int argc, char** argv) {
    std::string arg(argv[1]);
    if(arg == "send")
    {
        printf("Execute sender\n");
        send();
    }
    else if(arg == "recv")
    {
        printf("Execute receiver\n");
        receive();
    }
    std::cout << "Finished\n";
}
