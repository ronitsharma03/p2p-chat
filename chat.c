#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

typedef struct {
    int sockfd;
} ThreadArgs;

// Function to receive messages
void *receive_messages(void *arg) {
    int sockfd = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        int bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("\nPeer disconnected. Exiting...\n");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        buffer[bytes_received] = '\0';
        printf("\nPeer: %sYou: ", buffer);
        fflush(stdout);
    }
    return NULL;
}

// Function to send messages
void *send_messages(void *arg) {
    int sockfd = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        printf("You: ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            perror("Error reading input");
            break;
        }
        if (send(sockfd, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            break;
        }
    }
    return NULL;
}

int main() {
    int listen_fd, conn_fd, peer_fd;
    struct sockaddr_in my_addr, peer_addr;
    pthread_t recv_thread, send_thread;
    socklen_t addr_len = sizeof(peer_addr);

    int my_port, peer_port;
    char peer_ip[16];

    printf("Enter your listening port: ");
    scanf("%d", &my_port);
    printf("Enter peer's IP address: ");
    scanf("%s", peer_ip);
    printf("Enter peer's port: ");
    scanf("%d", &peer_port);
    getchar(); // Consume newline

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(my_port);

    if (bind(listen_fd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    listen(listen_fd, 1);

    if ((peer_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Peer socket creation failed");
        exit(EXIT_FAILURE);
    }

    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = inet_addr(peer_ip);
    peer_addr.sin_port = htons(peer_port);

    printf("Trying to connect to peer...\n");
    if (connect(peer_fd, (struct sockaddr *)&peer_addr, sizeof(peer_addr)) < 0) {
        printf("Connection failed. Waiting for incoming connection...\n");
        conn_fd = accept(listen_fd, (struct sockaddr *)&peer_addr, &addr_len);
        if (conn_fd < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Peer connected.\n");
    } else {
        printf("Connected to peer.\n");
        conn_fd = peer_fd;
    }

    pthread_create(&recv_thread, NULL, receive_messages, (void *)&conn_fd);
    pthread_create(&send_thread, NULL, send_messages, (void *)&conn_fd);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);

    close(conn_fd);
    close(listen_fd);
    return 0;
}
