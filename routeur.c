#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>



#define MAX 1024

#define CLIENT_PORT 9090

#define ROUTER2_PORT 9091 // Port pour le deuxième routeur



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

void transfer_data(int client_sockfd, int router2_sockfd) {

    char buffer[MAX];

    int n;



    // Lecture des données envoyées par le client

    while ((n = read(client_sockfd, buffer, sizeof(buffer))) > 0) {

        printf("Message du client: %s\n", buffer);



        // Envoi des données au deuxième routeur

        write(router2_sockfd, buffer, n);

        

        // Lecture de la réponse du deuxième routeur

        n = read(router2_sockfd, buffer, sizeof(buffer));

        if (n > 0) {

            printf("Réponse du  routeur: %s\n", buffer);

            // Envoi de la réponse au client

            write(client_sockfd, buffer, n);

        }

    }

}



// Fonction principale

int main() {

    int router1_sockfd, router2_sockfd, client_sockfd;

    struct sockaddr_in router2_addr, client_addr;

    socklen_t client_len = sizeof(client_addr);



    // Création du socket du premier routeur pour écouter les connexions du client

    router1_sockfd = create_socket(CLIENT_PORT);

    if (listen(router1_sockfd, 5) < 0) {

        perror("Erreur d'écoute");

        close(router1_sockfd);

        exit(EXIT_FAILURE);

    }

    printf("Routeur en écoute sur le port %d...\n", CLIENT_PORT);



    // Accepter la connexion du client

    client_sockfd = accept(router1_sockfd, (struct sockaddr*)&client_addr, &client_len);

    if (client_sockfd < 0) {

        perror("Erreur d'acceptation de connexion du client");

        close(router1_sockfd);

        exit(EXIT_FAILURE);

    }

    printf("Client connecté...\n");



    // Création du socket pour se connecter au deuxième routeur

    router2_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (router2_sockfd == -1) {

        perror("Erreur de création du socket pour router");

        close(router1_sockfd);

        close(client_sockfd);

        exit(EXIT_FAILURE);

    }



    router2_addr.sin_family = AF_INET;

    router2_addr.sin_port = htons(ROUTER2_PORT);

    router2_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Adresse du deuxième routeur



    if (connect(router2_sockfd, (struct sockaddr*)&router2_addr, sizeof(router2_addr)) < 0) {

        perror("Erreur de connexion au routeur");

        close(router1_sockfd);

        close(client_sockfd);

        close(router2_sockfd);

        exit(EXIT_FAILURE);

    }

    printf("Connecté au routeur...\n");



    // Transfert des données entre le client et le deuxième routeur via le premier routeur

    transfer_data(client_sockfd, router2_sockfd);



    // Fermeture des connexions

    close(client_sockfd);

    close(router2_sockfd);

    close(router1_sockfd);



    return 0;

}