#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX 1024
#define PORT 9090

// Fonction pour calculer le CRC
void appendCRC(const char* input, const char* generator, char* output) {
    int inputLen = strlen(input);
    int generatorLen = strlen(generator);
    char temp[100]; // Tampon temporaire pour calculer le CRC

    strcpy(temp, input); // Copie l'entrée dans le tampon temporaire
    for (int i = 0; i < generatorLen - 1; i++) {
        strcat(temp, "0"); // Ajoute des zéros pour l'espace CRC
    }

    strcpy(output, temp); // Initialise la sortie avec le tampon temporaire
    for (int i = 0; i <= strlen(output) - generatorLen; i++) {
        if (output[i] == '1') {
            for (int j = 0; j < generatorLen; j++) {
                output[i + j] = (output[i + j] == generator[j]) ? '0' : '1';
            }
        }
    }

    // Ajoute le CRC calculé à l'entrée initiale
    strncpy(temp, input, inputLen); // Copie l'entrée initiale dans le tampon temporaire
    strcpy(temp + inputLen, output + inputLen); // Ajoute le CRC calculé
    strcpy(output, temp); // Met à jour la sortie finale
}

// Fonction principale du client
int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAX];
    char message[100];
    char generator[] = "10011"; // Polynôme générateur pour le CRC
    char messageWithCRC[200];

    // Création du socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Erreur de création du socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket client créé avec succès...\n");

    // Initialisation de l'adresse du serveur
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connexion au routeur
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        perror("Erreur de connexion au routeur");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Connecté au routeur...\n");

    // Boucle de communication avec le routeur
    while (1) {
        memset(buffer, 0, sizeof(buffer)); // Réinitialiser le tampon
        memset(messageWithCRC, 0, sizeof(messageWithCRC)); // Réinitialiser le message avec CRC

        printf("Entrez un message binaire (ou 'exit' pour quitter) : ");
        scanf("%s", message);

        // Vérifier si le message est "exit"
        if (strcmp(message, "exit") == 0) {
            write(sockfd, message, strlen(message));
            printf("Fermeture du client...\n");
            break;
        }

        // Valider si le message est binaire
        bool isBinary = true;
        for (int i = 0; i < strlen(message); i++) {
            if (message[i] != '0' && message[i] != '1') {
                isBinary = false;
                break;
            }
        }

        if (!isBinary) {
            printf("Erreur : Le message doit être binaire (composé uniquement de 0 et 1).\n");
            continue;
        }

        // Ajouter le CRC au message
        appendCRC(message, generator, messageWithCRC);

        // Envoyer le message avec CRC au routeur
        write(sockfd, messageWithCRC, strlen(messageWithCRC));
        printf("Message envoyé avec CRC : %s\n", messageWithCRC);

        // Lire la réponse du serveur (via le routeur)
        read(sockfd, buffer, sizeof(buffer));
        printf("Réponse du serveur : %s\n", buffer);
    }

    // Fermer le socket
    close(sockfd);
    return 0;
}