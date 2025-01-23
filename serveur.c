
#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>



#define MAX 1024

#define PORT 8080

#define SA struct sockaddr



void binaryToText(const char* binary, char* text) {
    int len = strlen(binary);
    int textIndex = 0;

    // Par défaut, le texte est une chaîne vide
    text[0] = '\0';

    // Vérifiez si la longueur du binaire est un multiple de 8
    int padding = len % 8;
    if (padding != 0) {
        // Ajouter des zéros pour rendre la longueur un multiple de 8
        padding = 8 - padding;
        char paddedBinary[MAX];
        strcpy(paddedBinary, binary);
        
        for (int i = 0; i < padding; i++) {
            paddedBinary[len + i] = '0'; // Ajouter les zéros à la fin
        }
        paddedBinary[len + padding] = '\0'; // Terminer la chaîne

        // Mettre à jour la chaîne binaire à traiter
        binary = paddedBinary;
        len = strlen(binary); // Mettre à jour la longueur après ajout des zéros
    }

    // Traiter chaque bloc de 8 bits
    for (int i = 0; i < len; i += 8) {
        char byte[9] = {0}; // Un octet
        strncpy(byte, binary + i, 8); // Copier les 8 bits

        // Convertir l'octet binaire en caractère ASCII
        char ch = (char) strtol(byte, NULL, 2); // strtol convertit le binaire en entier, puis en caractère
        text[textIndex++] = ch; // Ajouter le caractère au texte
    }

    text[textIndex] = '\0'; // Terminer la chaîne de caractères
}


// Fonction pour vérifier si une chaîne est binaire

int isBinary(const char* message) {

    for (int i = 0; message[i] != '\0'; i++) {

        if (message[i] != '0' && message[i] != '1') {

            return 0; // Retourne faux si un caractère non binaire est trouvé

        }

    }

    return 1; // Retourne vrai si le message est strictement binaire

}



// Fonction pour vérifier le CRC

int verifyCRC(const char* input, const char* generator) {

    char temp[100];

    strcpy(temp, input);

    int inputLen = strlen(temp);

    int generatorLen = strlen(generator);



    for (int i = 0; i <= inputLen - generatorLen; i++) {

        if (temp[i] == '1') {

            for (int j = 0; j < generatorLen; j++) {

                temp[i + j] = (temp[i + j] == generator[j]) ? '0' : '1';

            }

        }

    }



    for (int i = inputLen - generatorLen + 1; i < inputLen; i++) {

        if (temp[i] == '1') {

            return 0; // Si une erreur est détectée dans la vérification CRC

        }

    }

    return 1; // Si le CRC est valide

}



// Fonction principale du serveur

int main() {

    int sockfd, connfd;

    struct sockaddr_in servaddr, cli;

    char buffer[MAX];

    char generator[] = "10011"; // Polynôme générateur pour le CRC



    // Création du socket du serveur

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {

        perror("Erreur de création du socket");

        exit(EXIT_FAILURE);

    }

    printf("Socket serveur créé...\n");



    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;

    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    servaddr.sin_port = htons(PORT);



    // Lier le socket à l'adresse et au port du serveur

    if (bind(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {

        perror("Erreur de bind");

        close(sockfd);

        exit(EXIT_FAILURE);

    }



    // Écouter les connexions entrantes

    if (listen(sockfd, 5) != 0) {

        perror("Erreur d'écoute");

        close(sockfd);

        exit(EXIT_FAILURE);

    }

    printf("Serveur en écoute sur le port %d...\n", PORT);



    // Accepter les connexions du routeur

    socklen_t len = sizeof(cli);

    connfd = accept(sockfd, (SA*)&cli, &len);

    if (connfd < 0) {

        perror("Erreur d'acceptation de connexion");

        close(sockfd);

        exit(EXIT_FAILURE);

    }



    // Réception du message

    while (1) {

        memset(buffer, 0, sizeof(buffer));

        read(connfd, buffer, sizeof(buffer));

        if (strcmp(buffer, "exit") == 0) {

            printf("Fermeture du serveur...\n");

            break;

        }



        // Vérification de la validité du message (binaire)

        if (isBinary(buffer)) {

            // Vérification du CRC

            if (verifyCRC(buffer, generator)) {

                printf("Message reçu et CRC vérifié : %s\n", buffer);
                        // Convertir le message binaire en texte
              char realMessage[MAX];
              if (isBinary(buffer)) {
                  binaryToText(buffer, realMessage);
                  printf("Message réel (texte) : %s\n", realMessage);
              }

                write(connfd, "CRC valide", strlen("CRC valide"));

            } else {

                printf("Erreur CRC détectée dans le message : %s\n", buffer);

                write(connfd, "Erreur CRC", strlen("Erreur CRC"));

            }

        } else {

            printf("Message non valide : %s\n", buffer);

            write(connfd, "Message non valide", strlen("Message non valide"));

        }

    }



    // Fermeture des connexions

    close(connfd);

    close(sockfd);

    return 0;

}