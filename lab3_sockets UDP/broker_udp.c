#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9090
#define MAX_CLIENTS 20
#define BUFFER_SIZE 1024

typedef struct {
    struct sockaddr_in addr;
    int is_publisher;
    int active;
} Client;

int main() {
    int sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    Client clients[MAX_CLIENTS] = {0};

    // Crear socket UDP
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creando socket");
        exit(EXIT_FAILURE);
    }

    // Configurar dirección
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind");
        exit(EXIT_FAILURE);
    }

    printf("Broker UDP escuchando en puerto %d...\n", PORT);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        // Recibir cualquier datagrama
        int n = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                         (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Error en recvfrom");
            continue;
        }
        buffer[n] = '\0';

        // Buscar o registrar cliente
        int found = -1;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].active &&
                clients[i].addr.sin_addr.s_addr == client_addr.sin_addr.s_addr &&
                clients[i].addr.sin_port == client_addr.sin_port) {
                found = i;
                break;7
            }
        }

        if (found == -1) {
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (!clients[i].active) {
                    clients[i].addr = client_addr;
                    clients[i].active = 1;
                    clients[i].is_publisher = 0;
                    found = i;
                    break;
                }
            }
        }

        if (found == -1) continue;

        // Registrar tipo de cliente
        if (strncmp(buffer, "PUBLISHER", 9) == 0) {
            clients[found].is_publisher = 1;
            printf(" Publisher registrado\n");
        } 
        else if (strncmp(buffer, "SUBSCRIBER", 10) == 0) {
            clients[found].is_publisher = 0;
            printf(" Subscriber registrado\n");
        } 
        else {
            // Es un mensaje de un publisher 
            if (clients[found].is_publisher) {
                
                // Enviar a cada suscriptor 
                for (int j = 0; j < MAX_CLIENTS; j++) {
                    if (clients[j].active && !clients[j].is_publisher) {
                        // Simplemente envía y continúa
                        sendto(sock, buffer, strlen(buffer), 0,
                               (struct sockaddr *)&clients[j].addr,
                               sizeof(clients[j].addr));
                    }
                }
                printf(" Mensaje reenviado a %d suscriptor(es)\n", 
                       (int)(clients[found].is_publisher ? 0 : 1));
            }
        }
    }

    close(sock);
    return 0;
}