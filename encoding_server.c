#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

void encoding(char *input)
{
    int l = strlen(input);
    for (int i = 0; i < l - 1; i++)
    {
        if (input[i] >= 48 && input[i] <= 57)
        {
            input[i] = (105 - input[i]);
        }
        else if (input[i] >= 65 && input[i] <= 90)
        {
            input[i] = input[i] + 1;
            if (input[i] == 91)
                input[i] = 65;
        }
        else if (input[i] >= 97 && input[i] <= 122)
        {
            input[i] = input[i] + 1;
            if (input[i] == 123)
                input[i] = 97;
        }
    }
}

int remove_client(int client, int *client_sockets, char **client_names, int *num_clients) {
    int i = 0;
    for (; i < *num_clients; i++)
        if (client_sockets[i] == client)
            break;
    
    if (i < *num_clients) {
        if (i < *num_clients - 1) {
            client_sockets[i] = client_sockets[*num_clients - 1];
            strcpy(client_names[i], client_names[*num_clients - 1]);
        }

        free(client_names[*num_clients - 1]);
        *num_clients -= 1;
    }
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

    fd_set fdread, fdtest;
    FD_ZERO(&fdread);

    // Gan socket listener vao fdread
    FD_SET(listener, &fdread);

    char buf[256];

    // Mang luu tru cac client da dang nhap
    int client_sockets[FD_SETSIZE];
    char *client_names[FD_SETSIZE];
    int num_clients = 0;

    while (1)
    {
        fdtest = fdread;
        int ret = select(FD_SETSIZE, &fdtest, NULL, NULL, NULL);
        if (ret == -1)
        {
            break;
        }

        for (int i = 0; i < FD_SETSIZE; i++)
            if (FD_ISSET(i, &fdtest))
            {
                if (i == listener)
                {
                    // Co ket noi
                    int client = accept(listener, NULL, NULL);
                    if (client >= FD_SETSIZE)
                    {
                        close(client);
                    }
                    else
                    {
                        num_clients++;
                        FD_SET(client, &fdread);
                        printf("New client connected: %d\n", client);
                        char hello[1024];
                        sprintf(hello, "Xin chao, hien co %d clients dang ket noi.\n", num_clients);
                        send(client, hello, strlen(hello), 0);
                    }
                }
                else
                {
                    // Co du lieu truyen den
                    int client = i;
                    ret = recv(client, buf, sizeof(buf), 0);
                    if (ret <= 0)
                    {
                        close(client);
                        FD_CLR(client, &fdread);
                        remove_client(client, client_sockets, client_names, &num_clients);
                        continue;
                    }
                    else
                    {
                        buf[ret] = 0;
                        printf("Received from %d: %s\n", client, buf);
                        if (strcmp(buf, "exit\n") == 0) {
                            char byebye[] = "Bye bye.\n";
                            send(client, byebye, strlen(byebye), 0);
                            close(client);
                        }
                        encoding(buf);
                        send(client, buf, strlen(buf), 0);
                    }
                }
            }
    }

    return 0;
}