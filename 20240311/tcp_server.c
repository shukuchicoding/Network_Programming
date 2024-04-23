#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // Tao socket cho ket noi
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1) {
        perror("socket() failed");
        return 1;
    }

    // Khai bao dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    // Gan socket voi cau truc dia chi
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("bind() failed");
        return 1;
    }

    // Chuyen socket sang trang thai cho ket noi
    if (listen(listener, 5)) {
        perror("listen() failed");
        return 1;
    }

    int client = accept(listener, NULL, NULL);

    // Gui loi chao den client
    FILE *f = fopen("hello_client.txt", "rb");
    char send_to_client[2048];
    
    while (1){
        int ret = fread(send_to_client, 1, sizeof(send_to_client), f);
        if (ret <= 0) break;
        send(client, send_to_client, ret, 0);
    }

    // Luu du lieu client gui den
    f = fopen("server_recv.txt", "wb");
    char buf[2048];

    while(1) {
        int ret = recv(client, buf, sizeof(buf), 0); //so bytes doc duoc
        if (ret <= 0) break;
        fwrite(buf, 1, ret, f);
    }

    fclose(f);
    
    close(client);
    close(listener);

    return 0;
}