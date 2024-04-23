#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

struct sv_info
{
    char mssv[10];
    char hoten[30];
    char ngaysinh[15];
    double diemtrungbinh;
};

void insert_log_data(FILE *f, char *ip_address, struct sv_info sv, int count)
{
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    char time_stamp[20];

    printf("SV %d: %s %s %s %.2lf\n", count, sv.mssv, sv.hoten, sv.ngaysinh, sv.diemtrungbinh);

    // Chuyen dinh dang xau thoi gian
    strftime(time_stamp, sizeof(time_stamp), "%Y-%m-%d %H:%M:%S", local_time);

    fprintf(f, "%s %s %s %s %s %.2lf\n", ip_address, time_stamp, sv.mssv, sv.hoten, sv.ngaysinh, sv.diemtrungbinh);
}

int main(int argc, char *argv[])
{
    // Tao socket cho ket noi
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }

    // Khai bao dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));

    // Gan socket voi cau truc dia chi
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)))
    {
        perror("bind() failed");
        return 1;
    }

    // Chuyen socket sang trang thai cho ket noi
    if (listen(listener, 5))
    {
        perror("listen() failed");
        return 1;
    }

    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    int client = accept(listener, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (client == -1)
    {
        perror("accept() failed");
        return 1;
    }

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
    struct sv_info sv;

    int count = 0;

    printf("Ket noi thanh cong!\n");

    FILE *f = fopen(argv[2], "a");

    while (++count)
    {
        int ret = recv(client, &sv, sizeof(sv), 0);
        if (ret <= 0)
            break;
        insert_log_data(f, clientIP, sv, count);
    }

    close(client);
    close(listener);

    return 0;
}