#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void){

    int socket_desc, client_sock, client_size;
    struct sockaddr_in server_addr, client_addr;
    char server_msg[2000], client_msg[2000];

    // clear the buffers
    memset(server_msg, '\0', sizeof(server_msg));
    memset(client_msg, '\0', sizeof(client_msg));

    // create the socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_desc < 0){
        printf("Error while creating socket..\n");
        return -1;
    }
    printf("Socket created successfully !");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Binding failed!");
        return -1;
    }
    printf("Done with binding!");

    //listen for clients
    if(listen(socket_desc, 1) < 0){
        printf("Error while listenting");
        return -1;
    }
    printf("Accepting clients on port 3000....");

    //Accept incoming connections..
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);

    if(client_sock < 0){
        printf("Failed to accept clients.\n");
        return -1;
    }
    printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));


    // Receive the chat message
    // strcpy(server_msg, "Message received!\n");

    // if(send(client_sock, server_msg, strlen(server_msg), 0) < 0){
    //     printf("failed to send message\n");
    //     return -1;
    // }
    // printf("Message sent to client...");

    while (1) {
        memset(client_msg, '\0', sizeof(client_msg));
        int bytes_received = recv(client_sock, client_msg, sizeof(client_msg), 0);
        // Receive message from client
        if (bytes_received <= 0) {
            printf("Client disconnected\n");
            break;  // Exit loop if there's an error
        }

        printf("Client: %s\n", client_msg);

        // Send a response back
        strcpy(server_msg, "Message received!");
        if (send(client_sock, server_msg, strlen(server_msg), 0) < 0) {
            printf("Failed to send message\n");
            break;
        }
    }

    close(client_sock);
    close(socket_desc);


    return 0;
}