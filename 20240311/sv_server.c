#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

struct sv_info {
    char mssv[10];
    char hoten[30];
    char ngaysinh[15];
    double diemtrungbinh;
};

int main() {
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
    addr.sin_port = htons(9000);

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

    struct sv_info sv;

    int count = 0;

    printf("Ket noi thanh cong!\n");

    while(++count) {
        int ret = recv(client, &sv, sizeof(sv), 0);
        if (ret <= 0)
            break;
        printf("SV %d: %d bytes:\nMSSV: %s\nHo va ten: %s\nNgay sinh: %s\nDiem trung binh: %.2lf\n\n", count, ret, sv.mssv, sv.hoten, sv.ngaysinh, sv.diemtrungbinh);
    }

    close(client);
    close(listener);

    return 0;
}