#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

void get_time(char *MSG, char *FORMAT)
{
}

int main()
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
    addr.sin_port = htons(9000);

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

    int num_processes = 8;

    for (int i = 0; i < num_processes; i++)
    {
        if (fork() == 0)
        {
            while (1)
            {
                // Cho ket noi
                int client = accept(listener, NULL, NULL);
                printf("New client accepted: %d\n", client);
                while (1)
                {
                    // Cho du lieu tu client
                    char buf[256];
                    int ret = recv(client, buf, sizeof(buf), 0);
                    if (ret <= 0)
                        continue;
                    printf("Received from %d: %s", client, buf);
                    buf[ret] = 0;
                    // Xu ly du lieu
                    char cmd[20], format[20], tmp[50];
                    int n = sscanf(buf, "%s %s %s", cmd, format, tmp);

                    if (n != 2 || strcmp(cmd, "GET_TIME") != 0)
                    {
                        char *msg = "Sai cu phap. Hay nhap lai.\n";
                        send(client, msg, strlen(msg), 0);
                    }
                    else
                    {
                        if (strcmp(format, "dd/mm/yyyy\0\n") == 0)
                        {
                            time_t now = time(NULL);
                            struct tm *local_time = localtime(&now);
                            char time_stamp[20];
                            strftime(time_stamp, sizeof(time_stamp), "%d/%m/%Y\n", local_time);
                            send(client, time_stamp, strlen(time_stamp), 0);

                        }
                        else if (strcmp(format, "dd/mm/yy\0\n") == 0)
                        {
                            time_t now = time(NULL);
                            struct tm *local_time = localtime(&now);
                            char time_stamp[20];
                            strftime(time_stamp, sizeof(time_stamp), "%d/%m/%y\n", local_time);
                            send(client, time_stamp, strlen(time_stamp), 0);
                        }
                        else if (strcmp(format, "mm/dd/yyyy\0\n") == 0)
                        {
                            time_t now = time(NULL);
                            struct tm *local_time = localtime(&now);
                            char time_stamp[20];
                            strftime(time_stamp, sizeof(time_stamp), "%m/%d/%Y\n", local_time);
                            send(client, time_stamp, strlen(time_stamp), 0);
                        }
                        else if (strcmp(format, "mm/dd/yy\0\n") == 0)
                        {
                            time_t now = time(NULL);
                            struct tm *local_time = localtime(&now);
                            char time_stamp[20];
                            strftime(time_stamp, sizeof(time_stamp), "%m/%d/%y\n", local_time);
                            send(client, time_stamp, strlen(time_stamp), 0);
                        }
                        else
                        {
                            char *msg = "Khong ho tro dinh dang nay, hay nhap lai.\n";
                            send(client, msg, strlen(msg), 0);
                        }
                    }
                }
                // Đóng kết nối
                close(client);
            }
            exit(0);
        }
    }
    getchar();
    killpg(0, SIGKILL);
    // wait(NULL);

    return 0;
}