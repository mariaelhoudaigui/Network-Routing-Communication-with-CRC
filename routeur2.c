#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>



#define MAX 1024

#define ROUTER2_PORT 9094

#define SERVER_PORT 8080



// Fonction pour créer et initialiser le socket

int create_socket(int port) {

    int sockfd;

    struct sockaddr_in addr;



    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {

        perror("Erreur de création du socket");

        exit(EXIT_FAILURE);

    }



    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;

    addr.sin_port = htons(port);

    addr.sin_addr.s_addr = htonl(INADDR_ANY);



    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {

        perror("Erreur de bind");

        close(sockfd);

        exit(EXIT_FAILURE);

    }



    return sockfd;

}



// Fonction pour transférer les données entre le client et le serveur

void transfer_data(int router2_sockfd, int server_sockfd) {

    char buffer[MAX];

    int n;



    // Lecture des données envoyées par le premier routeur

    while ((n = read(router2_sockfd, buffer, sizeof(buffer))) > 0) {

        printf("Message du routeur: %s\n", buffer);



        // Envoi des données au serveur

        write(server_sockfd, buffer, n);



        // Lecture de la réponse du serveur

        n = read(server_sockfd, buffer, sizeof(buffer));

        if (n > 0) {

            printf("Réponse du serveur: %s\n", buffer);

            // Envoi de la réponse au premier routeur

            write(router2_sockfd, buffer, n);

        }

    }

}



// Fonction principale

int main() {

    int router2_sockfd, server_sockfd;

    struct sockaddr_in server_addr;



    // Création du socket du deuxième routeur pour écouter les connexions du premier routeur

    router2_sockfd = create_socket(ROUTER2_PORT);

    if (listen(router2_sockfd, 5) < 0) {

        perror("Erreur d'écoute");

        close(router2_sockfd);

        exit(EXIT_FAILURE);

    }

    printf("Routeur en écoute sur le port %d...\n", ROUTER2_PORT);



    // Acceptation de la connexion du premier routeur

    int client_sockfd = accept(router2_sockfd, NULL, NULL);

    if (client_sockfd < 0) {

        perror("Erreur d'acceptation de connexion du routeur");

        close(router2_sockfd);

        exit(EXIT_FAILURE);

    }

    printf("Connecté au routeur...\n");



    // Création du socket pour se connecter au serveur

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sockfd == -1) {

        perror("Erreur de création du socket pour le serveur");

        close(router2_sockfd);

        close(client_sockfd);

        exit(EXIT_FAILURE);

    }



    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(SERVER_PORT);

    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Adresse du serveur



    if (connect(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {

        perror("Erreur de connexion au serveur");

        close(router2_sockfd);

        close(client_sockfd);

        close(server_sockfd);

        exit(EXIT_FAILURE);

    }

    printf("Connecté au serveur...\n");



    // Transfert des données entre le premier routeur et le serveur via le deuxième routeur

    transfer_data(client_sockfd, server_sockfd);



    // Fermeture des connexions

    close(client_sockfd);

    close(server_sockfd);

    close(router2_sockfd);



    return 0;

}