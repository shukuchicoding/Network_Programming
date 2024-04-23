#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

void proc(char *s, size_t size)
{
    fgets(s, size, stdin);
    size_t length = strlen(s);
    if (length > 0 && s[length - 1] == '\n')
    {
        s[length - 1] = '\0';
    }
}

struct sv_info
{
    char mssv[10];
    char hoten[30];
    char ngaysinh[15];
    double diemtrungbinh;
};

int main(int argc, char *argv[])
{
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = htons(atoi(argv[2]));

    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect() failed");
        return 1;
    }

    int count = 0;

    while (++count)
    {

        struct sv_info sv;

        printf("Nhap thong tin sinh vien %d:\n", count);

        printf("Nhập MSSV: ");
        fgets(sv.mssv, sizeof(sv.mssv), stdin);
        // size_t length = strlen(sv.mssv);
        // if (sv.mssv[length - 1] == '\n')
        // {
        //     sv.mssv[length - 1] = '\0';
        // }

        printf("Nhập họ tên: ");
        fgets(sv.hoten, sizeof(sv.hoten), stdin);
        // length = strlen(sv.hoten);
        // if (sv.hoten[length - 1] == '\n')
        // {
        //     sv.hoten[length - 1] = '\0';
        // }

        printf("Nhập ngày sinh: ");
        fgets(sv.ngaysinh, sizeof(sv.ngaysinh), stdin);
        // length = strlen(sv.ngaysinh);
        // if (sv.ngaysinh[length - 1] == '\n')
        // {
        //     sv.ngaysinh[length - 1] = '\0';
        // }

        printf("Nhập điểm trung bình: ");
        scanf("%lf", &sv.diemtrungbinh);
        getchar(); // Xóa ký tự newline còn lại trong bộ đệm

        send(client, &sv, sizeof(sv), 0);

        if (strcmp(sv.mssv, "") == 0)
            break;
    }

    close(client);

    return 0;
}