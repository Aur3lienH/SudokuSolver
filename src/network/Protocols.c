#include "network/Protocols.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 4444

const char* SERVER_IP = "141.94.16.58";

int socket_init()
{
    int socket_desc;
    struct sockaddr_in server;
    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    //Connect to remote server
    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");
    return socket_desc;

}

void disconnect(int socket_desc)
{
    close(socket_desc);
}


int Net_Send(void *data, size_t len)
{
    int socket_desc = socket_init();
    size_t total = 0;
    while(total < len)
    {
        size_t toSend = len - total;
        int sent = send(socket_desc, data + total, toSend, 0);
        if(sent < 0)
        {
            perror("Send failed");
            return 1;
        }
        total += sent;
    }

    disconnect(socket_desc);
}