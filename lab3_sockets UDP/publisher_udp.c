#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 9090
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in broker_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(broker_addr);

    // Crear socket UDP
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creando socket");
        return -1;
    }

    // Configurar dirección del broker
    memset(&broker_addr, 0, sizeof(broker_addr));
    broker_addr.sin_family = AF_INET;
    broker_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &broker_addr.sin_addr);

    // Identificarse como PUBLISHER
    strcpy(buffer, "PUBLISHER");
    sendto(sock, buffer, strlen(buffer), 0,
           (struct sockaddr *)&broker_addr, addr_len);
    
    sleep(1);

    // Enviar mensajes
    srand(time(NULL));
    for (int i = 1; i <= 10; i++) {
        memset(buffer, 0, BUFFER_SIZE);
        
        int id = getpid() % 1000;
        int tipo = rand() % 3;
        
        if (tipo == 0) {
            sprintf(buffer, "Publisher %d -> Gol de Equipo A al minuto %d", id, i * 5);
        } else if (tipo == 1) {
            sprintf(buffer, "Publisher %d -> Cambio: jugador %d entra por jugador %d", id, i, i + 10);
        } else {
            sprintf(buffer, "Publisher %d -> Tarjeta amarilla al número %d", id, i);
        }
        
        sendto(sock, buffer, strlen(buffer), 0,
               (struct sockaddr *)&broker_addr, addr_len);
        
        printf("Enviado: %s\n", buffer);
        
        sleep(2);  
    }
    
    printf("Todos los mensajes enviados.\n");
    close(sock);
    return 0;
}