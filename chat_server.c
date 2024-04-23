#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

#define MAX_FDS 2048

// struct Client
// {
//     int id;
//     char name[30];
// };

char name[MAX_FDS][30];


void removeClient(struct pollfd fds[], int *nfds, int index)
{
    if (index < *nfds - 1)
        fds[index] = fds[*nfds - 1];
    (*nfds)--;
}

int main()
{
    // Tao socket cho ket noi
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Tao ket noi TCP/IP
    if (listener == -1)
    {
        perror("socket() failed");
        return 1;
    }
    // Khai bao dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(5678);
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

    struct pollfd fds[MAX_FDS]; // Tap cac clients
    int nfds = 0;               // So luong client
    // Gan socket listener vao tap tham do
    fds[0].fd = listener;
    fds[0].events = POLLIN;
    nfds++;

    char buf[256];

    while (1)
    {
        int ret = poll(fds, nfds, -1);
        if (ret == -1)
        {
            break;
        }
        if (ret == 0)
        {
            printf("Time out...\n");
            continue;
        }
        for (int i = 0; i < nfds; i++)
        {
            if (fds[i].revents & POLLIN)
            {
                int client;
                if (fds[i].fd == listener)
                {
                    // Co ket noi moi
                    // struct Client client;
                    client = accept(listener, NULL, NULL);
                    if (client >= MAX_FDS)
                    {
                        // Vuot qua so ket noi toi da
                        close(client);
                    }
                    else
                    {
                        // Them vao mang do su kien
                        fds[nfds].fd = client;
                        fds[nfds].events = POLLIN;
                        nfds++;
                        printf("New client connected: %d\n", client);
                        // Gui loi chao den client
                        char hi[1024];
                        sprintf(hi, "Hello client, your client_id is %d.\nPlease enter your name follow the syntax (no space in client_name):\n\"client_id: client_name\".\n", client);
                        send(client, hi, strlen(hi), 0);
                        int flag = 0;
                        do
                        {
                            char info[50];
                            int info_ret = recv(client, info, sizeof(info), 0);
                            info[info_ret - 1] = 0;
                            char tmp[20];
                            sprintf(tmp, "%d: ", client);
                            char *p = strstr(info, tmp);
                            if (p == NULL || *p != tmp[0])
                            {
                                char retry[] = "Your information is invalid. Please try again.\n";
                                send(client, retry, strlen(retry), 0);
                                flag = 1;
                            }
                            else
                            {
                                p += strlen(tmp);
                                // sprintf(client.name, "%s", p);
                                sprintf(name[client], "%s", p);

                                flag = 0;
                                for (int i = 0; i < strlen(p); i++)
                                {
                                    if (*(p + i) == 32)
                                    {
                                        char retry[] = "Your information is invalid. Please try again.\n";
                                        send(client, retry, strlen(retry), 0);
                                        flag = 1;
                                        break;
                                    }
                                }
                            }
                        } while (flag);
                    }
                }
                else
                {
                    // Co du lieu tu client truyen den
                    int client;
                    client = fds[i].fd;
                    ret = recv(client, buf, sizeof(buf), 0);
                    if (ret <= 0)
                    {
                        // Ket noi bi dong
                        printf("Client %d disconnected\n", fds[i].fd);
                        // Xoa khoi tap clients
                        removeClient(fds, &nfds, i);
                        i--;
                        continue;
                    }
                    else
                    {
                        // Xu ly du lieu
                        buf[ret] = 0;
                        // Them dau thoi gian
                        time_t now = time(NULL);
                        struct tm *local_time = localtime(&now);
                        char time_stamp[20];
                        // Chat_log
                        strftime(time_stamp, sizeof(time_stamp), "%Y-%m-%d %H:%M:%S", local_time);

                        FILE *f = fopen("chat_log.txt", "a");
                        fprintf(f, "%s %s: %s\n", time_stamp, name[client], buf);
                        fclose(f);

                        // Du lieu tong hop: thoi gian, nguoi gui, noi dung
                        char data[1024];
                        sprintf(data, "%s: %s\n", name[client], buf);
                        for (int j = 0; j < nfds; j++)
                        {
                            if (fds[j].fd != listener)
                            {
                                send(fds[j].fd, data, strlen(data), 0);
                            }
                        }
                    }
                }
            }
        }
    }
    close(listener);
    return 1;
}
