#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main(void)
{
    int socket_desc;
    struct sockaddr_in server_addr;
    char server_buffer[2000], client_buffer[2000];

    // clear the buffers
    memset(server_buffer, '\0', sizeof(server_buffer));
    memset(client_buffer, '\0', sizeof(client_buffer));

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("cannot open a socket");
        return -1;
    }
    printf("Created the socket\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // connnect to the server
    if (connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Failed to connect to the server! \n");
        return -1;
    }
    printf("Connected to the server...\n");

    while (1)
    {
        // Get input from the user to send the message
        printf("Enter your message: ");
        // gets(client_buffer); -> depreceated and instead use the below
        fgets(client_buffer, sizeof(client_buffer), stdin);

        if (strcmp(client_buffer, "exit\n") == 0)
        {
            printf("Closing connection\n");
            break;
        }

        // Send the message to the server
        if (send(socket_desc, client_buffer, strlen(client_buffer), 0) < 0)
        {
            printf("Error while sending message to server\n");
            return -1;
        }

        // Receive the message
        memset(server_buffer, '\0', sizeof(server_buffer));
        if (recv(socket_desc, server_buffer, sizeof(server_buffer), 0) < 0)
        {
            printf("Error receiving message from server\n");
            return -1;
        }

        printf("Server response: %s\n", server_buffer);
    }
    close(socket_desc);

    return 0;
}